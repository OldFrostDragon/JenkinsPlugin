#ifndef RESTREQUESTBUILDER_H
#define RESTREQUESTBUILDER_H

#include "jenkinssettings.h"
#include <QNetworkRequest>

namespace JenkinsPlugin
{
namespace Internal
{

class RestRequestBuilder : public QObject
{
    Q_OBJECT

public:
    RestRequestBuilder(const JenkinsSettings &settings);
    static QString urlToRestApiUrl(const QString &url);
    QNetworkRequest buildRequest(const QString urlString) const;

    JenkinsSettings jenkinsSettings() const;
    void setJenkinsSettings(const JenkinsSettings &jenkinsSettings);

signals:
    void settingsChanged();

private:
    static const QString REST_API_URL_SUFFIX;

    JenkinsSettings _jenkinsSettings;
};

}
}

#endif  // RESTREQUESTBUILDER_H