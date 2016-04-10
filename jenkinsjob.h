#ifndef JENKINSJOB_H
#define JENKINSJOB_H

#include <QIcon>
#include <QMap>

namespace JenkinsPlugin
{
namespace Internal
{
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

    QString healthIconPath() const;

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
};

}
}

#endif  // JENKINSJOB_H
