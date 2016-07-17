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
    JenkinsViewComboBox(QWidget *parent = nullptr);

    void updateViews(const QSet< ViewInfo > &jenkinsViews);

    ViewInfo getSelectedView() const; //may be empty
    void clearViews();

signals:
    void currentViewChanged();

private:
    QList< ViewInfo > _jenkinsViews;
};
}
}
#endif  // JENKINSVIEWCOMBOBOX_H
