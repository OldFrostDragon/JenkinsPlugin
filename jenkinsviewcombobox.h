#ifndef JENKINSVIEWCOMBOBOX_H
#define JENKINSVIEWCOMBOBOX_H

#include <QComboBox>

#include "jenkinsviewfetcher.h"
#include "jenkinssettings.h"

namespace JenkinsPlugin
{
namespace Internal
{
class JenkinsViewComboBox : public QComboBox
{
    Q_OBJECT
public:
    JenkinsViewComboBox(JenkinsSettings settings, QWidget *parent = nullptr);

    void updateViews(const QSet< ViewInfo > &jenkinsViews);

    ViewInfo getSelectedOrDefaultView() const;
    void setJenkinsSettings(const JenkinsSettings &settings);

signals:
    void jobResetRequired();

private:
    QList< ViewInfo > _jenkinsViews;
    JenkinsSettings _settings;
};
}
}
#endif  // JENKINSVIEWCOMBOBOX_H
