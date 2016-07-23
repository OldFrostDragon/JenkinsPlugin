#include "fetchingtimeoutmanager.h"

using namespace JenkinsCI::Internal;

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
    {
        if (_forcedJobUpdateRequested)
        {
            _forcedJobUpdateRequested = false;
            emit jobForcedUpdateRequested();
        }
        else
            emit jobDataUpdateRequested();
    }
}

bool FetchingTimeoutManager::isViewsFetched() const { return _isViewsFetched; }

void FetchingTimeoutManager::setIsViewsFetched(bool isViewsFetched)
{
    if (!_isViewsFetched && isViewsFetched)
        emit jobDataUpdateRequested();
    _isViewsFetched = isViewsFetched;
}

void FetchingTimeoutManager::triggerFetching(FetchType fetchType)
{
    _timer->stop();
    if (fetchType == FetchType::ForcedJobFetching)
        _forcedJobUpdateRequested = true;
    processTimeout();
    _timer->start();
}

void FetchingTimeoutManager::startTimer()
{
    emit viewUpdateRequested();
    _timer->start();
}
