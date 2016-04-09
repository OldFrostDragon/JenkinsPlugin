#include "jenkinspane.h"

#include "jenkinstreeitemdelegate.h"

#include <QHeaderView>
#include <QMenu>

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

    _view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_view, &QTreeView::customContextMenuRequested, this,
            &JenkinsPane::onCustomContextMenuRequested);
    //_view->setItemDelegateForColumn(0, new JenkinsTreeItemDelegate(this));
}

JenkinsPane::~JenkinsPane() { delete _view; }

QList< QWidget * > JenkinsPane::toolBarWidgets() const { return QList< QWidget * >{}; }

QString JenkinsPane::displayName() const { return tr("Jenkins"); }

int JenkinsPane::priorityInStatusBar() const { return 0; }

void JenkinsPane::visibilityChanged(bool visible) { _view->setVisible(visible); }

void JenkinsPane::setFocus() { _view->setFocus(); }

bool JenkinsPane::hasFocus() const { return _view->hasFocus(); }

bool JenkinsPane::canFocus() const { return true; }

bool JenkinsPane::canNavigate() const { return false; }

bool JenkinsPane::canNext() const { return false; }

bool JenkinsPane::canPrevious() const { return false; }

void JenkinsPane::onCustomContextMenuRequested(const QPoint &point)
{
    QModelIndex index = _view->indexAt(point);
    if (!index.isValid())
        return;
    JenkinsTreeItem *item = static_cast< JenkinsTreeItem * >(index.internalPointer());
    if (item == nullptr)
        return;
    if (item->itemType() != JenkinsTreeItem::Type::Job)
        return;
    _contextMenuJob = item->job();

    QMenu *contextMenu = new QMenu(_view);
    QAction *buildHistoryEntry = new QAction(QObject::tr("show build history"), contextMenu);
    contextMenu->addAction(buildHistoryEntry);
    connect(buildHistoryEntry, &QAction::triggered, this, &JenkinsPane::requestBuildHistory);
    contextMenu->exec(_view->mapToGlobal(point));
}

void JenkinsPane::requestBuildHistory() { emit buildHistoryRequested(_contextMenuJob); }
