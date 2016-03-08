#ifndef JENKINSPLUGINCONSTANTS_H
#define JENKINSPLUGINCONSTANTS_H

namespace JenkinsPlugin {
namespace Constants {

const char ACTION_ID[] = "JenkinsPlugin.Action";
const char PLUGIN_ID[] = "JenkinsPlugin.Jenkins";
const char MENU_ID[] = "JenkinsPlugin.Menu";

const char SUCCESS_ICON[]   = ":/icons/Resources/green_ball.ico";
const char FAIL_ICON[]      = ":/icons/Resources/red_ball.ico";
const char UNSTABLE_ICON[]  = ":/icons/Resources/yellow_ball.ico";
const char NOT_BUILT_ICON[] = ":/icons/Resources/grey_ball.ico";

const char JENKINS_LOGO[] = ":/icons/Resources/jenkins_logo.png";

//settings
const char SETTINGS_GROUP[] = "JenkinsPlugin";
const char URL[] = "Url";
const char PORT[] = "Port";
const char USERNAME[] = "Username";
const char API_TOKEN[] = "ApiToken";

} // namespace JenkinsPlugin
} // namespace Constants

#endif // JENKINSPLUGINCONSTANTS_H
