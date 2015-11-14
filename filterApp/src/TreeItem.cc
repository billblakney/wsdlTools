#include <iostream>
#include "TreeItem.hh"

ccl::Logger TreeItem::sLogger("TreeItem");

TreeItem::TreeItem(const QList<QVariant> &aData,TreeItem *aParentItem)
  : _ParentItem(aParentItem),
    _ItemData(aData)
{
}

TreeItem::~TreeItem()
{
  qDeleteAll(_ChildItems);
}

void TreeItem::appendChild(TreeItem *item)
{
  _ChildItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
  return _ChildItems.value(row);
}

int TreeItem::childCount() const
{
  return _ChildItems.count();
}

int TreeItem::row() const
{
  if( _ParentItem )
  {
    return _ParentItem->_ChildItems.indexOf(const_cast<TreeItem *>(this));
  }
  return 0;
}

int TreeItem::columnCount() const
{
  return _ItemData.count();
}

QVariant TreeItem::data(int column) const
{
  return _ItemData.value(column);
}

TreeItem *TreeItem::parentItem() const
{
  return _ParentItem;
}
