#include "jenkinstreeitem.h"

#include <QIcon>
#include "jenkinspluginconstants.h"

#include <limits>

using namespace JenkinsCI::Internal;

JenkinsTreeItem::JenkinsTreeItem(const QString &name, const Type type)
    : _itemType(type), _name(name)
{
    if (type == Type::Root)
        _serverIcon = QIcon(QStringLiteral(":/icons/Resources/server.png"));
}

JenkinsTreeItem::JenkinsTreeItem(const QString &name, const QString &url,
                                 const JenkinsTreeItem::Type type)
    : _itemType(type), _name(name), _itemUrl(url)
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

JenkinsTreeItem::Type JenkinsTreeItem::itemType() const { return _itemType; }

void JenkinsTreeItem::setItemType(const Type &itemType) { _itemType = itemType; }

QString JenkinsTreeItem::name() const { return _name; }

void JenkinsTreeItem::setName(const QString &name) { _name = name; }

JenkinsTreeItem *JenkinsTreeItem::childItem(int row) const
{
    return static_cast< JenkinsTreeItem * >(childAt(row));
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
                    return _job.lastBuildDate().toString(QStringLiteral("dd MMMM yyyy hh:mm:ss"));
                else
                    return QVariant();
            }
            break;
            default:
                return QVariant();
                break;
        }
    }
    else if (role == Qt::ToolTipRole && _itemType == Type::Job)
    {
        const QString DESCRIPTION_CELL_TEMPLATE = QLatin1String("<td>%1</td>");
        const QString IMAGE_CELL_TEMPLATE
            = QLatin1String("<td align=\"center\"><img src=\"%1\", align=\"center\"></img></td>");

        QString htmlData = QLatin1String("<table border=\"0\">");
        foreach (HealthReport report, _job.healthReports())
        {
            htmlData.append(QStringLiteral("<tr>"));
            htmlData.append(IMAGE_CELL_TEMPLATE.arg(report.getIconFile()));
            htmlData.append(DESCRIPTION_CELL_TEMPLATE.arg(report.description()));
            htmlData.append(QStringLiteral("</tr>"));
        }
        htmlData.append(QStringLiteral("</table>"));
        return htmlData;
    }
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
    else if (role == JobRoles::AnimationOpacity && column == 0 && _itemType == Type::Job)
    {
        if (_job.isRunning())
            return _currectOpacityValue;
        else
            return 1.0;
    }
    else
        return QVariant();
    return QVariant();
}

bool JenkinsTreeItem::setData(int column, const QVariant &data, int role)
{
    if (column != 0)
        return false;
    if (static_cast< JobRoles >(role) != JobRoles::AnimationOpacity)
        return false;
    double newOpacity = data.toDouble();
    if (std::isgreater(newOpacity, 1.0))
        newOpacity = 1.0;
    if (std::isless(newOpacity, 0.0))
        newOpacity = 0.0;
    _currectOpacityValue = newOpacity;
    if(std::abs(_currectOpacityValue - 1.0) < std::numeric_limits<qreal>::epsilon())
        _isAscendingOpacityChange = false;
    else if(std::abs(_currectOpacityValue - 0.1) < std::numeric_limits<qreal>::epsilon())
        _isAscendingOpacityChange = true;
    return true;
}

JenkinsJob JenkinsTreeItem::job() const { return _job; }

void JenkinsTreeItem::setJob(const JenkinsJob &job) { _job = job; }

QString JenkinsTreeItem::itemUrl() const { return _itemUrl; }

void JenkinsTreeItem::setItemUrl(const QString &itemUrl) { _itemUrl = itemUrl; }

qreal JenkinsTreeItem::currentOpacityValue() const
{
    return _currectOpacityValue;
}

qreal JenkinsTreeItem::getNextOpacityValue() const
{
    if(_isAscendingOpacityChange)
        return _currectOpacityValue + 0.1;
    else
        return _currectOpacityValue - 0.1;
}
