#include "jenkinsdatafetcher.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

using namespace JenkinsPlugin::Internal;

JenkinsDataFetcher::JenkinsDataFetcher(QObject *parent) : QObject(parent)
{
    _manager = new QNetworkAccessManager(this);
    connect(_manager, &QNetworkAccessManager::finished, this, &JenkinsDataFetcher::readReply);
}

JenkinsSettings JenkinsDataFetcher::jenkinsSettings() const { return _jenkinsSettings; }

void JenkinsDataFetcher::setJenkinsSettings(const JenkinsSettings &jenkinsSettings)
{
    _jenkinsSettings = jenkinsSettings;
}

void JenkinsDataFetcher::getAvaliableJobs()
{
    QString queryUrl = urlToRestApiUrl(_jenkinsSettings.jenkinsUrl());
    QNetworkRequest request{QUrl(queryUrl)};
    _manager->get(request);
}

QString JenkinsDataFetcher::urlToRestApiUrl(const QString &url)
{
    if (url.endsWith(QStringLiteral("/api/json")))
        return url;
    else if (url.endsWith(QStringLiteral("/")))
        return url + QStringLiteral("api/json");
    else
        return url + QStringLiteral("/api/json");
}

void JenkinsDataFetcher::readReply(QNetworkReply *reply)
{
    if (reply == nullptr)
    {
        qDebug() << "reply is null";
        return;
    }
    QByteArray data = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (jsonDoc.isNull())
    {
        qDebug() << "json is null";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    if (!jsonObject.contains(QStringLiteral("jobs")))
    {
        qDebug() << "jobs are absent";
        return;
    }
    QJsonArray jobsArray = jsonObject[QStringLiteral("jobs")].toArray();
    QList< JenkinsJob > parsedJobs;
    foreach (auto job, jobsArray)
    {
        QJsonObject jobObject = job.toObject();
        JenkinsJob jenkinsJob;
        if (jobObject.contains(QStringLiteral("name")))
            jenkinsJob.setName(jobObject[QStringLiteral("name")].toString());
        if (jobObject.contains(QStringLiteral("url")))
            jenkinsJob.setJobUrl(jobObject[QStringLiteral("url")].toString());
        parsedJobs.append(jenkinsJob);
    }
    emit jobsUpdated(parsedJobs);
}

QString JenkinsJob::jobUrl() const { return _jobUrl; }

void JenkinsJob::setJobUrl(const QString &jobUrl) { _jobUrl = jobUrl; }

QString JenkinsJob::name() const { return _name; }

void JenkinsJob::setName(const QString &name) { _name = name; }

QString JenkinsJob::color() const { return _color; }

void JenkinsJob::setColor(const QString &color) { _color = color; }
