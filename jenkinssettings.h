#ifndef JENKINSSETTINGS_H
#define JENKINSSETTINGS_H

#include <QString>

class JenkinsSettings
{
public:
    JenkinsSettings();

    QString jenkinsUrl() const;
    void setJenkinsUrl(const QString &jenkinsUrl);

private:
    QString _jenkinsUrl{QStringLiteral("https://ci.jenkins-ci.org/")};

};

#endif // JENKINSSETTINGS_H
