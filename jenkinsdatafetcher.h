#ifndef JENKINSDATAFETCHER_H
#define JENKINSDATAFETCHER_H

#include <QObject>

#include "jenkinssettings.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QTimer>
#include <QIcon>

#include "restrequestbuilder.h"
#include <memory>

namespace JenkinsPlugin
{
namespace Internal
{

class HealthReport
{
public:
    HealthReport() = default;
    HealthReport(const int score, const QString &description,
                 const QString &iconClassName = QString());

    QString description() const;
    void setDescription(const QString &description);

    int score() const;
    void setScore(int score);

    QString iconClassName() const;
    void setIconClassName(const QString &iconClassName);

    QString getIconFile() const { return ICON_CLASS_ICONS.value(_iconClassName); }
private:
    static const QMap< QString, QString > ICON_CLASS_ICONS;
    int _score{-1};
    QString _description;
    QString _iconClassName;
};

class JenkinsJob
{
public:
    struct BuildUrl
    {
        BuildUrl() {}
        BuildUrl(const int buildNumber, const QString buildUrl) : number(buildNumber), url(buildUrl)
        {
        }
        int number{1};
        QString url;
    };

    JenkinsJob() {}

    QString jobUrl() const;
    void setJobUrl(const QString &jobUrl);

    QString name() const;
    void setName(const QString &name);

    void setBuildStatus(const QString &colorEntry);

    bool isValid() { return !_jobUrl.isEmpty() && !_name.isEmpty(); }

    bool isRunning() const;
    QString colorIcon() const;

    QList< HealthReport > healthReports() const;
    void setHealthReports(const QList< HealthReport > &healthReports);
    QIcon healthIcon() const;

    QList< BuildUrl > buildUrls() const;
    void setBuildUrls(const QList< BuildUrl > &buildUrls);

private:
    QIcon _healthIcon;

    QString _jobUrl;
    QString _name;
    QString _color;
    QList< HealthReport > _healthReports;
    QList< BuildUrl > _buildUrls;
    bool _isRunning{false};
    QString _colorIcon;
};

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
