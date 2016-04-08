#ifndef JENKINSTREEITEM_H
#define JENKINSTREEITEM_H

#include <utils/treemodel.h>
#include "jenkinsdatafetcher.h"

namespace JenkinsPlugin
{
namespace Internal
{
class JenkinsTreeItem : public Utils::TreeItem
{
public:
    enum class Type
    {
        Root,
        Job
    };

    enum JobRoles
    {
        IsRunningRole = Qt::UserRole + 1
    };

    JenkinsTreeItem(const QString &name, const Type type);
    JenkinsTreeItem(const Type type, const JenkinsJob &job);

    JenkinsTreeItem(const JenkinsTreeItem &other);

    Type itemType() const;
    void setItemType(const Type &itemType);

    QString name() const;
    void setName(const QString &name);

    JenkinsTreeItem *childItem(int row) const;
    // TreeItem interface
public:
    QVariant data(int column, int role) const;

    JenkinsJob job() const;
    void setJob(const JenkinsJob &job);

private:
    Type _itemType;
    QString _name;
    JenkinsJob _job;
    QIcon _serverIcon;
};
}
}
#endif  // JENKINSTREEITEM_H
