#include "jenkinssettingswidget.h"
#include "ui_jenkinssettingswidget.h"

using namespace JenkinsPlugin::Internal;

JenkinsSettingsWidget::JenkinsSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JenkinsSettingsWidget)
{
    ui->setupUi(this);
}

JenkinsSettingsWidget::~JenkinsSettingsWidget()
{
    delete ui;
}

void JenkinsSettingsWidget::settingsToUi(const JenkinsSettings &settings)
{
    ui->urlLineEdit->setText(settings.jenkinsUrl());
    ui->portSpinBox->setValue(settings.port());
    ui->usernameLineEdit->setText(settings.username());
    ui->apiTokenLineEdit->setText(settings.apiToken());
    ui->notifyAboutFailedBuilds->setChecked(settings.notifyAboutFailedBuilds());
}

JenkinsSettings JenkinsSettingsWidget::settingsFromUi() const
{
    JenkinsSettings settings;
    settings.setJenkinsUrl(ui->urlLineEdit->text());
    settings.setPort(ui->portSpinBox->value());
    settings.setUsername(ui->usernameLineEdit->text());
    settings.setApiToken(ui->apiTokenLineEdit->text());
    settings.setNotifyAboutFailedBuilds(ui->notifyAboutFailedBuilds->isChecked());
    return settings;
}

JenkinsSettings JenkinsSettingsWidget::settings()
{
    _settings = settingsFromUi();
    return _settings;
}

void JenkinsSettingsWidget::setSettings(const JenkinsSettings &settings)
{
    _settings = settings;
    settingsToUi(_settings);
}
