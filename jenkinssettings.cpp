#include "jenkinssettings.h"

using namespace JenkinsPlugin::Internal;

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
