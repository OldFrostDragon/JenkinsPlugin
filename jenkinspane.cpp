#include "jenkinspane.h"

#include "jenkinsjobsmodel.h"
#include "jenkinstreeitemdelegate.h"

using namespace JenkinsPlugin::Internal;

JenkinsPane::JenkinsPane(QObject *parent) : Core::IOutputPane(parent)
{
    _view = new JenkinsJobView();
    JenkinsJobsModel *model = JenkinsJobsModel::instance();
    _view->setModel(model);
    _view->setItemDelegate(new JenkinsTreeItemDelegate(this));
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
