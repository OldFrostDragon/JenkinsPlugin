#ifndef BUILDHISTORYFETCHER_H
#define BUILDHISTORYFETCHER_H

#include <QObject>
#include <QDateTime>
#include <QtNetwork/QNetworkAccessManager>

#include <memory>

#include "restrequestbuilder.h"

namespace JenkinsPlugin
{
namespace Internal
{

class BuildInfo
{
public:
    enum class Result
    {
        Aborted,
        Failure,
        NotBuilt,
        Success,
        Unstable,
        Unknown,
    };

    BuildInfo() {}

    QString url() const;
    void setUrl(const QString &url);

    int number() const;
    void setNumber(int number);

    QDateTime timestamp() const;
    void setTimestamp(const QDateTime &timestamp);
    void setTimestamp(const qint64 timestamp);

    Result result() const;
    QString getResultIcon() const;

    void setResult(const Result &result);
    void setResult(const QString &result);

    QString displayName() const;
    void setDisplayName(const QString &displayName);

    QString fullDisplayName() const;
    void setFullDisplayName(const QString &fullDisplayName);

    QString description() const;
    void setDescription(const QString &description);

    int duration() const;
    void setDuration(int duration);

private:
    QString _url;
    int _number;
    QDateTime _timestamp;
    Result _result{Result::Unknown};
    QString _displayName;
    QString _fullDisplayName;
    QString _description;
    int _duration;  // in miliseconds
};

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
