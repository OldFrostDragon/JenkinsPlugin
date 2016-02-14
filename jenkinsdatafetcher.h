#ifndef JENKINSDATAFETCHER_H
#define JENKINSDATAFETCHER_H

#include <QObject>

#include "jenkinssettings.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QNetworkReply>

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsJob
{
public:
    JenkinsJob() {}

    QString jobUrl() const;
    void setJobUrl(const QString &jobUrl);

    QString name() const;
    void setName(const QString &name);

    QString color() const;
    void setColor(const QString &color);

private:
    QString _jobUrl;
    QString _name;
    QString _color;
};

class JenkinsDataFetcher : public QObject
{
    Q_OBJECT
public:
    explicit JenkinsDataFetcher(QObject *parent = 0);

    JenkinsSettings jenkinsSettings() const;
    void setJenkinsSettings(const JenkinsSettings &jenkinsSettings);

    void getAvaliableJobs();

    static QString urlToRestApiUrl(const QString &url);

signals:
    void jobsUpdated(QList<JenkinsJob>);

public slots:

private slots:
    void readReply(QNetworkReply *reply);

private:
    JenkinsSettings _jenkinsSettings;
    QNetworkAccessManager *_manager;
};
}
}

#endif  // JENKINSDATAFETCHER_H
