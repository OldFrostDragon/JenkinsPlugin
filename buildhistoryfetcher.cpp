#include "buildhistoryfetcher.h"

#include "jenkinspluginconstants.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

using namespace JenkinsPlugin::Internal;

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

QString BuildInfo::getResultIcon() const
{
    switch (_result)
    {
        case Result::Success:
            return QLatin1String(JenkinsPlugin::Constants::SUCCESS_ICON);
        case Result::Failure:
            return QLatin1String(JenkinsPlugin::Constants::FAIL_ICON);
        case Result::Unstable:
            return QLatin1String(JenkinsPlugin::Constants::UNSTABLE_ICON);
        case Result::NotBuilt:
        case Result::Aborted:
        case Result::Unknown:
            return QLatin1String(JenkinsPlugin::Constants::NOT_BUILT_ICON);
    }
    return QLatin1String(JenkinsPlugin::Constants::NOT_BUILT_ICON);
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

BuildHistoryFetcher::BuildHistoryFetcher(std::shared_ptr< RestRequestBuilder > restRequestBuilder,
                                         QObject *parent)
    : QObject(parent)
{
    _manager = std::make_unique< QNetworkAccessManager >();
    _restRequestBuilder = restRequestBuilder;

    connect(_manager.get(), &QNetworkAccessManager::finished, this,
            &BuildHistoryFetcher::readBuildInfoReply);
    connect(this, &BuildHistoryFetcher::nextBuildInfoFetchRequested, this,
            &BuildHistoryFetcher::processFirstPendingUrl);
}

void BuildHistoryFetcher::fetchBuildHistory(QStringList urls)
{
    _urlsToFetch = urls;
    processFirstPendingUrl();
}

void BuildHistoryFetcher::readBuildInfoReply(QNetworkReply *reply)
{
    if (reply == nullptr)
        return;

    QByteArray data = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull())
    {
        qDebug() << Q_FUNC_INFO << "json is null";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    if (!jsonObject.contains(QStringLiteral("url")))
    {
        return;
    }
    QString buildUrl = jsonObject[QStringLiteral("url")].toString();
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

    emit buildInfoFetched(info);
    emit nextBuildInfoFetchRequested();
}

void BuildHistoryFetcher::processFirstPendingUrl()
{
    if (_urlsToFetch.isEmpty())
        return;
    QString url = _urlsToFetch.takeFirst();
    QNetworkRequest request = _restRequestBuilder->buildBuildInfoRequest(url);
    _manager->get(request);
}
