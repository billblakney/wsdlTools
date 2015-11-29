#ifndef TESTREGEXDELEGATE_H
#define TESTREGEXDELEGATE_H

#include <string>
#include <vector>
#include <QComboBox>
#include <QStyledItemDelegate>
#include "DataStructModel.hh"
#include "StructorBuilder.hh"

class QModelIndex;
class QWidget;
class QVariant;

class TestRegexDelegate : public QStyledItemDelegate
{
Q_OBJECT
public:
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  TestRegexDelegate(DataStructModel *aDataStructModel,QObject *parent = 0);
//  TestRegexDelegate(std::vector<std::string> aValues,QObject *parent = 0);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  QWidget *createEditor(QWidget *parent,const QStyleOptionViewItem &option,
      const QModelIndex &index) const;

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  void setEditorData(QWidget *editor, const QModelIndex &index) const;

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  void setModelData(QWidget *editor,QAbstractItemModel *model,
      const QModelIndex &index) const;

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,
      const QModelIndex &index) const;

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
//  void paint(QPainter *painter,const QStyleOptionViewItem &option,
//      const QModelIndex &index) const;

private:
  DataStructModel          *_DataStructModel;
  std::vector<std::string>   Items;
};
#endif


