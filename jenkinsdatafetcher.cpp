#include "jenkinsdatafetcher.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "jenkinspluginconstants.h"

using namespace JenkinsPlugin::Internal;

const QString JenkinsDataFetcher::REST_API_URL_SUFFIX = QStringLiteral("api/json");

JenkinsDataFetcher::JenkinsDataFetcher(QObject *parent) : QObject(parent)
{
    _manager = new QNetworkAccessManager(this);
    connect(_manager, &QNetworkAccessManager::finished, this, &JenkinsDataFetcher::readReply);
    _timer = new QTimer(this);
    _timer->setInterval(1000 * 1);  // 30 seconds
    connect(_timer, &QTimer::timeout, this, &JenkinsDataFetcher::updateJobInfo);
    _timer->start();
}

JenkinsSettings JenkinsDataFetcher::jenkinsSettings() const { return _jenkinsSettings; }

void JenkinsDataFetcher::setJenkinsSettings(const JenkinsSettings &jenkinsSettings)
{
    _jenkinsSettings = jenkinsSettings;
}

void JenkinsDataFetcher::getAvaliableJobs()
{
    QString queryUrl = urlToRestApiUrl(_jenkinsSettings.jenkinsUrl());
    QNetworkRequest request{QUrl(queryUrl)};
    _timer->stop();
    _manager->get(request);
}

QString JenkinsDataFetcher::urlToRestApiUrl(const QString &url)
{
    if (url.endsWith(REST_API_URL_SUFFIX))
        return url;
    else if (url.endsWith(QStringLiteral("/")))
        return url + REST_API_URL_SUFFIX;
    else
        return url + QStringLiteral("/") + REST_API_URL_SUFFIX;
}

void JenkinsDataFetcher::readReply(QNetworkReply *reply)
{
    qDebug() << "reply:" << reply->url();
    switch (_state)
    {
        case State::Ready:
            return;  // no tasks scheduled
            break;
        case State::FetchingJobs:
        {
            QList< JenkinsJob > jobs = fetchJobList(reply);
            if (jobs.isEmpty())
            {
                _state = State::Ready;
                emit jobsUpdated(jobs);
            }
            else
            {
                _state = State::FetchingJobDetails;
                _jobsForDetalization = jobs;
                sendDetailsRequestForFirstJob();
            }
        }
        break;
        case State::FetchingJobDetails:
        {
            JenkinsJob detailedJob;
            if (reply->error() != QNetworkReply::NoError)
            {
                //FIXME: Maybe better to retry request?
                detailedJob = _jobsForDetalization.takeFirst();
            }
            else
            {
                detailedJob = fillBuildDetails(reply);
            }

            if (detailedJob.isValid())
                emit jobUpdated(detailedJob);
            if (_jobsForDetalization.isEmpty())
            {
                _state = State::Ready;
                _timer->start();
            }
            else
                sendDetailsRequestForFirstJob();
        }
        break;
        default:
            break;
    }
}

void JenkinsDataFetcher::updateJobInfo()
{
    if (_state == State::Ready)
    {
        _state = State::FetchingJobs;
        getAvaliableJobs();
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
    QString buildUrl = jsonObject[QStringLiteral("url")].toString();
    // TODO: maybe better to use URL from reply?
    QString expectedJobUrl = buildUrlToJobUrl(buildUrl);

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

    // fill build info
    BuildInfo info;
    info.setUrl(buildUrl);
    if (jsonObject.contains(QStringLiteral("number")))
        info.setNumber(jsonObject[QStringLiteral("number")].toInt());
    if (jsonObject.contains(QStringLiteral("timestamp")))
        info.setTimestamp(jsonObject[QStringLiteral("timestamp")].toVariant().toULongLong());

    if (jsonObject.contains(QStringLiteral("result")))
        info.setResult(jsonObject[QStringLiteral("result")].toString());
    if (jsonObject.contains(QStringLiteral("displayName")))
        info.setDisplayName(jsonObject[QStringLiteral("displayName")].toString());
    if (jsonObject.contains(QStringLiteral("fullDisplayName")))
        info.setFullDisplayName(jsonObject[QStringLiteral("fullDisplayName")].toString());
    if (jsonObject.contains(QStringLiteral("description")))
        info.setDescription(jsonObject[QStringLiteral("description")].toString());
    if (jsonObject.contains(QStringLiteral("duration")))
        info.setDuration(jsonObject[QStringLiteral("duration")].toInt());
    job.setBuildInfo(info);
    return job;
}

QString JenkinsDataFetcher::cutRestApiUrlPart(QString url)
{
    if (url.endsWith(REST_API_URL_SUFFIX))
        url.chop(REST_API_URL_SUFFIX.size());
    return url;
}

//! remove build number from build url
QString JenkinsDataFetcher::buildUrlToJobUrl(QString buildUrl)
{
    QString localBuildUrl = buildUrl;
    if (localBuildUrl.endsWith(QStringLiteral("/")))
        localBuildUrl.chop(1);
    int lastSlashIndex = localBuildUrl.lastIndexOf(QLatin1Char('/'));
    if (lastSlashIndex == -1)
        return buildUrl;
    // remove all after last '/'
    int startIndex = lastSlashIndex + 1;
    int length = localBuildUrl.size() - startIndex;
    localBuildUrl.chop(length);
    return localBuildUrl;
}

void JenkinsDataFetcher::sendDetailsRequestForFirstJob()
{
    if (_jobsForDetalization.isEmpty())
        return;
    QString queryUrl
        = urlToRestApiUrl(_jobsForDetalization.at(0).jobUrl() + QStringLiteral("lastBuild/"));
    QNetworkRequest request{QUrl(queryUrl)};
    _manager->get(request);
}

QString JenkinsJob::jobUrl() const { return _jobUrl; }

void JenkinsJob::setJobUrl(const QString &jobUrl) { _jobUrl = jobUrl; }

QString JenkinsJob::name() const { return _name; }

void JenkinsJob::setName(const QString &name) { _name = name; }

void JenkinsJob::setBuildStatus(const QString &colorEntry)
{
    QString entry = colorEntry;
    if(entry.endsWith(QStringLiteral("_anime")))
    {
        _isRunning = true;
        entry.chop(6);
    }
    else
        _isRunning = false;
    if(entry == QStringLiteral("blue"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::SUCCESS_ICON);
    else if(entry == QStringLiteral("red"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::FAIL_ICON);
    else if(entry == QStringLiteral("yellow"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::UNSTABLE_ICON);
    else
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::NOT_BUILT_ICON);
}

BuildInfo JenkinsJob::buildInfo() const { return _buildInfo; }

void JenkinsJob::setBuildInfo(const BuildInfo &buildInfo) { _buildInfo = buildInfo; }

bool JenkinsJob::isRunning() const
{
    return _isRunning;
}

QString JenkinsJob::colorIcon() const
{
    return _colorIcon;
}

QString BuildInfo::url() const { return _url; }

void BuildInfo::setUrl(const QString &url) { _url = url; }

int BuildInfo::number() const { return _number; }

void BuildInfo::setNumber(int number) { _number = number; }

QDateTime BuildInfo::timestamp() const { return _timestamp; }

void BuildInfo::setTimestamp(const QDateTime &timestamp) { _timestamp = timestamp; }

void BuildInfo::setTimestamp(const qint64 timestamp)
{
    _timestamp = QDateTime::fromMSecsSinceEpoch(timestamp);
}

BuildInfo::Result BuildInfo::result() const { return _result; }

QString BuildInfo::getResultIcon()
{
    switch (_result)
    {
        case Result::Success:
            return QLatin1String(JenkinsPlugin::Constants::SUCCESS_ICON);
            break;
        case Result::Failure:
            return QLatin1String(JenkinsPlugin::Constants::FAIL_ICON);
            break;
        case Result::Unstable:
            return QLatin1String(JenkinsPlugin::Constants::UNSTABLE_ICON);
            break;
        default:
            return QLatin1String(JenkinsPlugin::Constants::NOT_BUILT_ICON);
            break;
    }
}

void BuildInfo::setResult(const Result &result) { _result = result; }

void BuildInfo::setResult(const QString &result)
{
    if (result == QStringLiteral("SUCCESS"))
        _result = Result::Success;
    else if (result == QStringLiteral("ABORTED"))
        _result = Result::Aborted;
    else if (result == QStringLiteral("FAILURE"))
        _result = Result::Failure;
    else if (result == QStringLiteral("NOT_BUILT"))
        _result = Result::NotBuilt;
    else if (result == QStringLiteral("UNSTABLE"))
        _result = Result::Unstable;
    else
        _result = Result::Unknown;
}

QString BuildInfo::displayName() const { return _displayName; }

void BuildInfo::setDisplayName(const QString &displayName) { _displayName = displayName; }

QString BuildInfo::fullDisplayName() const { return _fullDisplayName; }

void BuildInfo::setFullDisplayName(const QString &fullDisplayName)
{
    _fullDisplayName = fullDisplayName;
}

QString BuildInfo::description() const { return _description; }

void BuildInfo::setDescription(const QString &description) { _description = description; }

int BuildInfo::duration() const { return _duration; }

void BuildInfo::setDuration(int duration) { _duration = duration; }
