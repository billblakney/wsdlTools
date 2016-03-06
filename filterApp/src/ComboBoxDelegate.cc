#include "ComboBoxDelegate.hh"

#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>

#include <iostream>

//-----------------------------------------------------------------------------
// Construct combo-box delegate with a vector of std::string.
//-----------------------------------------------------------------------------
ComboBoxDelegate::ComboBoxDelegate(
    std::vector<std::string> aValues,QObject *parent)
  :QStyledItemDelegate(parent)
{
  for (size_t tIdx = 0; tIdx < aValues.size(); tIdx++)
  {
    _Items.push_back(aValues.at(tIdx).c_str());
  }
}

//-----------------------------------------------------------------------------
// Construct combo-box delegate with a QStringList.
//-----------------------------------------------------------------------------
ComboBoxDelegate::ComboBoxDelegate(QStringList aStringList,QObject *parent)
  :QStyledItemDelegate(parent)
{
  for (int i = 0; i < aStringList.size(); i++)
  {
    _Items.push_back(aStringList.at(i));
  }
}

//-----------------------------------------------------------------------------
// Get the combo-box editor.
// TODO memory leak. create first time and reuse
//-----------------------------------------------------------------------------
QWidget *ComboBoxDelegate::createEditor(
    QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
  QComboBox* editor = new QComboBox(parent);
  for(int i = 0; i < _Items.size(); ++i)
  {
    editor->addItem(_Items[i]);
  }
  return editor;
}

//-----------------------------------------------------------------------------
// Set the editor data per the specified QModelIndex.
//-----------------------------------------------------------------------------
void ComboBoxDelegate::setEditorData(
    QWidget *editor, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  int value = index.model()->data(index, Qt::EditRole).toUInt();
  comboBox->setCurrentIndex(value);
}

//-----------------------------------------------------------------------------
// Set the model data with the current selection.
//-----------------------------------------------------------------------------
void ComboBoxDelegate::setModelData(
    QWidget *editor,QAbstractItemModel *model,const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ComboBoxDelegate::updateEditorGeometry(
    QWidget *editor,
    const QStyleOptionViewItem &option,
    const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}

#if 0 // TODO any use for this?
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ComboBoxDelegate::paint(
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
