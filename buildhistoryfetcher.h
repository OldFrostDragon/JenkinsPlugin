#ifndef BUILDHISTORYFETCHER_H
#define BUILDHISTORYFETCHER_H

#include <QObject>

namespace JenkinsPlugin
{
namespace Internal
{
class BuildHistoryFetcher : public QObject
{
    Q_OBJECT
public:
    explicit BuildHistoryFetcher(QObject *parent = 0);

signals:

public slots:
};
}
}
#endif  // BUILDHISTORYFETCHER_H
