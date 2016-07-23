#ifndef JENKINSPANE_H
#define JENKINSPANE_H

#include <coreplugin/ioutputpane.h>
#include <utils/outputformat.h>

#include <QTreeView>
#include <QLabel>

#include "jenkinsjobsmodel.h"
#include "jenkinsviewcombobox.h"
#include "restrequestbuilder.h"

namespace JenkinsCI
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
    JenkinsPane(const std::shared_ptr<RestRequestBuilder> builder, QObject *parent = 0);
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

    ViewInfo getSelectedView() const;
signals:
    void buildHistoryRequested(const JenkinsJob job);
    void currentViewChanged();

public slots:
    void clearViews();
    void updateViews(const QSet<ViewInfo> &views);

private slots:
    void onCustomContextMenuRequested(const QPoint &point);
    void requestBuildHistory();
    void openInBrowser();

private:
    QTreeView *_view;
    QModelIndex _contextMenuIndex;
    JenkinsTreeItemDelegate *_delegate;
    JenkinsJobsModel *_model;
    QLabel *_viewLabel;
    JenkinsViewComboBox *_jenkinsViewComboBox;

    std::shared_ptr<RestRequestBuilder> _restRequestBuilder;
};
}
}

#endif  // JENKINSPANE_H
