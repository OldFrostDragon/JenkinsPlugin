#ifndef JENKINSJOBSMODEL_H
#define JENKINSJOBSMODEL_H

#include <QTimer>

#include <utils/treemodel.h>
#include "jenkinstreeitem.h"
#include "jenkinssettings.h"

#include "jenkinsjob.h"

namespace JenkinsCI
{
namespace Internal
{

class JenkinsJobsModel : public Utils::TreeModel
{
    Q_OBJECT
public:
    static JenkinsJobsModel* instance();
    void updateHeader();

    void resetModel(const QString &jenkinsUrlString);

    void resetJobs(QList<JenkinsJob> newJobs);
    void setJenkinsJobs(QList<JenkinsJob> jobs);
    void setOrUpdateJob(JenkinsJob job);

    virtual int columnCount(const QModelIndex &) const override {return 3;}

    QString getRootItemContent() const {return  _rootItem->name();}
signals:
    void rootItemUpdated();
    void jobFailed(const JenkinsJob job);
    void jobStarted(const JenkinsJob job);

private:
    explicit JenkinsJobsModel(QObject *parent = 0);
    void updateAnimationRecursively(Utils::TreeItem *rootItem);

    JenkinsTreeItem *_rootItem;

    //static JenkinsJobsModel *_instance;
    QString _jenkinsUrlString;
    QTimer *_animationTimer{nullptr};
};

}
}
#endif  // JENKINSJOBSMODEL_H
