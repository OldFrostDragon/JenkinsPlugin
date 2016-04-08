#ifndef JENKINSPANE_H
#define JENKINSPANE_H

#include <coreplugin/ioutputpane.h>
#include <utils/outputformat.h>

#include "jenkinsjobview.h"

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsPane : public Core::IOutputPane
{
public:
    JenkinsPane(QObject *parent = 0);
    ~JenkinsPane();

    // IOutputPane interface
public:
    QWidget *outputWidget(QWidget *) {return _view; }
    QList<QWidget *> toolBarWidgets() const;
    QString displayName() const;
    int priorityInStatusBar() const;
    void clearContents() {}
    void visibilityChanged(bool visible);
    void setFocus();
    bool hasFocus() const;
    bool canFocus() const;
    bool canNavigate() const;
    bool canNext() const;
    bool canPrevious() const;
    void goToNext() {}
    void goToPrev() {}

private:
    QTreeView *_view;
};

}
}

#endif  // JENKINSPANE_H
