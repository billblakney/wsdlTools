#ifndef FIELDITEM_HH_
#define FIELDITEM_HH_

#include <iterator>
#include <string>
#include <vector>
#include <QList>
#include <QVariant>
#include "FieldItemData.hh"
#include "Logger.hh"

/**
 * FieldItem represents a node in the data structure tree.
 */
class FieldItem
{
public:

  FieldItem(FieldItemData aData,FieldItem *aParentItem = 0);

  virtual ~FieldItem();

  //===========================================================================
  // tree related methods
  //===========================================================================

  void appendChild(FieldItem *item);

  FieldItem *child(int row);

  int childCount() const;

  int columnCount() const;

  QVariant data(int column) const;

  int row() const;

  FieldItem *parentItem() const;

  //===========================================================================
  // data related methods
  //===========================================================================

  FieldItemData &getData();

  void setCheckState(Qt::CheckState aCheckState);

  void setFieldMatch(const QVariant value);

  void setFieldTest(const QVariant value);

  void setTestScope(const QVariant value);

  void setFieldFormat(const QVariant value);

  void setFieldPostfix(const QVariant value);

protected:

  static ccl::Logger sLogger;

  FieldItem         *_ParentItem;
  QList<QVariant>    _ItemData;
  QList<FieldItem*>  _ChildItems;

  FieldItemData      _FieldItemData;
};

#endif /* FIELDITEM_HH_ */
