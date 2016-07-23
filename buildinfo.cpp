#include "buildinfo.h"
#include "jenkinspluginconstants.h"

using namespace JenkinsCI::Internal;

QString BuildInfo::url() const { return _url; }

void BuildInfo::setUrl(const QString &url) { _url = url; }

int BuildInfo::number() const { return _number; }

void BuildInfo::setNumber(int number) { _number = number; }

QDateTime BuildInfo::timestamp() const { return _timestamp; }

void BuildInfo::setTimestamp(const QDateTime &timestamp) { _timestamp = timestamp; }

void BuildInfo::setTimestamp(const qint64 timestamp)
{
    _timestamp = QDateTime::fromMSecsSinceEpoch(timestamp);
}

BuildInfo::Result BuildInfo::result() const { return _result; }

QString BuildInfo::getResultIcon() const
{
    switch (_result)
    {
        case Result::Success:
            return QLatin1String(JenkinsCI::Constants::SUCCESS_ICON);
        case Result::Failure:
            return QLatin1String(JenkinsCI::Constants::FAIL_ICON);
        case Result::Unstable:
            return QLatin1String(JenkinsCI::Constants::UNSTABLE_ICON);
        case Result::NotBuilt:
        case Result::Aborted:
        case Result::Unknown:
            return QLatin1String(JenkinsCI::Constants::NOT_BUILT_ICON);
    }
    return QLatin1String(JenkinsCI::Constants::NOT_BUILT_ICON);
}

void BuildInfo::setResult(const Result &result) { _result = result; }

void BuildInfo::setResult(const QString &result)
{
    if (result == QStringLiteral("SUCCESS"))
        _result = Result::Success;
    else if (result == QStringLiteral("ABORTED"))
        _result = Result::Aborted;
    else if (result == QStringLiteral("FAILURE"))
        _result = Result::Failure;
    else if (result == QStringLiteral("NOT_BUILT"))
        _result = Result::NotBuilt;
    else if (result == QStringLiteral("UNSTABLE"))
        _result = Result::Unstable;
    else
        _result = Result::Unknown;
}

QString BuildInfo::displayName() const { return _displayName; }

void BuildInfo::setDisplayName(const QString &displayName) { _displayName = displayName; }

QString BuildInfo::fullDisplayName() const { return _fullDisplayName; }

void BuildInfo::setFullDisplayName(const QString &fullDisplayName)
{
    _fullDisplayName = fullDisplayName;
}

QString BuildInfo::description() const { return _description; }

void BuildInfo::setDescription(const QString &description) { _description = description; }

int BuildInfo::duration() const { return _duration; }

void BuildInfo::setDuration(int duration) { _duration = duration; }

