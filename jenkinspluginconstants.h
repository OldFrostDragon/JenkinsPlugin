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
const char JENKINS_SERVER_ICON[] = ":/icons/Resources/server.png";

//settings
const char SETTINGS_GROUP[] = "JenkinsPlugin";
const char URL[] = "Url";
const char PORT[] = "Port";
const char USERNAME[] = "Username";
const char API_TOKEN[] = "ApiToken";


//Health report icons
const char HEALTH_80PLUS[] = ":/icons/Resources/health-80plus.png";
const char HEALTH_60TO79[] = ":/icons/Resources/health-60to79.png";
const char HEALTH_40TO59[] = ":/icons/Resources/health-40to59.png";
const char HEALTH_20TO39[] = ":/icons/Resources/health-20to39.png";
const char HEALTH_00TO19[] = ":/icons/Resources/health-00to19.png";

} // namespace JenkinsPlugin
} // namespace Constants

#endif // JENKINSPLUGINCONSTANTS_H
