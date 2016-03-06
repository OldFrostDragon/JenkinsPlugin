#include "jenkinsjobsmodel.h"

using namespace JenkinsPlugin::Internal;

static JenkinsJobsModel *_instance = nullptr;

JenkinsJobsModel::JenkinsJobsModel() { updateHeader(); }

JenkinsJobsModel *JenkinsJobsModel::instance()
{
    if (_instance == nullptr)
        _instance = new JenkinsJobsModel(nullptr);
    return _instance;
}

void JenkinsJobsModel::updateHeader()
{
    qDebug() << "update";
    _rootItem->setName(QString(QStringLiteral("Jenkins [%1]")).arg(_jenkinsSettings.jenkinsUrl()));
    emit dataChanged(QAbstractItemModel::createIndex(0, 0, _rootItem->parent()),
                     QAbstractItemModel::createIndex(0, 0, _rootItem->parent()),
                     QVector< int >() << Qt::DisplayRole);
}

JenkinsJobsModel::JenkinsJobsModel(QObject *parent)
    : TreeModel(parent), _rootItem(new JenkinsTreeItem(tr("Jenkins"), JenkinsTreeItem::Type::Root))
{
    rootItem()->appendChild(_rootItem);
}

void JenkinsJobsModel::resetJobs(QList< JenkinsJob > newJobs)
{
    _rootItem->removeChildren();
    foreach (JenkinsJob job, newJobs)
    {
        _rootItem->appendChild(new JenkinsTreeItem(job.name(), JenkinsTreeItem::Type::Job));
    }
}

JenkinsSettings JenkinsJobsModel::jenkinsSettings() const { return _jenkinsSettings; }

void JenkinsJobsModel::setJenkinsSettings(const JenkinsSettings &jenkinsSettings)
{
    _jenkinsSettings = jenkinsSettings;
    updateHeader();
}

void JenkinsJobsModel::setJenkinsJobs(QList< JenkinsJob > jobs) { resetJobs(jobs); }
