#ifndef JENKINSTREEITEMDELEGATE_H
#define JENKINSTREEITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace JenkinsCI
{
namespace Internal
{
class JenkinsTreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    JenkinsTreeItemDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};
}
}
#endif  // JENKINSTREEITEMDELEGATE_H
