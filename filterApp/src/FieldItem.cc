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
FieldItemData FieldItem::getData()
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

static SimpleLineMatcher tMatcher; //TODO

//-------------------------------------------------------------------------------
// Processes lines for a root node.
//-------------------------------------------------------------------------------
bool FieldItem::processRootLines(
    std::vector<std::string> &aLinesIn,
    std::vector<std::string>::iterator &aLineIter,
    std::vector<std::string> &aLinesOut)
{
  DEBUG(sLogger,"Proceeding with root");
  for (int tIdx = 0; tIdx < childCount(); tIdx++)
  {
    bool tResult = child(tIdx)->processLines(aLinesIn,aLineIter,aLinesOut);
    if (tResult == false)
    {
      ERROR(sLogger,"Processing child failed");
      return false;
    }
  }
  return true;
}

//-------------------------------------------------------------------------------
// Processes lines for a primitive field node.
//-------------------------------------------------------------------------------
bool FieldItem::processPrimitiveLines(
    std::vector<std::string> &aLinesIn,
    std::vector<std::string>::iterator &aLineIter,
    std::vector<std::string> &aLinesOut)
{
  /*
   * Going to test the single line that represents a primitive field.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *aLineIter;
  aLineIter++;

  tMatcher.setMatchRegex(getData().getMatch());

  if (tMatcher.match(tLine))
  {
    if (getData().getCheckState() == Qt::Checked)
    {
      aLinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"primitive node matched: " << getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"Primitive node <" << tLine << "> didn't match: <"
        << getData().getMatch() << ">");
    return false;
  }
  return true;
}

//-------------------------------------------------------------------------------
// Processes lines for a primitive array node.
//-------------------------------------------------------------------------------
bool FieldItem::processPrimitiveArrayLines(
    std::vector<std::string> &aLinesIn,
    std::vector<std::string>::iterator &aLineIter,
    std::vector<std::string> &aLinesOut)
{
  DEBUG(sLogger,"Looking for primitive array with match: "
      << getData().getMatch());

  /*
   * Going to test the line that indicates the start of the primitive array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *aLineIter;
  aLineIter++;

  tMatcher.setMatchRegex(getData().getMatch());
  if (tMatcher.match(tLine))
  {
    DEBUG(sLogger,"primitive array node matched: " << getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"primitive array node <" << tLine << "> didn't match: <"
        << getData().getMatch() << ">");
    return false;
  }

  /*
   * Going to test the line that specifies the number of elements in the
   * primitive array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *aLineIter;
  aLineIter++;

  // TODO better to use exact match of leading tabs in match regex?
  tMatcher.setMatchRegex(".*array of len: (\\d+)");
  if (tMatcher.match(tLine))
  {
    DEBUG(sLogger,"array of len matched: " << tLine);
  }
  else
  {
    ERROR(sLogger,"array of len didn't match: <" << tLine);
    return false;
  }

  int tArrayLen = atoi(tMatcher.getWhat().c_str());

  /*
   * Process the array elements that follow per the array length.
   */
  for (int tIdx = 0; tIdx < tArrayLen; tIdx++)
  {
    bool tResult = processPrimitiveArrayLine(aLinesIn,aLineIter,aLinesOut);
    if (tResult == false)
    {
      return false;
    }
  }

  return true;
}

//-------------------------------------------------------------------------------
// Processes a line for a primitive array element.
//-------------------------------------------------------------------------------
bool FieldItem::processPrimitiveArrayLine(
    std::vector<std::string> &aLinesIn,
    std::vector<std::string>::iterator &aLineIter,
    std::vector<std::string> &aLinesOut)
{
  /*
   * Going to test the single line that represents a primitive array item.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *aLineIter;
  aLineIter++;

  // TODO better to use exact match of leading tabs in match regex?
  std::string tPrimitiveArrayItemRegex(".*\\[\\d+\\]\\s+=\\s+(.*)");

  tMatcher.setMatchRegex(tPrimitiveArrayItemRegex);

  if (tMatcher.match(tLine))
  {
    if (getData().getCheckState() == Qt::Checked)
    {
      aLinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"primitive array element matched: " << getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"Primitive array element <" << tLine << "> didn't match: <"
        << tPrimitiveArrayItemRegex << ">");
    return false;
  }

  return true;
}

//-------------------------------------------------------------------------------
// Processes lines for a struct node.
//-------------------------------------------------------------------------------
bool FieldItem::processStructLines(
    std::vector<std::string> &aLinesIn,
    std::vector<std::string>::iterator &aLineIter,
    std::vector<std::string> &aLinesOut,
      bool aSkipStructName)
{
  DEBUG(sLogger,"Looking for struct with match: " << getData().getMatch());

  /*
   * The aSkipStructName tells us if this block of lines is started with a line
   * that specifies the struct name. If it is, get a reference to the line,
   * and increment the iterator. Then test for a match on the struct name line.
   */
  if (!aSkipStructName)
  {
    std::string &tLine = *aLineIter;
    aLineIter++;

    tMatcher.setMatchRegex(getData().getMatch());
    if (tMatcher.match(tLine))
    {
      DEBUG(sLogger,"struct node matched: " << getData().getMatch());
    }
    else
    {
      ERROR(sLogger,"struct node <" << tLine << "> didn't match: <"
          << getData().getMatch() << ">");
      return false;
    }
  }

  /*
   * We don't know the node types of the fields that comprise the struct,
   * so we'll be calling the higher-level processLines() method on each of the
   * child fields of the struct. That method will look at the node type of the
   * field and process it accordingly.
   */
  for (int tIdx = 0; tIdx < childCount(); tIdx++)
  {
    bool tResult = child(tIdx)->processLines(aLinesIn,aLineIter,aLinesOut);
    if (tResult == false)
    {
      ERROR(sLogger,"Processing child failed");
      return false;
    }
  }
  return true;
}

//-------------------------------------------------------------------------------
// Processes lines for an struct array node.
//-------------------------------------------------------------------------------
bool FieldItem::processStructArrayLines(
    std::vector<std::string> &aLinesIn,
    std::vector<std::string>::iterator &aLineIter,
    std::vector<std::string> &aLinesOut)
{
  DEBUG(sLogger,"Looking for struct array with match: " << getData().getMatch());

  /*
   * Going to test the line that indicates the start of the struct array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *aLineIter;
  aLineIter++;

  tMatcher.setMatchRegex(getData().getMatch());
  if (tMatcher.match(tLine))
  {
    DEBUG(sLogger,"struct array node matched: " << getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"struct array node <" << tLine << "> didn't match: <"
        << getData().getMatch() << ">");
    return false;
  }

  /*
   * Going to test the line that specifies the number of elements in the
   * struct array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *aLineIter;
  aLineIter++;

  tMatcher.setMatchRegex(".*array of len: (\\d+)"); //TODO exact match of tabs?
  if (tMatcher.match(tLine))
  {
    DEBUG(sLogger,"array of len matched: " << tLine);
  }
  else
  {
    ERROR(sLogger,"array of len didn't match: <" << tLine);
    return false;
  }

  int tArrayLen = atoi(tMatcher.getWhat().c_str());

  /*
   * Process the structures that follow per the array length.
   */
  for (int tIdx = 0; tIdx < tArrayLen; tIdx++)
  {
    bool tResult = processStructLines(aLinesIn,aLineIter,aLinesOut,true);
    if (tResult == false)
    {
      return false;
    }
  }

  return true;
}

//-------------------------------------------------------------------------------
// Processes lines for a field item and all of its children.
//-------------------------------------------------------------------------------
bool FieldItem::processLines(
    std::vector<std::string> &aLinesIn,
    std::vector<std::string>::iterator &aLineIter,
    std::vector<std::string> &aLinesOut)
{

  if (aLineIter == aLinesIn.end())
  {
    ERROR(sLogger,"Ran out of lines in FieldItem::processLines");
    return false;
  }

  if ( getData().getNodeType() == FieldItemData::eRoot )
  {
    DEBUG(sLogger,"<processLines> processing eRoot");
    processRootLines(aLinesIn,aLineIter,aLinesOut);
  }
  else if (getData().getNodeType() == FieldItemData::ePrimitive)
  {
    DEBUG(sLogger,"<processLines> processing ePrimitive");
    processPrimitiveLines(aLinesIn,aLineIter,aLinesOut);
  }
  else if (getData().getNodeType() == FieldItemData::ePrimitiveArray)
  {
    DEBUG(sLogger,"<processLines> processing ePrimitiveArray");
    processPrimitiveArrayLines(aLinesIn,aLineIter,aLinesOut);
  }
  else if ( getData().getNodeType() == FieldItemData::eStruct)
  {
    DEBUG(sLogger,"<processLines> processing eStruct");
    processStructLines(aLinesIn,aLineIter,aLinesOut);
  }
  else if (getData().getNodeType() == FieldItemData::eStructArray)
  {
    DEBUG(sLogger,"<processLines> processing eStructArray");
    processStructArrayLines(aLinesIn,aLineIter,aLinesOut);
  }
  else
  {
    ERROR(sLogger,"<processLines> unknown node type");
  }

  return true;
}
