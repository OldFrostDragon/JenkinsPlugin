#ifndef JENKINSJOB_H
#define JENKINSJOB_H

#include <QIcon>
#include <QMap>
#include <QDateTime>

namespace JenkinsCI
{
namespace Internal
{

/*!
 * \brief The HealthReport class holds information about health status of job. Use "healthReport" part
 * in JSON reply
 */
class HealthReport
{
public:
    HealthReport() = default;
    HealthReport(const int score, const QString &description,
                 const QString &iconClassName = QString());

    QString description() const;
    void setDescription(const QString &description);

    int score() const;
    void setScore(int score);

    QString iconClassName() const;
    void setIconClassName(const QString &iconClassName);

    QString getIconFile() const { return ICON_CLASS_ICONS.value(_iconClassName); }
private:
    static const QMap< QString, QString > ICON_CLASS_ICONS;
    int _score{-1};
    QString _description;
    QString _iconClassName;
};

/*!
 * \brief The JenkinsJob class holds information about job. Use entry of "jobs" array in JSON reply
 */
class JenkinsJob
{
public:
    struct BuildUrl
    {
        BuildUrl() {}
        BuildUrl(const int buildNumber, const QString buildUrl) : number(buildNumber), url(buildUrl)
        {
        }
        int number{1};
        QString url;
        bool operator==(const BuildUrl &rhs) const
        {
            return  number == rhs.number && url == rhs.url;
        }
    };

    enum BuildStatus
    {
        Success,
        Unstable,
        NotBuilt,
        Fail,
        Unknown
    };

    JenkinsJob() {}

    QString jobUrl() const;
    void setJobUrl(const QString &jobUrl);

    QString name() const;
    void setName(const QString &name);

    void setBuildStatus(const QString &colorEntry);

    bool isValid() { return !_jobUrl.isEmpty() && !_name.isEmpty(); }

    bool isRunning() const;
    QString colorIcon() const;

    QList< HealthReport > healthReports() const;
    void setHealthReports(const QList< HealthReport > &healthReports);
    QIcon healthIcon() const;

    QList< BuildUrl > buildUrls() const;
    void setBuildUrls(const QList< BuildUrl > &buildUrls);
    BuildUrl getLastBuildUrl() const;

    QString healthIconPath() const;

    bool isBuildable() const;
    void setIsBuildable(bool isBuildable);

    bool isQueued() const;
    void setIsQueued(bool isQueued);

    QDateTime lastBuildDate() const;
    void setLastBuildDate(const QDateTime &lastBuildDate);
    void setLastBuildDate(const quint64 &timestamp);

    BuildStatus buildStatus() const;

private:
    QIcon _healthIcon;
    QString _healthIconPath;

    QString _jobUrl;
    QString _name;
    QString _color;
    QList< HealthReport > _healthReports;
    QList< BuildUrl > _buildUrls;
    bool _isRunning{false};
    QString _colorIcon;

    bool _isBuildable{true};
    bool _isQueued{false};

    QDateTime _lastBuildDate;
    BuildStatus _buildStatus{BuildStatus::Unknown};
};

}
}

#endif  // JENKINSJOB_H
