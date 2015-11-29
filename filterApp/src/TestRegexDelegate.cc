#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>
#include "DataStructModel.hh"
#include "FieldItem.hh"
#include "TestRegexDelegate.hh"

#include <iostream>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TestRegexDelegate::TestRegexDelegate(
    DataStructModel *aDataStructModel,QObject *parent)
  : QStyledItemDelegate(parent),
    _DataStructModel(aDataStructModel)
{
#if 0
  for (size_t tIdx = 0; tIdx < aValues.size(); tIdx++)
  {
    Items.push_back(aValues.at(tIdx).c_str());
  }
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QWidget *TestRegexDelegate::createEditor(
    QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
  Enum *tEnum = _DataStructModel->getEnum(index);
  if (tEnum)
  {
    QComboBox *tEditor = new QComboBox(parent);
    tEditor->setEditable(true);
    tEditor->setInsertPolicy(QComboBox::InsertAtBottom);

    for(unsigned int i = 0; i < tEnum->_Values.size(); ++i)
    {
      tEditor->addItem(tEnum->_Values.at(i).c_str());
    }

    return tEditor;
  }
  else
  {
    return QStyledItemDelegate::createEditor(parent,option,index);
  }
#if 0
  QComboBox* editor = new QComboBox(parent);
  for(unsigned int i = 0; i < Items.size(); ++i)
    {
    editor->addItem(Items[i].c_str());
    }
  return editor;
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TestRegexDelegate::setEditorData(
    QWidget *editor, const QModelIndex &index) const
{
  Enum *tEnum = _DataStructModel->getEnum(index);
  if (tEnum)
  {
    QComboBox *tComboBox = static_cast<QComboBox*>(editor);
    int value = index.model()->data(index, Qt::EditRole).toUInt();
    tComboBox->setCurrentIndex(value);
  }
  else
  {
    QStyledItemDelegate::setEditorData(editor,index);
  }
#if 0
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  int value = index.model()->data(index, Qt::EditRole).toUInt();
  comboBox->setCurrentIndex(value);
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TestRegexDelegate::setModelData(
    QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  Enum *tEnum = _DataStructModel->getEnum(index);
  if (tEnum)
  {
    QComboBox *tComboBox = static_cast<QComboBox*>(editor);
    model->setData(index, tComboBox->currentText(), Qt::EditRole);
  }
  else
  {
    QStyledItemDelegate::setModelData(editor,model,index);
  }

#if 0
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->currentIndex(), Qt::EditRole);
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TestRegexDelegate::updateEditorGeometry(
    QWidget *editor,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
  Enum *tEnum = _DataStructModel->getEnum(index);
  if (tEnum)
  {
    editor->setGeometry(option.rect);
  }
  else
  {
    QStyledItemDelegate::updateEditorGeometry(editor,option,index);
  }
}

#if 0 // TODO
void TestRegexDelegate::paint(
    QPainter *painter,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const
{
  QStyleOptionViewItemV4 myOption = option;
  QString text = Items[index.row()].c_str();

  myOption.text = text;

  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}
#endif
