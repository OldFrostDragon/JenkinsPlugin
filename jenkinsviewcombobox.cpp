#include "jenkinsviewcombobox.h"

using namespace JenkinsPlugin::Internal;

JenkinsViewComboBox::JenkinsViewComboBox(JenkinsSettings settings, QWidget *parent)
    : QComboBox(parent), _settings(settings)
{
    connect(this, static_cast< void (QComboBox::*)(int) >(&QComboBox::currentIndexChanged), [=](int)
            {
                emit jobResetRequired();
            });
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
    blockSignals(wasBlocked);
    if (isJobUpdateRequired)
    {
        qDebug() << "job reset requested";
        emit jobResetRequired();
    }
    else
        qDebug() << "job reset was not requested";
}

ViewInfo JenkinsViewComboBox::getSelectedOrDefaultView() const
{
    qDebug() << "try to get current view";
    ViewInfo currentView;
    if (currentIndex() == -1)
    {
        currentView.name = QStringLiteral("root");
        currentView.url = QUrl(_settings.jenkinsUrl());
        qDebug() << "    default view";
    }
    else
    {
        currentView = _jenkinsViews.at(currentIndex());
        qDebug() << "    last used view";
    }
    return currentView;
}

void JenkinsViewComboBox::setJenkinsSettings(const JenkinsSettings &settings)
{
    _settings = settings;
    clear();
}
