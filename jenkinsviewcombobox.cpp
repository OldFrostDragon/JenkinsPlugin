#include "jenkinsviewcombobox.h"

using namespace JenkinsPlugin::Internal;

JenkinsViewComboBox::JenkinsViewComboBox(JenkinsSettings settings, QWidget *parent)
    : QComboBox(parent), _settings(settings)
{
}

void JenkinsViewComboBox::updateViews(const QSet< ViewInfo > &jenkinsViews)
{
    int currentViewIndex = currentIndex();
    ViewInfo selectedInfo;
    if (currentViewIndex >= 0)
        selectedInfo = _jenkinsViews.at(currentViewIndex);
    _jenkinsViews = jenkinsViews.toList();
    std::sort(_jenkinsViews.begin(), _jenkinsViews.end(),
              [](const ViewInfo first, const ViewInfo second)
              {
                  return first.name < second.name;
              });

    int newIndexOfExitingView = 0;

    bool wasBlocked = blockSignals(true);
    clear();
    for (int i = 0; i < _jenkinsViews.size(); ++i)
    {
        if (_jenkinsViews[i].url == selectedInfo.url)
            newIndexOfExitingView = i;
        addItem(_jenkinsViews[i].name);
    }
    setCurrentIndex(newIndexOfExitingView);
    blockSignals(wasBlocked);
}

ViewInfo JenkinsViewComboBox::getSelectedOrDefaultView() const
{
    ViewInfo currentView;
    if(currentIndex() == -1)
    {
        currentView.name = QStringLiteral("root");
        currentView.url = QUrl(_settings.jenkinsUrl());
    }
    else
        currentView = _jenkinsViews.at(currentIndex());
    return  currentView;
}

void JenkinsViewComboBox::setJenkinsSettings(const JenkinsSettings &settings)
{
    _settings = settings;
}
