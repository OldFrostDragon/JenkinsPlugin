#include "jenkinssettings.h"
#include "jenkinspluginconstants.h"

using namespace JenkinsPlugin::Internal;

JenkinsSettings::JenkinsSettings() {}

QString JenkinsSettings::jenkinsUrl() const { return _jenkinsUrl; }

void JenkinsSettings::setJenkinsUrl(const QString &jenkinsUrl) { _jenkinsUrl = jenkinsUrl; }

void JenkinsSettings::save(QSettings *settings) const
{
    settings->beginGroup(QLatin1String(JenkinsPlugin::Constants::SETTINGS_GROUP));
    settings->setValue(QLatin1String(JenkinsPlugin::Constants::URL), _jenkinsUrl);
    settings->setValue(QLatin1String(JenkinsPlugin::Constants::PORT), _port);
    settings->setValue(QLatin1String(JenkinsPlugin::Constants::USERNAME), _username);
    settings->setValue(QLatin1String(JenkinsPlugin::Constants::API_TOKEN), _apiToken);
    settings->setValue(QLatin1String(JenkinsPlugin::Constants::NOTIFY_ABOUT_FAILED_BUILD),
                       _notifyAboutFailedBuilds);
    settings->setValue(QLatin1String(JenkinsPlugin::Constants::SELECTED_VIEW_URL),
                       _selectedViewUrl);
    settings->endGroup();
}

void JenkinsSettings::load(QSettings *settings)
{
    setDefaults();
    settings->beginGroup(QLatin1String(JenkinsPlugin::Constants::SETTINGS_GROUP));
    _jenkinsUrl = settings->value(QLatin1String(JenkinsPlugin::Constants::URL)).toString();
    _port = settings->value(QLatin1String(JenkinsPlugin::Constants::PORT), 80).toInt();
    _username = settings->value(QLatin1String(JenkinsPlugin::Constants::USERNAME)).toString();
    _apiToken = settings->value(QLatin1String(JenkinsPlugin::Constants::API_TOKEN)).toString();
    _notifyAboutFailedBuilds
        = settings->value(QLatin1String(JenkinsPlugin::Constants::NOTIFY_ABOUT_FAILED_BUILD), true)
              .toBool();
    _selectedViewUrl
        = settings->value(QLatin1String(JenkinsPlugin::Constants::SELECTED_VIEW_URL), _jenkinsUrl)
              .toString();
    settings->endGroup();
}

int JenkinsSettings::port() const { return _port; }

void JenkinsSettings::setPort(int port) { _port = port; }

QString JenkinsSettings::username() const { return _username; }

void JenkinsSettings::setUsername(const QString &username) { _username = username; }

bool JenkinsSettings::equals(const JenkinsSettings &other) const
{
    return _jenkinsUrl == other._jenkinsUrl && _port == other._port && _username == other._username
           && _apiToken == other._apiToken
           && _notifyAboutFailedBuilds == other._notifyAboutFailedBuilds
           && _selectedViewUrl == other._selectedViewUrl;
}

void JenkinsSettings::setDefaults()
{
    _jenkinsUrl.clear();
    _port = 80;
    _username.clear();
    _apiToken.clear();
    _notifyAboutFailedBuilds = true;
    _selectedViewUrl.clear();
}

QString JenkinsSettings::apiToken() const { return _apiToken; }

void JenkinsSettings::setApiToken(const QString &apiToken) { _apiToken = apiToken; }

bool JenkinsSettings::notifyAboutFailedBuilds() const { return _notifyAboutFailedBuilds; }

void JenkinsSettings::setNotifyAboutFailedBuilds(bool notifyAboutFailedBuilds)
{
    _notifyAboutFailedBuilds = notifyAboutFailedBuilds;
}

QString JenkinsSettings::selectedViewUrl() const { return _selectedViewUrl; }

void JenkinsSettings::setSelectedViewUrl(const QString &selectedViewUrl)
{
    _selectedViewUrl = selectedViewUrl;
}

bool JenkinsSettings::isServerSettingsDiffers(const JenkinsSettings &other) const
{
    return _jenkinsUrl != other._jenkinsUrl || _port != other._port || _username != other._username
           || _apiToken != other._apiToken;
}

bool operator==(const JenkinsPlugin::Internal::JenkinsSettings &first,
                const JenkinsPlugin::Internal::JenkinsSettings &second)
{
    return first.equals(second);
}

bool operator!=(const JenkinsPlugin::Internal::JenkinsSettings &first,
                const JenkinsPlugin::Internal::JenkinsSettings &second)
{
    return !first.equals(second);
}
