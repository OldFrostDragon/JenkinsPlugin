#include "jenkinstreeitemdelegate.h"
#include "jenkinsjobsmodel.h"

#include <QPainter>
#include <QTextDocument>

using namespace JenkinsPlugin::Internal;

JenkinsTreeItemDelegate::JenkinsTreeItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void JenkinsTreeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    painter->save();

    QTextDocument doc;
    doc.setDocumentMargin(1);

    bool isRunning = index.data(JenkinsTreeItem::JobRoles::IsRunningRole).toBool();
    if (isRunning)
        doc.setHtml(opt.text + QStringLiteral("    <i>[Running]</i>"));
    else
        doc.setHtml(opt.text);

    opt.text = QStringLiteral("");
//    opt.state &= ~QStyle::State_Selected;
//    opt.state &= ~QStyle::State_HasFocus;
//    opt.state &= ~QStyle::State_MouseOver;

    opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);

    painter->translate(opt.rect.left() + 20, opt.rect.top());
    QRect clip(0, 0, opt.rect.width(), opt.rect.height());
    doc.drawContents(painter, clip);

    painter->restore();
}

QSize JenkinsTreeItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setDocumentMargin(1);
    bool isRunning = index.data(JenkinsTreeItem::JobRoles::IsRunningRole).toBool();
    if (isRunning)
        doc.setHtml(options.text + QStringLiteral("    <i>[Running]</i>"));
    else
        doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());

    QSize size = QStyledItemDelegate::sizeHint(option, index);

    return QSize(size.width() > doc.idealWidth() ? size.width() : doc.idealWidth(),
                 size.height() > doc.size().height() ? size.height() : doc.size().height());
}
