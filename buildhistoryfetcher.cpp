#include "buildhistoryfetcher.h"

#include "jenkinspluginconstants.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

using namespace JenkinsCI::Internal;

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

std::shared_ptr<RestRequestBuilder> BuildHistoryFetcher::restRequestBuilder() const
{
    return _restRequestBuilder;
}
