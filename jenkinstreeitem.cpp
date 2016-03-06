#include "jenkinstreeitem.h"

using namespace JenkinsPlugin::Internal;

JenkinsTreeItem::JenkinsTreeItem(const QString &name, const Type type)
    : _itemType(type), _name(name)
{
}

JenkinsTreeItem::JenkinsTreeItem(const JenkinsTreeItem &other)
    : TreeItem({other._name}), _itemType(other._itemType), _name(other._name)
{
    for (int row = 0, count = other.childCount(); row < count; ++row)
        appendChild(new JenkinsTreeItem(*other.childItem(row)));
}

JenkinsTreeItem::Type JenkinsTreeItem::itemType() const { return _itemType; }

void JenkinsTreeItem::setItemType(const Type &itemType) { _itemType = itemType; }

QString JenkinsTreeItem::name() const { return _name; }

void JenkinsTreeItem::setName(const QString &name) { _name = name; }

JenkinsTreeItem *JenkinsTreeItem::childItem(int row) const
{
    return static_cast< JenkinsTreeItem * >(child(row));
}

QVariant JenkinsTreeItem::data(int column, int role) const
{
    if(role == Qt::DisplayRole && column == 0)
        return _name;
    else
        return QVariant();
}
