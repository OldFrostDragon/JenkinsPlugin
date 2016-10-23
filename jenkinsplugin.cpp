#include "jenkinsplugin.h"
#include "jenkinspluginconstants.h"

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>

#include <QAction>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>

#include <QtPlugin>

#include "buildhistorydialog.h"
#include "jenkinsjobsmodel.h"

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/task.h>
#include <projectexplorer/taskhub.h>

#include "warningpopup.h"

using namespace JenkinsCI::Internal;

JenkinsCIPlugin::JenkinsCIPlugin()
{
    // Create your members
    _settings.load(Core::ICore::settings());
}

JenkinsCIPlugin::~JenkinsCIPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
    _settings.save(Core::ICore::settings());
}

bool JenkinsCIPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    _restRequestBuilder = std::make_shared< RestRequestBuilder >(_settings);

    _pane = new JenkinsPane(_restRequestBuilder);
    addAutoReleasedObject(_pane);
    connect(_pane, &JenkinsPane::buildHistoryRequested, this,
            &JenkinsCIPlugin::showJobHistoryDialog);

    _fetchTimeoutManager = new FetchingTimeoutManager();
    addAutoReleasedObject(_fetchTimeoutManager);

    _fetcher = new JenkinsDataFetcher(_restRequestBuilder);
    addAutoReleasedObject(_fetcher);

    _viewFetcher = new JenkinsViewFetcher(_restRequestBuilder);
    addAutoReleasedObject(_viewFetcher);

    //    onSettingsChanged(_settings);

    _alreadyReportedFailures.clear();
    JenkinsJobsModel::instance()->resetModel(_settings.jenkinsUrl());

    connect(_fetchTimeoutManager, &FetchingTimeoutManager::viewUpdateRequested, _viewFetcher,
            &JenkinsViewFetcher::fetchViews);
    connect(_fetchTimeoutManager, &FetchingTimeoutManager::jobDataUpdateRequested, this, [=]() {
        QUrl currentViewUrl = _restRequestBuilder->buildThisOrDefaultViewUrl(
            _pane->getSelectedView().url.toString());
        _fetcher->fetchJobs(currentViewUrl);
    });
    connect(_fetchTimeoutManager, &FetchingTimeoutManager::jobForcedUpdateRequested, this, [=]() {
        QUrl currentViewUrl = _restRequestBuilder->buildThisOrDefaultViewUrl(
            _pane->getSelectedView().url.toString());
        _fetcher->forceRefetch(currentViewUrl);
    });

    connect(_fetcher, &JenkinsDataFetcher::jobUpdated, this, &JenkinsCIPlugin::updateJob);
    connect(_viewFetcher, &JenkinsViewFetcher::viewsFetched, this, [=](QSet< ViewInfo > info) {
        _pane->updateViews(info);
        _fetchTimeoutManager->setIsViewsFetched(!info.isEmpty());
    });

    connect(_pane, &JenkinsPane::currentViewChanged, this, [=]() {
        QUrl currentViewUrl = _restRequestBuilder->buildThisOrDefaultViewUrl(
            _pane->getSelectedView().url.toString());
        QUrl settingsUrl = currentViewUrl;
        settingsUrl.setPort(-1);
        _settings.setSelectedViewUrl(settingsUrl.toString());
        onSettingsChanged(_settings);
    });

    connect(JenkinsJobsModel::instance(), &JenkinsJobsModel::jobFailed, this,
            &JenkinsCIPlugin::reportJobExecutionFailure);
    createOptionsPage();

    _fetchTimeoutManager->startTimer();
    return true;
}

void JenkinsCIPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag JenkinsCIPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void JenkinsCIPlugin::updateJobs(QList< JenkinsJob > jobs)
{
    JenkinsJobsModel::instance()->setJenkinsJobs(jobs);
}

void JenkinsCIPlugin::updateJob(JenkinsJob job)
{
    JenkinsJobsModel::instance()->setOrUpdateJob(job);
}

void JenkinsCIPlugin::onSettingsChanged(const JenkinsSettings &settings)
{
    settings.save(Core::ICore::settings());

    _alreadyReportedFailures.clear();
    JenkinsJobsModel::instance()->resetModel(settings.jenkinsUrl());
    _restRequestBuilder->setJenkinsSettings(settings);
    _fetchTimeoutManager->setIsViewsFetched(false);
    // reset view list only when user update jenkins server settings in options
    if (_settings.isServerSettingsDiffers(settings))
        _pane->clearViews();
    _fetchTimeoutManager->triggerFetching(FetchingTimeoutManager::FetchType::ForcedJobFetching);

    _settings = settings;
}

void JenkinsCIPlugin::showJobHistoryDialog(JenkinsJob job)
{
    BuildHistoryDialog *dialog = new BuildHistoryDialog(job, createBuildHistoryModel(), nullptr);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    dialog->show();
}

void JenkinsCIPlugin::reportJobExecutionFailure(const JenkinsJob job)
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

    WarningPopup *popup = new WarningPopup(_optionsPage->widget());
    popup->setShowPeriod(_settings.popupShowPeriod());
    QString popupMessage
        = QCoreApplication::translate("JenkinsPlugin::Task", "<b>Jenkins:</b>\n \"%1\" #%2 failed")
              .arg(job.name())
              .arg(lastBuildUrl.number);
    popup->showPopup(popupMessage);
}

void JenkinsCIPlugin::createOptionsPage()
{
    _optionsPage = new OptionsPage(_settings);
    addAutoReleasedObject(_optionsPage);
    connect(_optionsPage, &OptionsPage::settingsChanged, this, &JenkinsCIPlugin::onSettingsChanged);
}

BuildHistoryModel *JenkinsCIPlugin::createBuildHistoryModel()
{
    BuildHistoryFetcher *_fetcher = new BuildHistoryFetcher(_restRequestBuilder);
    BuildHistoryModel *model = new BuildHistoryModel(_fetcher);
    return model;
}
