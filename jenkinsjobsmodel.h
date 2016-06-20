#ifndef JENKINSJOBSMODEL_H
#define JENKINSJOBSMODEL_H

#include <QTimer>

#include <utils/treemodel.h>
#include "jenkinstreeitem.h"
#include "jenkinssettings.h"

#include "jenkinsjob.h"

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsJobsModel : public Utils::TreeModel
{
    Q_OBJECT
public:
    static JenkinsJobsModel* instance();
    void updateHeader();

    JenkinsSettings jenkinsSettings() const;
    void setJenkinsSettings(const JenkinsSettings &jenkinsSettings);
    void setJenkinsJobs(QList<JenkinsJob> jobs);
    void setOrUpdateJob(JenkinsJob job);
    virtual int columnCount(const QModelIndex &) const override {return 3;}

signals:
    void animationRepaintRequested();
    void jobFailed(const JenkinsJob job);

private:
    explicit JenkinsJobsModel(QObject *parent = 0);
    void resetJobs(QList<JenkinsJob> newJobs);
    void updateAnimationRecursively(Utils::TreeItem *rootItem);

    JenkinsTreeItem *_rootItem;

    //static JenkinsJobsModel *_instance;
    JenkinsSettings _jenkinsSettings;
    QTimer *_animationTimer{nullptr};
};

}
}
#endif  // JENKINSJOBSMODEL_H
