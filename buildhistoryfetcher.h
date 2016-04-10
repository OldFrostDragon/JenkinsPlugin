#ifndef BUILDHISTORYFETCHER_H
#define BUILDHISTORYFETCHER_H

#include <QObject>
#include <QDateTime>
#include <QtNetwork/QNetworkAccessManager>

#include <memory>

#include "restrequestbuilder.h"
#include "buildinfo.h"

namespace JenkinsPlugin
{
namespace Internal
{

class BuildInfo;

class BuildHistoryFetcher : public QObject
{
    Q_OBJECT
public:
    explicit BuildHistoryFetcher(std::shared_ptr< RestRequestBuilder > restRequestBuilder,
                                 QObject *parent = 0);

signals:
    void buildInfoFetched(const BuildInfo &buildInfo);
    void nextBuildInfoFetchRequested();

public slots:
    void fetchBuildHistory(QStringList urls);

private slots:
    void readBuildInfoReply(QNetworkReply *reply);
    void processFirstPendingUrl();

private:
    QStringList _urlsToFetch;
    std::unique_ptr<QNetworkAccessManager> _manager;
    std::shared_ptr< RestRequestBuilder > _restRequestBuilder;
};
}
}
#endif  // BUILDHISTORYFETCHER_H
