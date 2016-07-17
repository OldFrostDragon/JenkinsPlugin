#include "fetchingtimeoutmanager.h"

using namespace JenkinsPlugin::Internal;

FetchingTimeoutManager::FetchingTimeoutManager(QObject *parent) : QObject(parent)
{
    _timer = new QTimer(this);
    _timer->setInterval(1000 * 10);  // 10 seconds
    connect(_timer, &QTimer::timeout, this, &FetchingTimeoutManager::processTimeout);
}

void FetchingTimeoutManager::processTimeout()
{
    emit viewUpdateRequested();
    if (_isViewsFetched)
        emit jobDataUpdateRequested();
}

bool FetchingTimeoutManager::isViewsFetched() const { return _isViewsFetched; }

void FetchingTimeoutManager::setIsViewsFetched(bool isViewsFetched)
{
    if(!_isViewsFetched && isViewsFetched)
        emit jobDataUpdateRequested();
    _isViewsFetched = isViewsFetched;
}

void FetchingTimeoutManager::triggerFetching()
{
    _timer->stop();
    processTimeout();
    _timer->start();
}

void FetchingTimeoutManager::startTimer()
{
    emit viewUpdateRequested();
    _timer->start();
}