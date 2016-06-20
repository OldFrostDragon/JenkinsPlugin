#ifndef JENKINSPLUGIN_H
#define JENKINSPLUGIN_H

#include "jenkinsplugin_global.h"

#include <extensionsystem/iplugin.h>

#include "jenkinsdatafetcher.h"
#include "optionspage.h"
#include "jenkinspane.h"
#include "restrequestbuilder.h"
#include "jenkinsjob.h"

namespace JenkinsPlugin
{
namespace Internal
{

class BuildHistoryModel;

class JenkinsPluginPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "JenkinsPlugin.json")

public:
    JenkinsPluginPlugin();
    ~JenkinsPluginPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private slots:
    void updateJobs(QList< JenkinsJob > jobs);
    void updateJob(JenkinsJob job);

    void onSettingsChanged(const JenkinsSettings &settings);
    void showJobHistoryDialog(JenkinsJob job);
    void addFailedJobMessageToIssues(const JenkinsJob job);

private:
    void createOptionsPage();
    BuildHistoryModel *createBuildHistoryModel();


    std::shared_ptr<RestRequestBuilder> _restRequestBuilder;

    JenkinsDataFetcher *_fetcher;
    OptionsPage *_optionsPage;
    JenkinsSettings _settings;
    JenkinsPane *_pane;

    //key - job name
    QMap<QString, JenkinsJob::BuildUrl> _alreadyReportedFailures;
};

}  // namespace Internal
}  // namespace JenkinsPlugin

#endif  // JENKINSPLUGIN_H
