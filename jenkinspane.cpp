#include "jenkinspane.h"

#include "jenkinsjobsmodel.h"
#include "jenkinstreeitemdelegate.h"

#include <QHeaderView>

using namespace JenkinsPlugin::Internal;

JenkinsPane::JenkinsPane(QObject *parent) : Core::IOutputPane(parent)
{
    _view = new QTreeView();
    _view->setHeaderHidden(false);
    JenkinsJobsModel *model = JenkinsJobsModel::instance();
    _view->setModel(model);

    _view->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    _view->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _view->header()->setSectionResizeMode(2, QHeaderView::Stretch);

    //_view->setItemDelegateForColumn(0, new JenkinsTreeItemDelegate(this));
}

JenkinsPane::~JenkinsPane()
{
    delete _view;
}

QList<QWidget *> JenkinsPane::toolBarWidgets() const
{
    return QList<QWidget *>{};
}

QString JenkinsPane::displayName() const
{
    return tr("Jenkins");
}

int JenkinsPane::priorityInStatusBar() const
{
    return 0;
}

void JenkinsPane::visibilityChanged(bool visible)
{
    _view->setVisible(visible);
}

void JenkinsPane::setFocus()
{
    _view->setFocus();
}

bool JenkinsPane::hasFocus() const
{
    return _view->hasFocus();
}

bool JenkinsPane::canFocus() const
{
    return true;
}

bool JenkinsPane::canNavigate() const
{
    return false;
}

bool JenkinsPane::canNext() const
{
    return false;
}

bool JenkinsPane::canPrevious() const
{
    return false;
}
