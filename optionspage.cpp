#include "optionspage.h"
#include "jenkinspluginconstants.h"

using namespace JenkinsPlugin::Internal;

OptionsPage::OptionsPage(const JenkinsSettings settings, QObject *parent) : Core::IOptionsPage(parent)
{
    _settings = settings;
    setId("JenkinsPluginSettings");
    setDisplayName(tr("Jenkins"));
    setCategory("Jenkins");
    setDisplayCategory(tr("Jenkins"));
    setCategoryIcon(QLatin1String(JenkinsPlugin::Constants::JENKINS_LOGO));
}

QWidget *OptionsPage::widget()
{
    if(_optionsWidget == NULL)
    {
        _optionsWidget = new JenkinsSettingsWidget();
        _optionsWidget->setSettings(_settings);
    }
    return _optionsWidget;
}

void OptionsPage::apply()
{
    JenkinsSettings newSettings = _optionsWidget->settings();
    if(!newSettings.equals(_settings))
    {
        _settings = newSettings;
        emit settingsChanged(_settings);
    }
}

void OptionsPage::finish()
{
}

void OptionsPage::setSettings(const JenkinsSettings &settings)
{
    _settings = settings;
}
