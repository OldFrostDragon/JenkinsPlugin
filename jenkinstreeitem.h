#ifndef JENKINSTREEITEM_H
#define JENKINSTREEITEM_H

#include <utils/treemodel.h>

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

    JenkinsTreeItem(const QString &name, const Type type);
    JenkinsTreeItem(const JenkinsTreeItem &other);

    Type itemType() const;
    void setItemType(const Type &itemType);

    QString name() const;
    void setName(const QString &name);

    JenkinsTreeItem *childItem(int row) const;
    // TreeItem interface
public:
    QVariant data(int column, int role) const;

private:
    Type _itemType;
    QString _name;

};
}
}
#endif  // JENKINSTREEITEM_H
