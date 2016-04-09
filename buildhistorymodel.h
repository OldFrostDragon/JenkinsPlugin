#ifndef BUILDHISTORYMODEL_H
#define BUILDHISTORYMODEL_H

#include <QAbstractListModel>

#include "buildhistoryfetcher.h"

namespace JenkinsPlugin
{
namespace Internal
{
class BuildHistoryModel : public QAbstractListModel
{
public:
    BuildHistoryModel();

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void appendBuildInfo(BuildInfo buildInfo);

private:
    QList<BuildInfo> _buildHistory;
};
}
}
#endif  // BUILDHISTORYMODEL_H
