#ifndef RESTREQUESTBUILDER_H
#define RESTREQUESTBUILDER_H

#include "jenkinssettings.h"
#include <QNetworkRequest>

namespace JenkinsCI
{
namespace Internal
{
/*!
 * \brief The RestRequestBuilder class builds QNetworkRequest objects to specified Jenkins instance.
 * Built QNetworkRequest use JSON REST API url with port and authorization options if they are set.
 */
class RestRequestBuilder : public QObject
{
    Q_OBJECT
public:
    RestRequestBuilder(const JenkinsSettings &settings);

    static QString urlToRestApiUrl(const QString &url); /// add /api/json to the end if absent
    static QString cutRestApiUrlPart(const QString &url); /// remove /api/json from end of URL
    static QString lastBuildUrlToJobUrl(QString buildUrl);

    QUrl buildUrl(QString urlString) const; /// create REST URL using Jenkins instance settings
    QUrl buildThisOrDefaultViewUrl(QString urlString) const;

    QNetworkRequest buildRequest(const QString urlString) const;
    QNetworkRequest buildAvaliableJobsRequest(QUrl viewUrl) const;
    QNetworkRequest buildBuildInfoRequest(const QString url) const;
    QNetworkRequest buildLastBuildInfoRequest(const QString &jobUrl) const;

    JenkinsSettings jenkinsSettings() const;
    void setJenkinsSettings(const JenkinsSettings &jenkinsSettings);


signals:
    void settingsChanged();

private:
    static const QString REST_API_URL_SUFFIX;
    static const QString LAST_BUILD_URL_SUFFIX;
    JenkinsSettings _jenkinsSettings;
};

}
}

#endif  // RESTREQUESTBUILDER_H
