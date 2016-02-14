#include "jenkinsviewwidget.h"
#include "ui_jenkinsviewwidget.h"

#include "jenkinspluginconstants.h"
#include <coreplugin/coreconstants.h>

#include <QToolButton>

using namespace JenkinsPlugin::Internal;

JenkinsViewWidget::JenkinsViewWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::JenkinsViewWidget) {
  ui->setupUi(this);
}

JenkinsViewWidget::~JenkinsViewWidget() { delete ui; }

QList<QToolButton *> JenkinsViewWidget::createToolButtons() {

  QList<QToolButton *> list;

  _refreshButton = new QToolButton(this);
  _refreshButton->setIcon(QIcon(QLatin1String(Core::Constants::ICON_NEXT)));
  _refreshButton->setToolTip(tr("Refresh"));
  _refreshButton->setProperty("noArrow", true);

  list << _refreshButton;
  return list;
}

JenkinsViewWidgetFactory::JenkinsViewWidgetFactory() {
  setDisplayName(tr("Jenkins"));
  setId(JenkinsPlugin::Constants::PLUGIN_ID);
  setPriority(999);
}

Core::NavigationView JenkinsViewWidgetFactory::createWidget() {
  JenkinsViewWidget *widget = new JenkinsViewWidget;
  Core::NavigationView view;
  view.widget = widget;
  view.dockToolBarWidgets = widget->createToolButtons();
  return view;
}
