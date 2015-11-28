#ifndef TESTREGEXDELEGATE_H
#define TESTREGEXDELEGATE_H

#include <string>
#include <vector>

#include <QStyledItemDelegate>

class QModelIndex;
class QWidget;
class QVariant;

class TestRegexDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
  TestRegexDelegate(std::vector<std::string> aValues,QObject *parent = 0);

  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
//  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
  std::vector<std::string> Items;

};
#endif


