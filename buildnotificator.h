#ifndef BUILDNOTIFICATOR_H
#define BUILDNOTIFICATOR_H

#include <QObject>

#include "jenkinsjob.h"
#include "jenkinssettings.h"

namespace JenkinsCI
{
namespace Internal
{
class BuildNotificator : public QObject
{
    Q_OBJECT
public:
    explicit BuildNotificator(QWidget *notificationParent, QObject *parent = 0);

    JenkinsSettings settings() const;
    void setSettings(const JenkinsSettings &settings);

public slots:
    void notifyAboutFailure(const JenkinsJob &job);
    void clear() { _alreadyReportedFailures.clear(); }

private:
    // key - job name
    QMap< QString, JenkinsJob::BuildUrl > _alreadyReportedFailures;
    JenkinsSettings _settings;
    QWidget *_notificationParent{nullptr};
};
}
}
#endif  // BUILDNOTIFICATOR_H
