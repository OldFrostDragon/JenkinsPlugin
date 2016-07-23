#include "jenkinsviewfetcher.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>

using namespace JenkinsCI::Internal;

JenkinsViewFetcher::JenkinsViewFetcher(const std::shared_ptr< RestRequestBuilder > builder)
    : _builder(builder)
{
    _manager = new QNetworkAccessManager(this);
    connect(_manager, &QNetworkAccessManager::finished, this, &JenkinsViewFetcher::readReply);

    _timer = new QTimer(this);
    _timer->setInterval(1000 * 10);  // 10 seconds
    connect(_timer, &QTimer::timeout, this, &JenkinsViewFetcher::fetchViews);
    startBackgroundFetching();
}

void JenkinsViewFetcher::fetchViews()
{
    QString rootUrl = _builder->jenkinsSettings().jenkinsUrl();
    rootUrl = _builder->urlToRestApiUrl(rootUrl);
    if(rootUrl.isEmpty())
    {
        qDebug() << "Jenkins root URL is empty";
        return;
    }
    QNetworkRequest request = _builder->buildRequest(rootUrl);
    _manager->get(request);
}

void JenkinsViewFetcher::startBackgroundFetching()
{
    _timer->start();
}

void JenkinsViewFetcher::stopBackgroundFetching()
{
    _timer->stop();
}

void JenkinsViewFetcher::readReply(QNetworkReply *reply)
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
    if (!jsonObject.contains(QStringLiteral("views")))
    {
        qDebug() << "views are absent";
        return;
    }

    QJsonArray viewsData = jsonObject[QStringLiteral("views")].toArray();
    QSet< ViewInfo > views;
    foreach (auto job, viewsData)
    {
        QJsonObject jobObject = job.toObject();
        ViewInfo info;
        if (jobObject.contains(QStringLiteral("name")))
            info.name = jobObject[QStringLiteral("name")].toString();
        if (jobObject.contains(QStringLiteral("url")))
            info.url = QUrl(jobObject[QStringLiteral("url")].toString());
        views.insert(info);
    }
    emit viewsFetched(views);
}
