#ifndef JENKINSPANE_H
#define JENKINSPANE_H

#include <coreplugin/ioutputpane.h>
#include <utils/outputformat.h>

#include <QTreeView>
#include "jenkinsjobsmodel.h"
#include "jenkinsviewcombobox.h"

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsJob;
class JenkinsTreeItemDelegate;
class JenkinsJobsModel;

class JenkinsPane : public Core::IOutputPane
{
    Q_OBJECT
public:
    JenkinsPane(QObject *parent = 0);
    ~JenkinsPane();

    // IOutputPane interface
public:
    QWidget *outputWidget(QWidget *) { return _view; }
    QList< QWidget * > toolBarWidgets() const;
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

    ViewInfo getSelectedOrDefaultView() const;
signals:
    void buildHistoryRequested(const JenkinsJob job);
    void currentViewChanged();

public slots:
    void setJenkinsSettings(JenkinsSettings settings);
    void updateViews(const QSet<ViewInfo> &views);

private slots:
    void onCustomContextMenuRequested(const QPoint &point);
    void requestBuildHistory();
    void openInBrowser();

private:
    QTreeView *_view;
    QModelIndex _contextMenuIndex;
    JenkinsSettings _settings;
    JenkinsTreeItemDelegate *_delegate;
    JenkinsJobsModel *_model;
    JenkinsViewComboBox *_jenkinsViewComboBox;
};
}
}

#endif  // JENKINSPANE_H
