#ifndef FETCHINGTIMEOUTMANAGER_H
#define FETCHINGTIMEOUTMANAGER_H

#include <QObject>
#include <QTimer>

namespace JenkinsCI
{
namespace Internal
{

/*!
 * \brief The FetchingTimeoutManager class schedules views and job fetching process. It syncronize
 * all avaliable Jenkins fetchers.
 * Typical flow is to fetch avaliable views and then, if user selected view is present is fetched
 * ones, fetch avaliable jobs for selected view. It use timer for continious fetch triggering.
 */
class FetchingTimeoutManager : public QObject
{
    Q_OBJECT

public:
    enum class FetchType
    {
        NormalJobFetching,
        ForcedJobFetching
    };

    explicit FetchingTimeoutManager(QObject *parent = 0);

    bool isViewsFetched() const;
    void setIsViewsFetched(bool isViewsFetched);

    void triggerFetching(FetchType fetchType);

signals:
    void viewUpdateRequested();
    void jobDataUpdateRequested();
    void jobForcedUpdateRequested();

public slots:
    void startTimer();

private slots:
    void processTimeout();

private:
    QTimer *_timer;
    bool _isViewsFetched{false};
    bool _forcedJobUpdateRequested{false};
};
}
}
#endif  // FETCHINGTIMEOUTMANAGER_H
