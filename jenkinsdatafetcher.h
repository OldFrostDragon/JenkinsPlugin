#ifndef JENKINSDATAFETCHER_H
#define JENKINSDATAFETCHER_H

#include <QObject>

#include "jenkinssettings.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QTimer>

#include "restrequestbuilder.h"
#include "jenkinsjob.h"
#include <memory>

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

signals:
    void jobsUpdated(QList< JenkinsJob >);
    void jobUpdated(JenkinsJob);

public slots:

private slots:
    void resetFetchProgress();

    void readReply(QNetworkReply *reply);
    void switchToNextFetchStep();
    //    void authentificateUser();

private:
    enum class State
    {
        Ready,
        FetchingJobs,
        FetchingJobDetails,
    };

    QList< JenkinsJob > fetchJobList(QNetworkReply *reply);
    JenkinsJob fillBuildDetails(QNetworkReply *reply);
    void sendDetailsRequestForFirstJob();

    QList< JenkinsJob > _jobsForDetalization;

    QNetworkAccessManager *_manager;
    State _state{State::Ready};
    QTimer *_timer;
    std::shared_ptr< RestRequestBuilder > _restRequestBuilder;
};
}
}

#endif  // JENKINSDATAFETCHER_H
