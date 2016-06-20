#ifndef JENKINSSETTINGS_H
#define JENKINSSETTINGS_H

#include <QString>
#include <QSettings>

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsSettings
{
public:
    JenkinsSettings();

    QString jenkinsUrl() const;
    void setJenkinsUrl(const QString &jenkinsUrl);

    void save(QSettings *settings) const;
    void load(QSettings *settings);

    int port() const;
    void setPort(int port);

    QString username() const;
    void setUsername(const QString &username);

    bool equals(const JenkinsSettings &other) const;
    void setDefaults();

    QString apiToken() const;
    void setApiToken(const QString &apiToken);

    bool notifyAboutFailedBuilds() const;
    void setNotifyAboutFailedBuilds(bool notifyAboutFailedBuilds);

private:
    QString _jenkinsUrl{QStringLiteral("https://ci.jenkins-ci.org/view/Libraries/")};
    int _port;
    QString _username;
    QString _apiToken;

    bool _notifyAboutFailedBuilds{true};
    //  QString _jenkinsUrl{QStringLiteral("http://jenkins.cyanogenmod.com")};
    //  QString _jenkinsUrl{QStringLiteral("http://dotnet-ci.cloudapp.net/view/Roslyn/")};
};

bool operator==(const JenkinsSettings &first, const JenkinsSettings &second);
bool operator!=(const JenkinsSettings &first, const JenkinsSettings &second);
}
}

#endif  // JENKINSSETTINGS_H
