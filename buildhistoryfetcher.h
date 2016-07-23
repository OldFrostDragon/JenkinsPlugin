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

/*!
 * \brief The BuildHistoryFetcher class fetch information about builds using its URLs. It depends on
 * \c RestRequestBuilder and automatically convert given URLs to REST API URLs. It works
 * asynchronously and emit \c buildInfoFetched(const BuildInfo) signal when new BuildInfo data was
 * parsed. \c fetchBuildHistory(QStringList) always cancels exeting fetch process and starts new one
 */
class BuildHistoryFetcher : public QObject
{
    Q_OBJECT
public:
    explicit BuildHistoryFetcher(std::shared_ptr< RestRequestBuilder > restRequestBuilder,
                                 QObject *parent = 0);

    std::shared_ptr< RestRequestBuilder > restRequestBuilder() const;

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
    std::unique_ptr< QNetworkAccessManager > _manager;
    std::shared_ptr< RestRequestBuilder > _restRequestBuilder;
};
}
}
#endif  // BUILDHISTORYFETCHER_H
