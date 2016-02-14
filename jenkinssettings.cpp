#include "jenkinssettings.h"

JenkinsSettings::JenkinsSettings()
{

}

QString JenkinsSettings::jenkinsUrl() const
{
    return _jenkinsUrl;
}

void JenkinsSettings::setJenkinsUrl(const QString &jenkinsUrl)
{
    _jenkinsUrl = jenkinsUrl;
}
