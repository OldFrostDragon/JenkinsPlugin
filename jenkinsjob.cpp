#include "jenkinsjob.h"

#include "jenkinspluginconstants.h"

using namespace JenkinsPlugin::Internal;

const QMap< QString, QString > HealthReport::ICON_CLASS_ICONS = {
    {QStringLiteral("icon-health-80plus"), QLatin1String(JenkinsPlugin::Constants::HEALTH_80PLUS)},
    {QStringLiteral("icon-health-60to79"), QLatin1String(JenkinsPlugin::Constants::HEALTH_60TO79)},
    {QStringLiteral("icon-health-40to59"), QLatin1String(JenkinsPlugin::Constants::HEALTH_40TO59)},
    {QStringLiteral("icon-health-20to39"), QLatin1String(JenkinsPlugin::Constants::HEALTH_20TO39)},
    {QStringLiteral("icon-health-00to19"), QLatin1String(JenkinsPlugin::Constants::HEALTH_00TO19)},
};

QString JenkinsJob::jobUrl() const { return _jobUrl; }

void JenkinsJob::setJobUrl(const QString &jobUrl) { _jobUrl = jobUrl; }

QString JenkinsJob::name() const { return _name; }

void JenkinsJob::setName(const QString &name) { _name = name; }

void JenkinsJob::setBuildStatus(const QString &colorEntry)
{
    QString entry = colorEntry;
    if (entry.endsWith(QStringLiteral("_anime")))
    {
        _isRunning = true;
        entry.chop(6);
    }
    else
        _isRunning = false;
    if (entry == QStringLiteral("blue"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::SUCCESS_ICON);
    else if (entry == QStringLiteral("red"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::FAIL_ICON);
    else if (entry == QStringLiteral("yellow"))
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::UNSTABLE_ICON);
    else
        _colorIcon = QLatin1String(JenkinsPlugin::Constants::NOT_BUILT_ICON);
}

bool JenkinsJob::isRunning() const { return _isRunning; }

QString JenkinsJob::colorIcon() const { return _colorIcon; }

QList< HealthReport > JenkinsJob::healthReports() const { return _healthReports; }

void JenkinsJob::setHealthReports(const QList< HealthReport > &healthReports)
{
    _healthReports = healthReports;
    QString iconPath;
    int score = 1000;
    foreach (const HealthReport &report, _healthReports)
    {
        if (report.score() < score)
        {
            score = report.score();
            iconPath = report.getIconFile();
        }
    }
    if (!iconPath.isEmpty())
        _healthIcon = QIcon(iconPath);
    else
        _healthIcon = QIcon();
    _healthIconPath = iconPath;
}

QIcon JenkinsJob::healthIcon() const { return _healthIcon; }

QList< JenkinsJob::BuildUrl > JenkinsJob::buildUrls() const { return _buildUrls; }

void JenkinsJob::setBuildUrls(const QList< BuildUrl > &buildUrls) { _buildUrls = buildUrls; }

QString JenkinsJob::healthIconPath() const
{
    return _healthIconPath;
}

bool JenkinsJob::isBuildable() const
{
    return _isBuildable;
}

void JenkinsJob::setIsBuildable(bool isBuildable)
{
    _isBuildable = isBuildable;
}

bool JenkinsJob::isQueued() const
{
    return _isQueued;
}

void JenkinsJob::setIsQueued(bool isQueued)
{
    _isQueued = isQueued;
}

QDateTime JenkinsJob::lastBuildDate() const
{
    return _lastBuildDate;
}

void JenkinsJob::setLastBuildDate(const QDateTime &lastBuildDate)
{
    _lastBuildDate = lastBuildDate;
}

void JenkinsJob::setLastBuildDate(const quint64 &timestamp)
{
    //Jenkins returns timestamp value in miliseconds, but fromTime_t require seconds
    _lastBuildDate = QDateTime::fromTime_t(timestamp / 1000);
}

HealthReport::HealthReport(const int score, const QString &description,
                           const QString &iconClassName)
    : _score(score), _description(description), _iconClassName(iconClassName)
{
}

QString HealthReport::description() const { return _description; }

void HealthReport::setDescription(const QString &description) { _description = description; }

int HealthReport::score() const { return _score; }

void HealthReport::setScore(int score) { _score = score; }

QString HealthReport::iconClassName() const { return _iconClassName; }

void HealthReport::setIconClassName(const QString &iconClassName)
{
    _iconClassName = iconClassName;
}

