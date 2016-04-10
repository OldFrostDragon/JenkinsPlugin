#include "restrequestbuilder.h"

using namespace JenkinsPlugin::Internal;

const QString RestRequestBuilder::REST_API_URL_SUFFIX = QStringLiteral("api/json");
const QString RestRequestBuilder::LAST_BUILD_URL_SUFFIX = QStringLiteral("lastBuild");

RestRequestBuilder::RestRequestBuilder(const JenkinsSettings &settings) : _jenkinsSettings(settings)
{
}

QString RestRequestBuilder::urlToRestApiUrl(const QString &url)
{
    if (url.endsWith(REST_API_URL_SUFFIX))
        return url;
    else if (url.endsWith(QStringLiteral("/")))
        return url + REST_API_URL_SUFFIX;
    else
        return url + QStringLiteral("/") + REST_API_URL_SUFFIX;
}

QString RestRequestBuilder::cutRestApiUrlPart(const QString &url)
{
    QString localUrl = url;
    if (localUrl.endsWith(REST_API_URL_SUFFIX))
        localUrl.chop(REST_API_URL_SUFFIX.size());
    return localUrl;
}

QString RestRequestBuilder::lastBuildUrlToJobUrl(QString buildUrl)
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

QNetworkRequest RestRequestBuilder::buildRequest(const QString urlString) const
{
    QUrl url(urlString);
    url.setPort(_jenkinsSettings.port());
    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("Authorization"),
                         QByteArray("Basic ")
                             + QByteArray(QString(QStringLiteral("%1:%2"))
                                              .arg(_jenkinsSettings.username())
                                              .arg(_jenkinsSettings.apiToken())
                                              .toLocal8Bit())
                                   .toBase64());
    return request;
}

QNetworkRequest RestRequestBuilder::buildAvaliableJobsRequest() const
{
    QNetworkRequest request = buildRequest(urlToRestApiUrl(_jenkinsSettings.jenkinsUrl()));
    return request;
}

QNetworkRequest RestRequestBuilder::buildBuildInfoRequest(const QString url) const
{
    QNetworkRequest request = buildRequest(urlToRestApiUrl(url));
    return request;
}

QNetworkRequest RestRequestBuilder::buildLastBuildInfoRequest(const QString &jobUrl) const
{
    QString localUrl = jobUrl;
    if(!localUrl.endsWith(LAST_BUILD_URL_SUFFIX))
    {
        if(!localUrl.endsWith(QLatin1Char('/')))
            localUrl.append(QString(QLatin1Char('/') + LAST_BUILD_URL_SUFFIX));
        else
            localUrl.append(LAST_BUILD_URL_SUFFIX);
    }

    QNetworkRequest request = buildRequest(urlToRestApiUrl(localUrl));
    return request;
}

JenkinsSettings RestRequestBuilder::jenkinsSettings() const { return _jenkinsSettings; }

void RestRequestBuilder::setJenkinsSettings(const JenkinsSettings &jenkinsSettings)
{
    if (_jenkinsSettings.equals(jenkinsSettings))
        emit settingsChanged();
    _jenkinsSettings = jenkinsSettings;
}
