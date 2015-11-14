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

  enum Column {eNameCol, eTypeCol, eMatchCol, eTestCol, ePostfixCol};

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

  FieldItemData getData();

  void setCheckState(Qt::CheckState aCheckState);

  void setFieldMatch(const QVariant &value);

  void setFieldTest(const QVariant &value);

  void setFieldPostfix(const QVariant &value);

  bool processRootLines(
      std::vector<std::string> &aLinesIn,
      std::vector<std::string>::iterator &aLineIter,
      std::vector<std::string> &aLinesOut);

  bool processPrimitiveLines(
      std::vector<std::string> &aLinesIn,
      std::vector<std::string>::iterator &aLineIter,
      std::vector<std::string> &aLinesOut);

  bool processPrimitiveArrayLines(
      std::vector<std::string> &aLinesIn,
      std::vector<std::string>::iterator &aLineIter,
      std::vector<std::string> &aLinesOut);

  bool processPrimitiveArrayLine(
      std::vector<std::string> &aLinesIn,
      std::vector<std::string>::iterator &aLineIter,
      std::vector<std::string> &aLinesOut);

  bool processStructLines(
      std::vector<std::string> &aLinesIn,
      std::vector<std::string>::iterator &aLineIter,
      std::vector<std::string> &aLinesOut,
      bool aSkipStructName = false);

  bool processStructArrayLines(
      std::vector<std::string> &aLinesIn,
      std::vector<std::string>::iterator &aLineIter,
      std::vector<std::string> &aLinesOut);

  bool processLines(
      std::vector<std::string> &aLinesIn,
      std::vector<std::string>::iterator &aLineIter,
      std::vector<std::string> &aLinesOut);

protected:

  static ccl::Logger sLogger;

  FieldItem         *_ParentItem;
  QList<QVariant>    _ItemData;
  QList<FieldItem*>  _ChildItems;

  FieldItemData      _FieldItemData;

};

#endif /* FIELDITEM_HH_ */
