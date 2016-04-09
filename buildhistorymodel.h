#ifndef BUILDHISTORYMODEL_H
#define BUILDHISTORYMODEL_H

#include <QAbstractListModel>

#include "buildhistoryfetcher.h"
#include "jenkinsdatafetcher.h"

namespace JenkinsPlugin
{
namespace Internal
{
class BuildHistoryModel : public QAbstractListModel
{
public:
    BuildHistoryModel(BuildHistoryFetcher *buildHistoryFetcher);
    ~BuildHistoryModel();

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void fetchBuildHistoryFor(JenkinsJob job);

private slots:
    void appendBuildInfo(BuildInfo buildInfo);

private:
    QList<BuildInfo> _buildHistory;
    BuildHistoryFetcher *_buildHistoryFetcher;
};
}
}
#endif  // BUILDHISTORYMODEL_H
