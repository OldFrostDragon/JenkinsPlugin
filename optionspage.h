#ifndef OPTIONSPAGE_H
#define OPTIONSPAGE_H

#include <QObject>
#include <QPointer>

#include "coreplugin/dialogs/ioptionspage.h"

#include "jenkinssettings.h"
#include "jenkinssettingswidget.h"

namespace JenkinsCI
{
namespace Internal
{
class OptionsPage : public Core::IOptionsPage
{
    Q_OBJECT
public:
    explicit OptionsPage(const JenkinsSettings settings, QObject *parent = 0);

    // IOptionsPage interface
public:
    QWidget *widget();
    void apply();
    void finish();

    void setSettings(const JenkinsSettings &settings);

signals:
    void settingsChanged(JenkinsSettings settings);

public slots:

private:
    JenkinsSettings _settings;
    QPointer< JenkinsSettingsWidget > _optionsWidget;
};
}
}
#endif  // OPTIONSPAGE_H
