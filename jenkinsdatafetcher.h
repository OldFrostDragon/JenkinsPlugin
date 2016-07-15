#ifndef JENKINSDATAFETCHER_H
#define JENKINSDATAFETCHER_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QTimer>

#include <memory>

#include "jenkinssettings.h"
#include "restrequestbuilder.h"
#include "jenkinsjob.h"

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsDataFetcher : public QObject
{
    Q_OBJECT
public:
    explicit JenkinsDataFetcher(std::shared_ptr< RestRequestBuilder > restRequestBuilder,
                                QObject *parent = 0);

    void getAvaliableJobs();

    void readIsQueuedFlagFor(QJsonObject &jsonObject, JenkinsJob &job);

signals:
    void jobsUpdated(QList< JenkinsJob >);
    void jobUpdated(JenkinsJob);

public slots:
    void forceRefetch();

private slots:

    void readReply(QNetworkReply *reply);
    void switchToNextFetchStep();
    //    void authentificateUser();

private:
    enum class State
    {
        Ready,
        FetchingJobs,
        FetchingJobDetails,
        FetchingLastBuild,
    };

    QList< JenkinsJob > fetchJobList(QNetworkReply *reply);
    JenkinsJob fillBuildDetails(QNetworkReply *reply);
    JenkinsJob fillLastBuildInfoDetails(QNetworkReply *reply);

    void sendDetailsRequestForFirstJob();

    void sendLastBuildInfoForFirstJob();

    void readBuildInfoFor(JenkinsJob &job, QJsonObject &jsonObject);
    void readHealthReportsPartFor(QJsonObject &jsonObject, JenkinsJob &job);
    void readBuildsListFor(JenkinsJob &job, QJsonObject &jsonObject);
    void readBuildableFlagFor(JenkinsJob &job, QJsonObject &jsonObject);

    QList< JenkinsJob > _jobsForDetalization;
    QList<JenkinsJob> _jobsForLastBuildDetalization;

    QNetworkAccessManager *_manager;
    State _state{State::Ready};
    QTimer *_timer;
    std::shared_ptr< RestRequestBuilder > _restRequestBuilder;

};
}
}

#endif  // JENKINSDATAFETCHER_H
