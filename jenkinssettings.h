#ifndef JENKINSSETTINGS_H
#define JENKINSSETTINGS_H

#include <QString>

namespace JenkinsPlugin {
namespace Internal {

class JenkinsSettings {
public:
  JenkinsSettings();

  QString jenkinsUrl() const;
  void setJenkinsUrl(const QString &jenkinsUrl);

private:
//  QString _jenkinsUrl{QStringLiteral("http://jenkins.cyanogenmod.com")};
  QString _jenkinsUrl{QStringLiteral("https://ci.jenkins-ci.org/view/Libraries/")};
//  QString _jenkinsUrl{QStringLiteral("http://dotnet-ci.cloudapp.net/view/Roslyn/")};
};

}
}

#endif // JENKINSSETTINGS_H
