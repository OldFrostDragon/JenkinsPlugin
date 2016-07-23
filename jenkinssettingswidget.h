#ifndef JENKINSSETTINGSWIDGET_H
#define JENKINSSETTINGSWIDGET_H

#include <QWidget>
#include "jenkinssettings.h"


namespace Ui
{
class JenkinsSettingsWidget;
}

namespace JenkinsCI
{
namespace Internal
{
class JenkinsSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit JenkinsSettingsWidget(QWidget *parent = 0);
    ~JenkinsSettingsWidget();

    JenkinsSettings settings();
    void setSettings(const JenkinsSettings &settings);

private:
    void settingsToUi(const JenkinsSettings &settings);
    JenkinsSettings settingsFromUi() const;

    JenkinsSettings _settings;
    Ui::JenkinsSettingsWidget *ui;
};
}
}
#endif  // JENKINSSETTINGSWIDGET_H
