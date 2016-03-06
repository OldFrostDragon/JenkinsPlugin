#ifndef JENKINSVIEWWIDGET_H
#define JENKINSVIEWWIDGET_H

#include <QWidget>

#include <coreplugin/inavigationwidgetfactory.h>
#include <utils/navigationtreeview.h>

#include "jenkinsdatafetcher.h"
#include "jenkinsjobview.h"

QT_BEGIN_NAMESPACE
class QToolButton;
QT_END_NAMESPACE

namespace Core {
class IContext;
}

namespace Utils {
class ProgressIndicator;
}

namespace Ui {
class JenkinsViewWidget;
}

namespace JenkinsPlugin {
namespace Internal {


class JenkinsViewWidget : public QWidget {
  Q_OBJECT

public:
  explicit JenkinsViewWidget(QWidget *parent = 0);
  ~JenkinsViewWidget();
  QList<QToolButton *> createToolButtons();

public slots:

private:
  JenkinsJobView *_view;
  Ui::JenkinsViewWidget *ui;
  QToolButton *_refreshButton;
};

class JenkinsViewWidgetFactory : public Core::INavigationWidgetFactory
{
    Q_OBJECT

public:
    JenkinsViewWidgetFactory();

private:
    Core::NavigationView createWidget();

};

}
}

#endif // JENKINSVIEWWIDGET_H
