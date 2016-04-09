#include "jenkinstreeitem.h"

#include <QIcon>
#include "jenkinspluginconstants.h"

using namespace JenkinsPlugin::Internal;

JenkinsTreeItem::JenkinsTreeItem(const QString &name, const Type type)
    : _itemType(type), _name(name)
{
    if (type == Type::Root)
        _serverIcon = QIcon(QStringLiteral(":/icons/Resources/server.png"));
}

JenkinsTreeItem::JenkinsTreeItem(const QString &name, const QString &url,
                                 const JenkinsTreeItem::Type type)
    : _itemType(type),
      _name(name),
      _itemUrl(url)
{
    if (type == Type::Root)
        _serverIcon = QIcon(QStringLiteral(":/icons/Resources/server.png"));
}

JenkinsTreeItem::JenkinsTreeItem(const JenkinsTreeItem::Type type, const JenkinsJob &job)
    : _itemType(type), _job(job)
{
    _itemUrl = _job.jobUrl();
    if (type == Type::Root)
        _serverIcon = QIcon(QStringLiteral(":/icons/Resources/server.png"));
}

JenkinsTreeItem::JenkinsTreeItem(const JenkinsTreeItem &other)
    : TreeItem({other._name}), _itemType(other._itemType), _name(other._name), _job(other._job)
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
    if (role == Qt::DisplayRole)
    {
        switch (column)
        {
            case 0:
            {
                if (_itemType == Type::Root)
                    return _name;
                else
                    return _job.name().isEmpty() ? QStringLiteral("Unknown job") : _job.name();
            }
            break;
            case 1:
            {
                if (_itemType == Type::Job)
                {
                    if (_job.healthReports().isEmpty())
                    {
                        return QObject::tr("Health report is absent");
                    }
                    else
                    {
                        return _job.healthReports().at(0).description();
                    }
                }
                else
                    return QVariant();
            }
            break;
            case 2:
            {
                if (_itemType == Type::Job)
                    return QStringLiteral("TBD");
                else
                    return QVariant();
            }
            break;
            default:
                return QVariant();
                break;
        }
    }
    // FIXME: remove commented code
    //    else if (role == Qt::ToolTipRole && column == 0 && _itemType == Type::Job)
    //    {
    //        QStringList data;
    //        data.append(QStringLiteral("Last build URL: ") + _job.buildInfo().url());
    //        data.append(QStringLiteral("number: ") + QString::number(_job.buildInfo().number()));
    //        data.append(QStringLiteral("Display name: ") + _job.buildInfo().displayName());
    //        data.append(
    //            QStringLiteral("Duration: ")
    //            + QTime(0, 0, 0,
    //            _job.buildInfo().duration()).toString(QStringLiteral("hh::mm::ss")));
    //        data.append(QStringLiteral("timestamp: ")
    //                    + _job.buildInfo().timestamp().toString(QStringLiteral("dd.MM.yyyy
    //                    hh:mm:ss")));
    //        return data.join(QLatin1Char('\n'));
    //    }
    else if (role == Qt::DecorationRole)
    {
        if (_itemType == Type::Job)
        {
            if (column == 0)
                return QIcon(_job.colorIcon());
            else if (column == 1)
                return _job.healthIcon();
            else
                return QVariant();
        }
        else if (_itemType == Type::Root)
        {
            if (column == 0)
                return _serverIcon;
            else
                return QVariant();
        }
        else
            return QVariant();
    }
    else if (role == JobRoles::IsRunningRole && column == 0 && _itemType == Type::Job)
    {
        return _job.isRunning();
    }

    else
        return QVariant();
    return QVariant();
}

JenkinsJob JenkinsTreeItem::job() const { return _job; }

void JenkinsTreeItem::setJob(const JenkinsJob &job) { _job = job; }

QString JenkinsTreeItem::itemUrl() const { return _itemUrl; }

void JenkinsTreeItem::setItemUrl(const QString &itemUrl) { _itemUrl = itemUrl; }
