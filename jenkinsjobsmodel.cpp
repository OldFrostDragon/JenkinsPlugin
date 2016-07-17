#include "jenkinsjobsmodel.h"

using namespace JenkinsPlugin::Internal;

static JenkinsJobsModel *_instance = nullptr;

JenkinsJobsModel *JenkinsJobsModel::instance()
{
    if (_instance == nullptr)
    {
        _instance = new JenkinsJobsModel(nullptr);
        _instance->setHeader(QStringList{tr("Job"), tr("Health report"), tr("Last build date")});
    }
    return _instance;
}

void JenkinsJobsModel::updateHeader()
{
    _rootItem->setName(QString(QStringLiteral("Jenkins [%1]")).arg(_jenkinsUrlString));
    _rootItem->setItemUrl(_jenkinsUrlString);
    emit dataChanged(QAbstractItemModel::createIndex(0, 0, _rootItem->parent()),
                     QAbstractItemModel::createIndex(0, 2, _rootItem->parent()),
                     QVector< int >() << Qt::DisplayRole);
    emit rootItemUpdated();
}

JenkinsJobsModel::JenkinsJobsModel(QObject *parent)
    : TreeModel(parent), _rootItem(new JenkinsTreeItem(tr("Jenkins"), JenkinsTreeItem::Type::Root))
{
    updateHeader();
    rootItem()->appendChild(_rootItem);
    _animationTimer = new QTimer(this);
    _animationTimer->setSingleShot(false);
    connect(_animationTimer, &QTimer::timeout, [=]()
            {
                updateAnimationRecursively(_rootItem);
            });
    _animationTimer->start(100);
};

void JenkinsJobsModel::resetJobs(QList< JenkinsJob > newJobs)
{
    _rootItem->removeChildren();
    foreach (JenkinsJob job, newJobs)
    {
        _rootItem->appendChild(new JenkinsTreeItem(job.name(), JenkinsTreeItem::Type::Job));
    }
}

void JenkinsJobsModel::updateAnimationRecursively(Utils::TreeItem *rootItem)
{
    if (rootItem == nullptr)
        return;
    foreach (Utils::TreeItem *child, rootItem->children())
    {
        updateAnimationRecursively(child);
    }
    auto castedItem = static_cast< JenkinsTreeItem * >(rootItem);

    if (castedItem->itemType() == JenkinsTreeItem::Type::Job)
    {
        setData(castedItem->index(), castedItem->getNextOpacityValue(),
                static_cast< int >(JenkinsTreeItem::JobRoles::AnimationOpacity));
    }
    else
    {
        if (std::isless(castedItem->currentOpacityValue(), 1.0))
        {
            setData(castedItem->index(), 1.0,
                    static_cast< int >(JenkinsTreeItem::JobRoles::AnimationOpacity));
        }
    }
}

void JenkinsJobsModel::resetModel(const QString &jenkinsUrlString)
{
    _jenkinsUrlString = jenkinsUrlString;
    resetJobs(QList< JenkinsJob >{});
    updateHeader();
}

void JenkinsJobsModel::setJenkinsJobs(QList< JenkinsJob > jobs) { resetJobs(jobs); }

void JenkinsJobsModel::setOrUpdateJob(JenkinsJob job)
{
    QVector< Utils::TreeItem * > items = _rootItem->children();
    QList< JenkinsTreeItem * > castedItems;
    foreach (Utils::TreeItem *treeItem, items)
        castedItems.append(static_cast< JenkinsTreeItem * >(treeItem));
    for (int i = 0; i < castedItems.size(); ++i)
    {
        if (castedItems[i]->job().jobUrl() == job.jobUrl())
        {
            JenkinsJob exitingJob = castedItems[i]->job();
            if (exitingJob.buildStatus() != JenkinsJob::BuildStatus::Unknown
                && job.buildStatus() == JenkinsJob::BuildStatus::Fail && !job.isRunning())
                emit jobFailed(job);

            castedItems[i]->setJob(job);
            emit dataChanged(QAbstractItemModel::createIndex(i, 0, castedItems[i]->parent()),
                             QAbstractItemModel::createIndex(i, 0, castedItems[i]->parent()));
            return;
        }
    }

    // item for job not found. Add new one
    JenkinsTreeItem *newItem = new JenkinsTreeItem(JenkinsTreeItem::Type::Job, job);
    _rootItem->appendChild(newItem);
}
