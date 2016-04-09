#include "jenkinsdatafetcher.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "jenkinspluginconstants.h"

using namespace JenkinsPlugin::Internal;

const QMap< QString, QString > HealthReport::ICON_CLASS_ICONS = {
    {QStringLiteral("icon-health-80plus"), QLatin1String(JenkinsPlugin::Constants::HEALTH_80PLUS)},
    {QStringLiteral("icon-health-60to79"), QLatin1String(JenkinsPlugin::Constants::HEALTH_60TO79)},
    {QStringLiteral("icon-health-40to59"), QLatin1String(JenkinsPlugin::Constants::HEALTH_40TO59)},
    {QStringLiteral("icon-health-20to39"), QLatin1String(JenkinsPlugin::Constants::HEALTH_20TO39)},
    {QStringLiteral("icon-health-00to19"), QLatin1String(JenkinsPlugin::Constants::HEALTH_00TO19)},
};

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
                qDebug() << "transition:"
                         << "Fetching details -> Ready";
                _state = State::Ready;
                _timer->start();
            }
            else
                sendDetailsRequestForFirstJob();
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
    QString expectedJobUrl = _restRequestBuilder->buildUrlToJobUrl(buildUrl);

    qDebug() << expectedJobUrl;
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

QString JenkinsJob::jobUrl() const { return _jobUrl; }

void JenkinsJob::setJobUrl(const QString &jobUrl) { _jobUrl = jobUrl; }

QString JenkinsJob::name() const { return _name; }

void JenkinsJob::setName(const QString &name) { _name = name; }

void JenkinsJob::setBuildStatus(const QString &colorEntry)
{
    QString entry = colorEntry;
    if (entry.endsWith(QStringLiteral("_anime")))
    {
        _isRunning = true;
        entry.chop(6);
    }
    else
        _isRunning = false;
    if (entry == QStringLiteral("blue"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::SUCCESS_ICON);
    else if (entry == QStringLiteral("red"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::FAIL_ICON);
    else if (entry == QStringLiteral("yellow"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::UNSTABLE_ICON);
    else
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::NOT_BUILT_ICON);
}

bool JenkinsJob::isRunning() const { return _isRunning; }

QString JenkinsJob::colorIcon() const { return _colorIcon; }

QList< HealthReport > JenkinsJob::healthReports() const { return _healthReports; }

void JenkinsJob::setHealthReports(const QList< HealthReport > &healthReports)
{
    _healthReports = healthReports;
    QString iconPath;
    int score = 1000;
    foreach (const HealthReport &report, _healthReports)
    {
        if (report.score() < score)
        {
            score = report.score();
            iconPath = report.getIconFile();
        }
    }
    if (!iconPath.isEmpty())
        _healthIcon = QIcon(iconPath);
    else
        _healthIcon = QIcon();
}

QIcon JenkinsJob::healthIcon() const { return _healthIcon; }

QList< JenkinsJob::BuildUrl > JenkinsJob::buildUrls() const { return _buildUrls; }

void JenkinsJob::setBuildUrls(const QList< BuildUrl > &buildUrls) { _buildUrls = buildUrls; }


HealthReport::HealthReport(const int score, const QString &description,
                           const QString &iconClassName)
    : _score(score), _description(description), _iconClassName(iconClassName)
{
}

QString HealthReport::description() const { return _description; }

void HealthReport::setDescription(const QString &description) { _description = description; }

int HealthReport::score() const { return _score; }

void HealthReport::setScore(int score) { _score = score; }

QString HealthReport::iconClassName() const { return _iconClassName; }

void HealthReport::setIconClassName(const QString &iconClassName)
{
    _iconClassName = iconClassName;
}
