#ifndef JENKINSPLUGIN_H
#define JENKINSPLUGIN_H

#include "jenkinsci_global.h"

#include <extensionsystem/iplugin.h>

#include "jenkinsdatafetcher.h"
#include "optionspage.h"
#include "jenkinspane.h"
#include "restrequestbuilder.h"
#include "jenkinsjob.h"
#include "jenkinsviewfetcher.h"
#include "fetchingtimeoutmanager.h"

namespace JenkinsCI
{
namespace Internal
{

class BuildHistoryModel;

class JenkinsCIPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "JenkinsCI.json")

public:
    JenkinsCIPlugin();
    ~JenkinsCIPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void updateJobs(QList< JenkinsJob > jobs);
    void updateJob(JenkinsJob job);

    void onSettingsChanged(const JenkinsSettings &settings);
    void showJobHistoryDialog(JenkinsJob job);
    void reportJobExecutionFailure(const JenkinsJob job);

private:
    void createOptionsPage();
    BuildHistoryModel *createBuildHistoryModel();


    std::shared_ptr<RestRequestBuilder> _restRequestBuilder;

    JenkinsDataFetcher *_fetcher;
    JenkinsViewFetcher *_viewFetcher;
    OptionsPage *_optionsPage;
    JenkinsSettings _settings;
    JenkinsPane *_pane;
    FetchingTimeoutManager *_fetchTimeoutManager;
    //key - job name
    QMap<QString, JenkinsJob::BuildUrl> _alreadyReportedFailures;
};

}  // namespace Internal
}  // namespace JenkinsPlugin

#endif  // JENKINSPLUGIN_H
