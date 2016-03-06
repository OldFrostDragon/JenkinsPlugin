#ifndef JENKINSJOBSMODEL_H
#define JENKINSJOBSMODEL_H

#include <utils/treemodel.h>
#include "jenkinstreeitem.h"
#include "jenkinssettings.h"

#include "jenkinsdatafetcher.h"

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsJobsModel : public Utils::TreeModel
{
public:
    JenkinsJobsModel();
    static JenkinsJobsModel* instance();
    void updateHeader();

    JenkinsSettings jenkinsSettings() const;
    void setJenkinsSettings(const JenkinsSettings &jenkinsSettings);
    void setJenkinsJobs(QList<JenkinsJob> jobs);

private:
    explicit JenkinsJobsModel(QObject *parent = 0);
    void resetJobs(QList<JenkinsJob> newJobs);

    JenkinsTreeItem *_rootItem;

    //static JenkinsJobsModel *_instance;
    JenkinsSettings _jenkinsSettings;
};

}
}
#endif  // JENKINSJOBSMODEL_H
