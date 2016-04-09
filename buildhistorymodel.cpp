#include "buildhistorymodel.h"

#include <QIcon>
using namespace JenkinsPlugin::Internal;

BuildHistoryModel::BuildHistoryModel(std::shared_ptr< RestRequestBuilder > restRequestBuilder)
{
    _buildHistoryFetcher = new BuildHistoryFetcher(restRequestBuilder);
    connect(_buildHistoryFetcher, &BuildHistoryFetcher::buildInfoFetched, this,
            &BuildHistoryModel::appendBuildInfo);
}

BuildHistoryModel::~BuildHistoryModel() { delete _buildHistoryFetcher; }

int BuildHistoryModel::rowCount(const QModelIndex &) const { return _buildHistory.size(); }

QVariant BuildHistoryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.isValid() && index.row() < _buildHistory.size())
            return QString(QStringLiteral("#%1 %2"))
                .arg(_buildHistory[index.row()].number())
                .arg(_buildHistory[index.row()].fullDisplayName());
        else
            return QVariant();
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.isValid() && index.row() < _buildHistory.size())
            return QIcon(_buildHistory[index.row()].getResultIcon());
        else
            return QVariant();
    }
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
