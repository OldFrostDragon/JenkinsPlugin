#include "buildhistorymodel.h"

#include <QIcon>
using namespace JenkinsPlugin::Internal;

BuildHistoryModel::BuildHistoryModel() {}

int BuildHistoryModel::rowCount(const QModelIndex &) const { return _buildHistory.size(); }

QVariant BuildHistoryModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.isValid() && index.row() < _buildHistory.size())
            return QString(QStringLiteral("#%1 %2"))
                .arg(_buildHistory[index.row()].number())
                .arg(_buildHistory[index.row()].displayName());
        else
            return QVariant();
    }
    else if(role == Qt::DecorationRole)
    {
        if (index.isValid() && index.row() < _buildHistory.size())
            return QIcon(_buildHistory[index.row()].getResultIcon());
        else
            return QVariant();
    }
    return QVariant();
}

void BuildHistoryModel::appendBuildInfo(BuildInfo buildInfo)
{
    emit beginInsertRows(QModelIndex(), _buildHistory.size(), _buildHistory.size());
    _buildHistory.append(buildInfo);
    emit endInsertRows();
}
