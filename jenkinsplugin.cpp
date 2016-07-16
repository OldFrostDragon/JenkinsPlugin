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

#include <projectexplorer/taskhub.h>
#include <projectexplorer/task.h>
#include <projectexplorer/projectexplorerconstants.h>

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

    _fetchTimeoutManager = new FetchingTimeoutManager();
    addAutoReleasedObject(_fetchTimeoutManager);

    _fetcher = new JenkinsDataFetcher(_restRequestBuilder);
    addAutoReleasedObject(_fetcher);

    _viewFetcher = new JenkinsViewFetcher(_restRequestBuilder);
    addAutoReleasedObject(_viewFetcher);

    //    onSettingsChanged(_settings);

    _alreadyReportedFailures.clear();
    JenkinsJobsModel::instance()->resetModel(_settings.jenkinsUrl());
    _pane->setJenkinsSettings(_settings);

    connect(_fetchTimeoutManager, &FetchingTimeoutManager::viewUpdateRequested, _viewFetcher,
            &JenkinsViewFetcher::fetchViews);
    connect(_fetchTimeoutManager, &FetchingTimeoutManager::jobDataUpdateRequested, this, [=]()
            {
                qDebug() << "fetch jobs for" << _pane->getSelectedOrDefaultView().url;
                _fetcher->fetchJobs(_pane->getSelectedOrDefaultView().url);
            });

    connect(_fetcher, &JenkinsDataFetcher::jobUpdated, this, &JenkinsPluginPlugin::updateJob);
    connect(_viewFetcher, &JenkinsViewFetcher::viewsFetched, this, [=](QSet< ViewInfo > info)
            {
                _pane->updateViews(info);
                _fetchTimeoutManager->setIsViewsFetched(!info.isEmpty());
            });

    connect(_pane, &JenkinsPane::currentViewChanged, this, [=]()
            {
                qDebug() << "force jobs update for" << _pane->getSelectedOrDefaultView().url;
                _fetcher->forceRefetch(_pane->getSelectedOrDefaultView().url);
            });

    connect(JenkinsJobsModel::instance(), &JenkinsJobsModel::jobFailed, this,
            &JenkinsPluginPlugin::addFailedJobMessageToIssues);
    createOptionsPage();

    _fetchTimeoutManager->startTimer();
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

    _alreadyReportedFailures.clear();
    JenkinsJobsModel::instance()->resetModel(settings.jenkinsUrl());
    _restRequestBuilder->setJenkinsSettings(settings);
    _pane->setJenkinsSettings(_settings);
    _fetchTimeoutManager->setIsViewsFetched(false);
    _pane->updateViews({}); // reset all views
    _fetchTimeoutManager->triggerFetching();
}

void JenkinsPluginPlugin::showJobHistoryDialog(JenkinsJob job)
{
    BuildHistoryDialog *dialog = new BuildHistoryDialog(job, createBuildHistoryModel(), nullptr);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->show();
}

void JenkinsPluginPlugin::addFailedJobMessageToIssues(const JenkinsJob job)
{
    if (!_settings.notifyAboutFailedBuilds())
        return;
    JenkinsJob::BuildUrl lastBuildUrl = job.getLastBuildUrl();
    if (_alreadyReportedFailures.contains(job.name()))
    {
        JenkinsJob::BuildUrl reportedUrl = _alreadyReportedFailures[job.name()];
        if (reportedUrl == lastBuildUrl)
            return;
    }
    QUrl url(lastBuildUrl.url);
    url.setPort(_settings.port());
    QString message = QCoreApplication::translate("JenkinsPlugin::Task",
                                                  "Jenkins: %1 Build #%2 failed. Build date: %3.\n "
                                                  "See %4 for details")
                          .arg(job.name())
                          .arg(lastBuildUrl.number)
                          .arg(job.lastBuildDate().toString(QStringLiteral("dd.MM.yyyy hh:mm:ss")))
                          .arg(url.toString());
    ProjectExplorer::TaskHub::addTask(ProjectExplorer::Task::Error, message,
                                      ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
    _alreadyReportedFailures.insert(job.name(), lastBuildUrl);
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
