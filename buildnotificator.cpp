#include "buildnotificator.h"

#include <QApplication>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/task.h>
#include <projectexplorer/taskhub.h>

#include "warningpopup.h"

using namespace JenkinsCI::Internal;

BuildNotificator::BuildNotificator(QWidget *notificationParent, QObject *parent)
    : QObject(parent), _notificationParent(notificationParent)
{
}

void BuildNotificator::notifyAboutFailure(const JenkinsJob &job)
{
    if (!_settings.notifyAboutFailedBuilds())
        return;
    JenkinsJob::BuildUrl lastBuildUrl = job.getLastBuildUrl();
    if (_alreadyReportedFailures.contains(job.name()))
    {
        JenkinsJob::BuildUrl reportedUrl = _alreadyReportedFailures[job.name()];
        if (reportedUrl == lastBuildUrl)
            return;
    }
    QUrl url(lastBuildUrl.url);
    url.setPort(_settings.port());
    QString message = QCoreApplication::translate("JenkinsPlugin::Task",
                                                  "Jenkins: %1 Build #%2 failed. Build date: %3.\n "
                                                  "See %4 for details")
                          .arg(job.name())
                          .arg(lastBuildUrl.number)
                          .arg(job.lastBuildDate().toString(QStringLiteral("dd.MM.yyyy hh:mm:ss")))
                          .arg(url.toString());
    ProjectExplorer::TaskHub::addTask(ProjectExplorer::Task::Error, message,
                                      ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);
    _alreadyReportedFailures.insert(job.name(), lastBuildUrl);

    WarningPopup *popup = new WarningPopup(_notificationParent);
    popup->setShowPeriod(_settings.popupShowPeriod());
    QString popupMessage
        = QCoreApplication::translate("JenkinsPlugin::Task", "<b>Jenkins:</b>\n \"%1\" #%2 failed")
              .arg(job.name())
              .arg(lastBuildUrl.number);
    popup->showPopup(popupMessage);
}

JenkinsSettings BuildNotificator::settings() const { return _settings; }

void BuildNotificator::setSettings(const JenkinsSettings &settings)
{
    _settings = settings;
    _alreadyReportedFailures.clear();
}
