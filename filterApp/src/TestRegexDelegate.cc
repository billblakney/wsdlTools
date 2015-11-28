#include "TestRegexDelegate.hh"

#include <QComboBox>
#include <QWidget>
#include <QModelIndex>
#include <QApplication>
#include <QString>

#include <iostream>

TestRegexDelegate::TestRegexDelegate(std::vector<std::string> aValues,QObject *parent)
  :QStyledItemDelegate(parent)
{
  for (size_t tIdx = 0; tIdx < aValues.size(); tIdx++)
  {
    Items.push_back(aValues.at(tIdx).c_str());
  }
}

QWidget *TestRegexDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
  QComboBox* editor = new QComboBox(parent);
  for(unsigned int i = 0; i < Items.size(); ++i)
    {
    editor->addItem(Items[i].c_str());
    }
  return editor;
}

void TestRegexDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  int value = index.model()->data(index, Qt::EditRole).toUInt();
  comboBox->setCurrentIndex(value);
}

void TestRegexDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *comboBox = static_cast<QComboBox*>(editor);
  model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}

void TestRegexDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
  editor->setGeometry(option.rect);
}

#if 0 // TODO
void TestRegexDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItemV4 myOption = option;
  QString text = Items[index.row()].c_str();

  myOption.text = text;

  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}
#endif
