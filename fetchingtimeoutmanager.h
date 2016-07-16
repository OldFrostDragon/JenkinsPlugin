#ifndef FETCHINGTIMEOUTMANAGER_H
#define FETCHINGTIMEOUTMANAGER_H

#include <QObject>
#include <QTimer>

namespace JenkinsPlugin
{
namespace Internal
{

class FetchingTimeoutManager : public QObject
{
    Q_OBJECT
public:
    explicit FetchingTimeoutManager(QObject *parent = 0);

    bool isViewsFetched() const;
    void setIsViewsFetched(bool isViewsFetched);

    void triggerFetching();

signals:
    void viewUpdateRequested();
    void jobDataUpdateRequested();

public slots:
    void startTimer();

private slots:
    void processTimeout();

private:
    QTimer *_timer;
    bool _isViewsFetched{false};
};
}
}
#endif  // FETCHINGTIMEOUTMANAGER_H
