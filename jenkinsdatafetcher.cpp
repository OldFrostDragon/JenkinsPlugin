#include "jenkinsdatafetcher.h"

#include <QDebug>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "jenkinspluginconstants.h"

using namespace JenkinsPlugin::Internal;

JenkinsDataFetcher::JenkinsDataFetcher(std::shared_ptr< RestRequestBuilder > restRequestBuilder,
                                       QObject *parent)
    : QObject(parent)
{
    _restRequestBuilder = restRequestBuilder;
    connect(_restRequestBuilder.get(), &RestRequestBuilder::settingsChanged, this,
            &JenkinsDataFetcher::resetFetchProgress);

    _manager = new QNetworkAccessManager(this);
    connect(_manager, &QNetworkAccessManager::finished, this, &JenkinsDataFetcher::readReply);

    _timer = new QTimer(this);
    _timer->setInterval(1000 * 10);  // 10 seconds
    connect(_timer, &QTimer::timeout, this, &JenkinsDataFetcher::switchToNextFetchStep);
    _timer->start();
}

void JenkinsDataFetcher::getAvaliableJobs()
{
    QNetworkRequest request = _restRequestBuilder->buildAvaliableJobsRequest();
    _manager->get(request);
}

void JenkinsDataFetcher::resetFetchProgress()
{
    // TODO: update for new host only?
    _jobsForDetalization.clear();
    _jobsForLastBuildDetalization.clear();
    _state = State::Ready;
    switchToNextFetchStep();
}

void JenkinsDataFetcher::readReply(QNetworkReply *reply)
{
    switch (_state)
    {
        case State::Ready:
            return;  // no tasks scheduled
            break;
        case State::FetchingJobs:
        {
            QList< JenkinsJob > jobs = fetchJobList(reply);
            _jobsForDetalization = jobs;
            switchToNextFetchStep();
        }
        break;
        case State::FetchingJobDetails:
        {
            JenkinsJob detailedJob;
            if (reply->error() != QNetworkReply::NoError)
            {
                // TODO: Maybe better to retry request?
                detailedJob = _jobsForDetalization.takeFirst();
            }
            else
            {
                detailedJob = fillBuildDetails(reply);
            }
            _jobsForLastBuildDetalization.append(detailedJob);
            switchToNextFetchStep();
        }
        break;
        case State::FetchingLastBuild:
        {
            JenkinsJob detailedJob;
            if (reply->error() != QNetworkReply::NoError)
            {
                // TODO: Maybe better to retry request?
                detailedJob = _jobsForLastBuildDetalization.takeFirst();
            }
            else
            {
                detailedJob = fillLastBuildInfoDetails(reply);
            }
            if (detailedJob.isValid())
                emit jobUpdated(detailedJob);
            switchToNextFetchStep();
        }
        break;
        default:
            break;
    }
}

void JenkinsDataFetcher::switchToNextFetchStep()
{
    switch (_state)
    {
        case State::Ready:
        {
            qDebug() << "transition:"
                     << "Ready -> Fetching jobs";
            _timer->stop();
            _state = State::FetchingJobs;
            getAvaliableJobs();
        }
        break;
        case State::FetchingJobs:
        {
            if (_jobsForDetalization.isEmpty())
            {
                qDebug() << "transition:"
                         << "Fetching jobs -> Ready";
                _state = State::Ready;
                _timer->start();
            }
            else
            {
                qDebug() << "transition:"
                         << "Fetching jobs -> Fetching details";
                _state = State::FetchingJobDetails;
                sendDetailsRequestForFirstJob();
            }
        }
        break;
        case State::FetchingJobDetails:
        {

            if (_jobsForDetalization.isEmpty())
            {
                if (_jobsForLastBuildDetalization.isEmpty())
                {
                    qDebug() << "transition:"
                             << "Fetching details -> Ready";
                    _state = State::Ready;
                    _timer->start();
                }
                else
                {
                    _state = State::FetchingLastBuild;
                    qDebug() << "transition:"
                             << "Fetching details -> FetchingLastBuild";
                    sendLastBuildInfoForFirstJob();
                }
            }
            else
                sendDetailsRequestForFirstJob();
        }
        break;
        case State::FetchingLastBuild:
        {
            if (_jobsForLastBuildDetalization.isEmpty())
            {
                qDebug() << "transition:"
                         << "FetchingLastBuild -> Ready";
                _state = State::Ready;
                _timer->start();
            }
            else
                sendLastBuildInfoForFirstJob();
        }
        break;
        default:
            qDebug() << "unknown state";
            break;
    }
}

QList< JenkinsJob > JenkinsDataFetcher::fetchJobList(QNetworkReply *reply)
{
    if (reply == nullptr)
    {
        qDebug() << "reply is null";
        return {};
    }
    QByteArray data = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull())
    {
        qDebug() << "json is null";
        return {};
    }

    QJsonObject jsonObject = jsonDoc.object();
    if (!jsonObject.contains(QStringLiteral("jobs")))
    {
        qDebug() << "jobs are absent";
        return {};
    }
    QJsonArray jobsArray = jsonObject[QStringLiteral("jobs")].toArray();
    QList< JenkinsJob > parsedJobs;
    foreach (auto job, jobsArray)
    {
        QJsonObject jobObject = job.toObject();
        JenkinsJob jenkinsJob;
        if (jobObject.contains(QStringLiteral("name")))
            jenkinsJob.setName(jobObject[QStringLiteral("name")].toString());
        if (jobObject.contains(QStringLiteral("url")))
            jenkinsJob.setJobUrl(jobObject[QStringLiteral("url")].toString());
        if (jobObject.contains(QStringLiteral("color")))
            jenkinsJob.setBuildStatus(jobObject[QStringLiteral("color")].toString());
        parsedJobs.append(jenkinsJob);
    }
    return parsedJobs;
}

void JenkinsDataFetcher::readHealthReportsPartFor(QJsonObject &jsonObject, JenkinsJob &job)
{
    if (jsonObject.contains(QStringLiteral("healthReport")))
    {
        QJsonArray healthArray = jsonObject[QStringLiteral("healthReport")].toArray();
        QList< HealthReport > reportList;
        foreach (auto item, healthArray)
        {
            HealthReport report;
            QJsonObject healthObject = item.toObject();
            if (healthObject.contains(QStringLiteral("score")))
                report.setScore(healthObject[QStringLiteral("score")].toInt());
            if (healthObject.contains(QStringLiteral("description")))
                report.setDescription(healthObject[QStringLiteral("description")].toString());
            if (healthObject.contains(QStringLiteral("iconClassName")))
                report.setIconClassName(healthObject[QStringLiteral("iconClassName")].toString());
            reportList.append(report);
        }
        job.setHealthReports(reportList);
    }
}

void JenkinsDataFetcher::readBuildsListFor(JenkinsJob &job, QJsonObject &jsonObject)
{
    if (jsonObject.contains(QStringLiteral("builds")))
    {
        QJsonArray buildsArray = jsonObject[QStringLiteral("builds")].toArray();
        QList< JenkinsJob::BuildUrl > urls;
        foreach (auto build, buildsArray)
        {
            QJsonObject buildObject = build.toObject();
            if (buildObject.contains(QStringLiteral("number"))
                && buildObject.contains(QStringLiteral("url")))
            {
                int number = buildObject[QStringLiteral("number")].toInt();
                QString url = buildObject[QStringLiteral("url")].toString();
                urls.append(JenkinsJob::BuildUrl(number, url));
            }
        }
        job.setBuildUrls(urls);
    }
}

void JenkinsDataFetcher::readBuildableFlagFor(JenkinsJob &job, QJsonObject &jsonObject)
{
    if (jsonObject.contains(QStringLiteral("buildable")))
        job.setIsBuildable(jsonObject[QStringLiteral("buildable")].toBool());
    else
        job.setIsBuildable(true);
}

void JenkinsDataFetcher::readIsQueuedFlagFor(QJsonObject &jsonObject, JenkinsJob &job)
{
    if (jsonObject.contains(QStringLiteral("inQueue")))
        job.setIsQueued(jsonObject[QStringLiteral("inQueue")].toBool());
    else
        job.setIsQueued(false);
}

void JenkinsDataFetcher::readBuildInfoFor(JenkinsJob &job, QJsonObject &jsonObject)
{
    readHealthReportsPartFor(jsonObject, job);
    readBuildsListFor(job, jsonObject);
    readBuildableFlagFor(job, jsonObject);
    readIsQueuedFlagFor(jsonObject, job);
}

/*!
 * \brief JenkinsDataFetcher::fillBuildDetails get build details for Jenkins Job and remove
 * it from list of jobs, that require detalization. It search appropriate job using URL
 * \param reply - JSON with details about last build
 * \return JenkinsJob with detailed information about last build
 */
JenkinsJob JenkinsDataFetcher::fillBuildDetails(QNetworkReply *reply)
{
    JenkinsJob job;
    if (reply == nullptr)
    {
        qDebug() << Q_FUNC_INFO << "reply is null";
        return job;
    }

    QByteArray data = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull())
    {
        qDebug() << Q_FUNC_INFO << "json is null";
        return job;
    }

    QJsonObject jsonObject = jsonDoc.object();
    if (!jsonObject.contains(QStringLiteral("url")))
    {
        return job;
    }
    // TODO: maybe better to use URL from reply?
    QString expectedJobUrl = jsonObject[QStringLiteral("url")].toString();

    // find Job to detalize
    int jobIndex = -1;
    for (int i = 0; i < _jobsForDetalization.size(); ++i)
    {
        if (_jobsForDetalization[i].jobUrl() == expectedJobUrl)
        {
            jobIndex = i;
            break;
        }
    }
    if (jobIndex == -1)
    {
        qDebug() << "failed to find job with URL" << expectedJobUrl;
        return job;
    }
    job = _jobsForDetalization.takeAt(jobIndex);

    readBuildInfoFor(job, jsonObject);
    return job;
}

JenkinsJob JenkinsDataFetcher::fillLastBuildInfoDetails(QNetworkReply *reply)
{
    // TODO: these checks are copy-pasted from fillBuildDetails
    JenkinsJob job;
    if (reply == nullptr)
    {
        qDebug() << Q_FUNC_INFO << "reply is null";
        return job;
    }

    QByteArray data = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull())
    {
        qDebug() << Q_FUNC_INFO << "json is null";
        return job;
    }

    QJsonObject jsonObject = jsonDoc.object();
    if (!jsonObject.contains(QStringLiteral("url")))
    {
        return job;
    }
    // TODO: maybe better to use URL from reply?
    QString expectedJobUrl = jsonObject[QStringLiteral("url")].toString();
    expectedJobUrl = _restRequestBuilder->lastBuildUrlToJobUrl(expectedJobUrl);

    // TODO: search copy-pasted from fillBuildDetails
    // find Job to detalize
    int jobIndex = -1;
    for (int i = 0; i < _jobsForLastBuildDetalization.size(); ++i)
    {
        if (_jobsForLastBuildDetalization[i].jobUrl() == expectedJobUrl)
        {
            jobIndex = i;
            break;
        }
    }
    if (jobIndex == -1)
    {
        qDebug() << "failed to find job with URL" << expectedJobUrl;
        return job;
    }

    job = _jobsForLastBuildDetalization.takeAt(jobIndex);
    if (jsonObject.contains(QStringLiteral("timestamp")))
    {
        job.setLastBuildDate(jsonObject[QStringLiteral("timestamp")].toVariant().toULongLong());
    }
    return job;
}

void JenkinsDataFetcher::sendDetailsRequestForFirstJob()
{
    if (_jobsForDetalization.isEmpty())
        return;
    QString queryUrl = _restRequestBuilder->urlToRestApiUrl(
        _jobsForDetalization.at(0).jobUrl() /*+ QStringLiteral("lastBuild/")*/);
    QNetworkRequest request = _restRequestBuilder->buildRequest(queryUrl);
    _manager->get(request);
}

void JenkinsDataFetcher::sendLastBuildInfoForFirstJob()
{
    if (_jobsForLastBuildDetalization.isEmpty())
        return;
    QNetworkRequest request = _restRequestBuilder->buildLastBuildInfoRequest(
        _jobsForLastBuildDetalization.at(0).jobUrl());
    _manager->get(request);
}
