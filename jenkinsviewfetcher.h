#ifndef JENKINSVIEWFETCHER_H
#define JENKINSVIEWFETCHER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QTimer>

#include <memory>

#include "restrequestbuilder.h"

namespace JenkinsCI
{
namespace Internal
{

struct ViewInfo
{
    QUrl url;
    QString name;
};

inline uint qHash(const ViewInfo &key, uint seed)
{
    return qHash(key.url, seed) ^ qHash(key.name, seed);
}

inline bool operator==(const ViewInfo &first, const ViewInfo &second)
{
    return first.name == second.name && first.url == second.url;
}

class JenkinsViewFetcher : public QObject
{
    Q_OBJECT
public:
    explicit JenkinsViewFetcher(const std::shared_ptr< RestRequestBuilder > builder);

    void fetchViews();

signals:
    void viewsFetched(QSet< ViewInfo > views);

public:
    void startBackgroundFetching();
    void stopBackgroundFetching();

private:
    void readReply(QNetworkReply *reply);

private:
    std::shared_ptr< RestRequestBuilder > _builder;
    QNetworkAccessManager *_manager;
    QTimer *_timer;
};
}
}

#endif  // JENKINSVIEWFETCHER_H
