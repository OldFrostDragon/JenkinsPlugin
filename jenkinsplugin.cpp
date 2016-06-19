#include "jenkinsplugin.h"
#include "jenkinspluginconstants.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>

#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>

#include <QtPlugin>

#include <QDebug>

#include "jenkinsjobsmodel.h"
#include "buildhistorydialog.h"

using namespace JenkinsPlugin::Internal;

JenkinsPluginPlugin::JenkinsPluginPlugin()
{
    // Create your members
    _settings.load(Core::ICore::settings());
}

JenkinsPluginPlugin::~JenkinsPluginPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
    _settings.save(Core::ICore::settings());
}

bool JenkinsPluginPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

    _pane = new JenkinsPane();
    addAutoReleasedObject(_pane);
    connect(_pane, &JenkinsPane::buildHistoryRequested, this,
            &JenkinsPluginPlugin::showJobHistoryDialog);

    _restRequestBuilder = std::make_shared< RestRequestBuilder >(_settings);

    _fetcher = new JenkinsDataFetcher(_restRequestBuilder);
    addAutoReleasedObject(_fetcher);
    onSettingsChanged(_settings);

    connect(_fetcher, &JenkinsDataFetcher::jobsUpdated, this, &JenkinsPluginPlugin::updateJobs);
    connect(_fetcher, &JenkinsDataFetcher::jobUpdated, this, &JenkinsPluginPlugin::updateJob);

    createOptionsPage();

    return true;
}

void JenkinsPluginPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag JenkinsPluginPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void JenkinsPluginPlugin::updateJobs(QList< JenkinsJob > jobs)
{
    JenkinsJobsModel::instance()->setJenkinsJobs(jobs);
}

void JenkinsPluginPlugin::updateJob(JenkinsJob job)
{
    JenkinsJobsModel::instance()->setOrUpdateJob(job);
}

void JenkinsPluginPlugin::onSettingsChanged(const JenkinsSettings &settings)
{
    _settings = settings;
    _settings.save(Core::ICore::settings());
    JenkinsJobsModel::instance()->setJenkinsSettings(settings);
    _restRequestBuilder->setJenkinsSettings(settings);
    _pane->setJenkinsSettings(_settings);
}

void JenkinsPluginPlugin::showJobHistoryDialog(JenkinsJob job)
{
    BuildHistoryDialog *dialog = new BuildHistoryDialog(job, createBuildHistoryModel(), nullptr);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->show();
}

void JenkinsPluginPlugin::createOptionsPage()
{
    _optionsPage = new OptionsPage(_settings);
    addAutoReleasedObject(_optionsPage);
    connect(_optionsPage, &OptionsPage::settingsChanged, this,
            &JenkinsPluginPlugin::onSettingsChanged);
}

BuildHistoryModel *JenkinsPluginPlugin::createBuildHistoryModel()
{
    BuildHistoryFetcher *_fetcher = new BuildHistoryFetcher(_restRequestBuilder);
    BuildHistoryModel *model = new BuildHistoryModel(_fetcher, _settings);
    return model;
}
