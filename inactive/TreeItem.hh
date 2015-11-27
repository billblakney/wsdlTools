#ifndef TREEITEM_HH_
#define TREEITEM_HH_

#include <iterator>
#include <string>
#include <vector>
#include <QList>
#include <QVariant>
#include "LineConsumer.hh"
#include "Logger.hh"

/**
 * TreeItem represents a node in the data structure tree.
 */
class TreeItem
{
public:

  TreeItem(const QList<QVariant> &aData,TreeItem *aParentItem = 0);

  virtual ~TreeItem();

  void appendChild(TreeItem *item);

  TreeItem *child(int row);

  int childCount() const;

  int columnCount() const;

  QVariant data(int column) const;

  int row() const;

  TreeItem *parentItem() const;

protected:

  static ccl::Logger sLogger;

  TreeItem         *_ParentItem;
  QList<QVariant>    _ItemData;
  QList<TreeItem*>  _ChildItems;
};

#endif /* TREEITEM_HH_ */
