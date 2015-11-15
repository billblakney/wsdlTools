#include <iostream>
#include "FieldItem.hh"
#include "SimpleLineMatcher.hh"

ccl::Logger FieldItem::sLogger("FieldItem");

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItem::FieldItem(FieldItemData aData,FieldItem *aParentItem)
  : _ParentItem(aParentItem)
{
  _FieldItemData = aData;

  QList<QVariant> tList;

  _ItemData.append(QVariant(aData.getName().c_str()));
  _ItemData.append(QVariant(aData.getKey().c_str()));
  _ItemData.append(QVariant(aData.getType().c_str()));
  _ItemData.append(QVariant(aData.getMatch().c_str()));
  _ItemData.append(QVariant(aData.getTest().c_str()));
  _ItemData.append(QVariant(QString("newline (\"\\n\")")));
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItem::~FieldItem()
{
  qDeleteAll(_ChildItems);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::appendChild(FieldItem *item)
{
  _ChildItems.append(item);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItem *FieldItem::child(int row)
{
  return _ChildItems.value(row);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int FieldItem::childCount() const
{
  return _ChildItems.count();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int FieldItem::row() const
{
  if( _ParentItem )
  {
    return _ParentItem->_ChildItems.indexOf(const_cast<FieldItem *>(this));
  }
  return 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
int FieldItem::columnCount() const
{
  return _ItemData.count();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
QVariant FieldItem::data(int column) const
{
  return _ItemData.value(column);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItem *FieldItem::parentItem() const
{
  return _ParentItem;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData &FieldItem::getData()
{
  return _FieldItemData;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::setCheckState(Qt::CheckState aCheckState)
{
  _FieldItemData.setCheckState(aCheckState);
}

//-------------------------------------------------------------------------------
// TODO can probably remove - only test needs to be editable
//-------------------------------------------------------------------------------
void FieldItem::setFieldMatch(const QVariant &aValue)
{
  _ItemData[eMatchCol] = QVariant(QString(aValue.toString()));
  _FieldItemData.setMatch(aValue.toString().toStdString());
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::setFieldTest(const QVariant &aValue)
{
  _ItemData[eTestCol] = QVariant(QString(aValue.toString()));
  _FieldItemData.setTest(aValue.toString().toStdString());
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::setFieldPostfix(const QVariant &aValue)
{
  _ItemData[ePostfixCol] = aValue;
  _FieldItemData.setPostfix(aValue.toString().toStdString());
}
