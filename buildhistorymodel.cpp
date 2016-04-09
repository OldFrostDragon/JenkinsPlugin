#include "buildhistorymodel.h"

#include <QIcon>
using namespace JenkinsPlugin::Internal;

BuildHistoryModel::BuildHistoryModel(BuildHistoryFetcher *buildHistoryFetcher)
{
    _buildHistoryFetcher = buildHistoryFetcher;
    connect(_buildHistoryFetcher, &BuildHistoryFetcher::buildInfoFetched, this,
            &BuildHistoryModel::appendBuildInfo);
}

BuildHistoryModel::~BuildHistoryModel() { delete _buildHistoryFetcher; }

int BuildHistoryModel::rowCount(const QModelIndex &) const { return _buildHistory.size(); }

int BuildHistoryModel::columnCount(const QModelIndex &) const { return Column::ColumnCount; }

QVariant BuildHistoryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == Number)
            return _buildHistory[index.row()].number();
        else if (index.column() == BuildName)
            return _buildHistory[index.row()].displayName();
        else if (index.column() == Timestamp)
            return _buildHistory[index.row()].timestamp().toString(
                QStringLiteral("dd.MM.yyyy hh:mm:ss"));
        else if (index.column() == Duration)
        {
            int durationInSeconds = _buildHistory[index.row()].duration() / 1000;
            if (durationInSeconds < 60)
                return QString(QStringLiteral("%1 seconds")).arg(durationInSeconds);
            else if (durationInSeconds < 60 * 60)
            {
                int minutes = durationInSeconds / 60;
                int seconds = durationInSeconds % 60;
                return QString(QStringLiteral("%1 minutes %2 seconds")).arg(minutes).arg(seconds);
            }
            else
            {
                int hours = durationInSeconds / (60 * 60);
                int minutes = hours / 60;
                int seconds = hours % 60;
                return QString(QStringLiteral("%1 hours %2 minutes %3 seconds"))
                    .arg(hours)
                    .arg(minutes)
                    .arg(seconds);
            }
        }
        else
            return QVariant();
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.isValid() && index.row() < _buildHistory.size() && index.column() == BuildName)
            return QIcon(_buildHistory[index.row()].getResultIcon());
        else
            return QVariant();
    }
    else if (role == Qt::TextAlignmentRole)
    {
        if (index.isValid() && index.row() < _buildHistory.size()
            && (index.column() == Timestamp || index.column() == Duration))
            return Qt::AlignCenter;
        else
            return QVariant();
    }
    return QVariant();
}

QVariant BuildHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == Number)
            return QString(QStringLiteral("#"));
        else if (section == BuildName)
            return QObject::tr("Name");
        else if (section == Timestamp)
            return QObject::tr("Time");
        else if (section == Duration)
            return QObject::tr("Duration");
        else
            return QVariant();
    }
    else if (orientation == Qt::Horizontal && role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }
    else
        return QVariant();
}

void BuildHistoryModel::fetchBuildHistoryFor(JenkinsJob job)
{
    QStringList urls;
    foreach (JenkinsJob::BuildUrl buildUrl, job.buildUrls())
        urls.append(buildUrl.url);
    _buildHistoryFetcher->fetchBuildHistory(urls);
}

void BuildHistoryModel::appendBuildInfo(BuildInfo buildInfo)
{
    emit beginInsertRows(QModelIndex(), _buildHistory.size(), _buildHistory.size());
    _buildHistory.append(buildInfo);
    emit endInsertRows();
}
