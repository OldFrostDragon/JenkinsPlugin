DEFINES += JENKINSPLUGIN_LIBRARY

# JenkinsPlugin files
QT += network

CONFIG += c++14

SOURCES += jenkinspluginplugin.cpp \
    jenkinssettings.cpp \
    jenkinsdatafetcher.cpp \
    jenkinstreeitem.cpp \
    jenkinsjobsmodel.cpp \
    jenkinstreeitemdelegate.cpp \
    jenkinssettingswidget.cpp \
    optionspage.cpp \
    jenkinspane.cpp \
    restrequestbuilder.cpp \
    buildhistoryfetcher.cpp \
    buildhistorydialog.cpp \
    buildhistorymodel.cpp

HEADERS += jenkinspluginplugin.h \
        jenkinsplugin_global.h \
        jenkinspluginconstants.h \
    jenkinssettings.h \
    jenkinsdatafetcher.h \
    jenkinstreeitem.h \
    jenkinsjobsmodel.h \
    jenkinstreeitemdelegate.h \
    jenkinssettingswidget.h \
    optionspage.h \
    jenkinspane.h \
    restrequestbuilder.h \
    buildhistoryfetcher.h \
    buildhistorydialog.h \
    buildhistorymodel.h

CONFIG += c++14
# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/home/oldfrostdragon/Sources/qt-creator

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/home/oldfrostdragon/Sources/qt-creator-3.6.0-build

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on Mac
# USE_USER_DESTDIR = yes

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = JenkinsPlugin
QTC_LIB_DEPENDS += \
    # nothing here at this time

QTC_PLUGIN_DEPENDS += \
    coreplugin

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

FORMS += \
    jenkinsviewwidget.ui \
    jenkinssettingswidget.ui \
    buildhistorydialog.ui

RESOURCES += \
    resources.qrc
