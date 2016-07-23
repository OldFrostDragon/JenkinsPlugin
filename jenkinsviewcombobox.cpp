#include "jenkinsviewcombobox.h"

using namespace JenkinsCI::Internal;

JenkinsViewComboBox::JenkinsViewComboBox(QString lastUsedViewUrl, QWidget *parent)
    : QComboBox(parent)
{
    _lastUsedInfo.url = lastUsedViewUrl;
    connect(this, static_cast< void (QComboBox::*)(int) >(&QComboBox::currentIndexChanged), [=](int)
            {
                emit currentViewChanged();
            });
}

void JenkinsViewComboBox::updateViews(const QSet< ViewInfo > &jenkinsViews)
{
    int currentViewIndex = currentIndex();
    ViewInfo selectedInfo;
    if (currentViewIndex >= 0)
        selectedInfo = _jenkinsViews.at(currentViewIndex);
    else
        selectedInfo = _lastUsedInfo;

    _jenkinsViews = jenkinsViews.toList();
    std::sort(_jenkinsViews.begin(), _jenkinsViews.end(),
              [](const ViewInfo first, const ViewInfo second)
              {
                  return first.name < second.name;
              });

    int newIndexOfExitingView = -1;

    bool wasBlocked = blockSignals(true);
    clear();
    for (int i = 0; i < _jenkinsViews.size(); ++i)
    {
        if (_jenkinsViews[i].url == selectedInfo.url)
            newIndexOfExitingView = i;
        addItem(_jenkinsViews[i].name);
    }

    // do not force job update only if view is present and do not changed
    bool isJobUpdateRequired = !(newIndexOfExitingView >= 0 && currentViewIndex >= 0);
    if (newIndexOfExitingView != -1)
    {
        setCurrentIndex(newIndexOfExitingView);
    }
    else
    {
        newIndexOfExitingView = 0;
        setCurrentIndex(newIndexOfExitingView);
    }
    if(newIndexOfExitingView < _jenkinsViews.size())
        _lastUsedInfo = _jenkinsViews[newIndexOfExitingView];

    blockSignals(wasBlocked);
    if (isJobUpdateRequired)
        emit currentViewChanged();
}

ViewInfo JenkinsViewComboBox::getSelectedView() const
{
    ViewInfo currentView;
    if (currentIndex() != -1)
        currentView = _jenkinsViews.at(currentIndex());
    return currentView;
}

void JenkinsViewComboBox::clearViews()
{
    clear();
    _jenkinsViews.clear();
    emit currentViewChanged();
}
