#include <iostream>
#include "FieldItem.hh"
#include "SimpleLineMatcher.hh"
#include "DataStructModel.hh"

ccl::Logger FieldItem::sLogger("FieldItem");

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItem::FieldItem(FieldItemData aData,FieldItem *aParentItem)
  : _ParentItem(aParentItem)
{
  _FieldItemData = aData;

  QList<QVariant> tList;

  _ItemData.append(QVariant(aData.getName().c_str()));
  _ItemData.append(QVariant(aData.getTest().c_str()));
  _ItemData.append(QVariant(aData.getTestScope().c_str()));
  _ItemData.append(QVariant(aData.getFormat()));
  _ItemData.append(QVariant(aData.getPostfix().c_str()));
  _ItemData.append(QVariant(aData.getKey().c_str()));
  _ItemData.append(QVariant(aData.getType().c_str()));
  _ItemData.append(QVariant(aData.getMatch().c_str()));
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
//-------------------------------------------------------------------------------
void FieldItem::setTestCheckState(Qt::CheckState aCheckState)
{
  _FieldItemData.setTestCheckState(aCheckState);
}

//-------------------------------------------------------------------------------
// TODO can probably remove - only test needs to be editable
//-------------------------------------------------------------------------------
void FieldItem::setFieldMatch(const QVariant aValue)
{
  _ItemData[DataStructModel::eColMatchRegex] = QVariant(QString(aValue.toString()));
  _FieldItemData.setMatch(aValue.toString().toStdString());
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::setFieldTest(const QVariant aValue)
{
  _ItemData[DataStructModel::eColTestRegex] = QVariant(QString(aValue.toString()));
  _FieldItemData.setTest(aValue.toString().toStdString());
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::setTestScope(const QVariant aValue)
{
  _ItemData[DataStructModel::eColTestScope] = QVariant(aValue);
  _FieldItemData.setTestScope(aValue.toString().toStdString());
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::setFieldFormat(const QVariant aValue)
{
  _ItemData[DataStructModel::eColFormat] = aValue;
  FieldItemData::Format tFormat =
      static_cast<FieldItemData::Format>(aValue.toInt());
  _FieldItemData.setFormat(tFormat);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void FieldItem::setFieldPostfix(const QVariant aValue)
{
  _ItemData[DataStructModel::eColPostfix] = aValue;
  _FieldItemData.setPostfix(aValue.toString().toStdString());
}
