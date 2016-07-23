#ifndef JENKINSTREEITEM_H
#define JENKINSTREEITEM_H

#include <utils/treemodel.h>
#include "jenkinsjob.h"

namespace JenkinsCI
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
        IsRunningRole = Qt::UserRole + 1,
        AnimationOpacity = Qt::UserRole + 2,
    };

    JenkinsTreeItem(const QString &name, const Type type);
    JenkinsTreeItem(const QString &name, const QString &url, const Type type);
    JenkinsTreeItem(const Type type, const JenkinsJob &job);

    JenkinsTreeItem(const JenkinsTreeItem &other);

    Type itemType() const;
    void setItemType(const Type &itemType);

    QString name() const;
    void setName(const QString &name);

    JenkinsTreeItem *childItem(int row) const;

public:
    QVariant data(int column, int role) const override;
    virtual bool setData(int column, const QVariant &data, int role) override;

    JenkinsJob job() const;
    void setJob(const JenkinsJob &job);

    QString itemUrl() const;
    void setItemUrl(const QString &itemUrl);

    qreal currentOpacityValue() const;
    qreal getNextOpacityValue() const;
private:
    Type _itemType;
    QString _name;
    QString _itemUrl;

    JenkinsJob _job;
    QIcon _serverIcon;
    qreal _currectOpacityValue{1.0};
    bool _isAscendingOpacityChange{false};
};
}
}
#endif  // JENKINSTREEITEM_H
