#ifndef JENKINSJOBVIEW_H
#define JENKINSJOBVIEW_H

#include "utils/navigationtreeview.h"

namespace JenkinsPlugin
{
namespace Internal
{

class JenkinsJobView : public Utils::NavigationTreeView
{
public:
    JenkinsJobView(QWidget *parent = 0);
};

}
}
#endif  // JENKINSJOBVIEW_H
