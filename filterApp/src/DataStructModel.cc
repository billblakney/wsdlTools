#include <iostream>
#include <iterator>
#include <sstream>
#include <boost/regex.hpp>
#include <QVariant>
#include "DataStructModel.hh"

ccl::Logger DataStructModel::sLogger("DataStructModel");

QFont DataStructModel::kArrayFont;

//------------------------------------------------------------------------------
// Construct all of the FieldItems that comprise the tree.
//------------------------------------------------------------------------------
DataStructModel::DataStructModel(
    Structure *aStruct,StructorBuilder *aStructBuilder)
  : _StructBuilder(aStructBuilder),
    _RootItem(0),
    _TopNodeItem(0),
    _PropogateFieldChecks(false)
{
  kArrayFont.setItalic(true);

  setupPostfixes();

  // root item
  FieldItemData tRootData(FieldItemData::eRoot,0,"root",aStruct->_Name,
      aStruct->_Name);
  _RootItem = new FieldItem(tRootData);

  // top node item
  FieldItemData tTopData(FieldItemData::eRoot,0,"","root",aStruct->_Name);
  _TopNodeItem = new FieldItem(tTopData,_RootItem);

  _RootItem->appendChild(_TopNodeItem);

  buildTree(_TopNodeItem,aStruct);

  appendToTreeItemVector(_TopNodeItem);

// Was going to be used at one time, but no longer. Can be removed.
//  if (childCount(_TopNodeItem,FieldItemData::eStructArray) == 1)
//  {
//    std::cout << "TopNode HAS ONE STRUCT_ARRAY child" << std::endl;
//  }

  setFilterScopeForDesignator();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
DataStructModel::~DataStructModel()
{
}

//------------------------------------------------------------------------------
// Applies special processing to filter scopes for contact designators.
//
// For many data structures that contain a contact designator field, i.e. field
// of type CLIR_CAR_DESIGNATOR, if the contact designator is the child of a
// struct array node, it is usually more common to want to use the filter scope
// corresponding to the struct array node element, than it is to use the default
// filter scope of "root". This method automatically makes such settings. In
// particular, the algorithm works as follows:
// For any contact designator field, of type CLIR_CAR_DESIGNATOR, look for an
// ancestor that is a struct array node. If one is found, then apply the test
// scope that applies to the struct array node element to the entire tree of
// that ancestor node.
//------------------------------------------------------------------------------
void DataStructModel::setFilterScopeForDesignator()
{
  std::vector<FieldItem *>::iterator tIter;
  for (tIter = _TreeItems.begin(); tIter != _TreeItems.end(); tIter++)
  {
    FieldItem *tNode = *tIter;
    if (tNode->getData().getType().compare(("CLIR_CAR_DESIGNATOR_T")) == 0)
    {
      FieldItem *tParent = findFirstAncestor(tNode,FieldItemData::eStructArray);
      if (tParent != NULL)
      {
        std::string tTestScope = tParent->getData().getKey() + ".Element";
        applyTestScope(tTestScope.c_str(),false,tParent);
      }
      else
      {
      }
    }
  }
}

//------------------------------------------------------------------------------
// Slot to apply a test scope to the items of the tree. The test scope can be
// applied to all items, or only to checked items.
//------------------------------------------------------------------------------
void DataStructModel::onApplyTestScope(QString aTestScope,bool aCheckedOnly)
{
  QVariant tScope(aTestScope);
  applyTestScope(tScope,aCheckedOnly,_TopNodeItem);
}

//------------------------------------------------------------------------------
// Apply a test scope to a branch of the tree. The test scope can be applied to
// all items, or only to checked items.
//------------------------------------------------------------------------------
void DataStructModel::applyTestScope(QVariant aTestScope,
    bool aCheckedOnly,FieldItem *aFieldItem)
{
  if ((aCheckedOnly==false) || aFieldItem->getData().isChecked())
  {
    aFieldItem->setTestScope(aTestScope);
  }

  for (int tIdx = 0; tIdx < aFieldItem->childCount(); tIdx++)
  {
    applyTestScope(aTestScope,aCheckedOnly,aFieldItem->child(tIdx));
  }
  emit modelUpdated();
}

//------------------------------------------------------------------------------
// For a tree node, find the first ancestor that matches a specified node type.
// Returns a pointer to that ancestor node, or NULL if none is found.
//------------------------------------------------------------------------------
FieldItem *DataStructModel::findFirstAncestor(
    FieldItem *aNode,FieldItemData::NodeType aNodeType)
{
  FieldItem *tReturn = NULL;

  FieldItem *tParent;
  while ((tParent = aNode->parentItem()) != NULL)
  {
    if (tParent->getData().getNodeType() == aNodeType)
    {
      tReturn = tParent;
      break;
    }
    aNode = tParent; // advance to next ancestor
  }

  return tReturn;
}

//------------------------------------------------------------------------------
// For a tree node, get the count of its children that match a specified node
// type.
//------------------------------------------------------------------------------
int DataStructModel::childCount(FieldItem *aNode,FieldItemData::NodeType aType)
{
  int tCount = 0;

  if (aType == FieldItemData::eNone)
  {
    tCount = aNode->childCount();
  }
  else
  {
    for (int i = 0; i < aNode->childCount(); i++)
    {
      if (aNode->child(i)->getData().getNodeType() == aType)
      {
        tCount++;
      }
    }
  }

  return tCount;
}

//------------------------------------------------------------------------------
// Populates _TreeItems.
// This method is meant to be called with the top node so that it can
// recursively populate _TreeItems with all nodes of the tree.
//------------------------------------------------------------------------------
void DataStructModel::appendToTreeItemVector(FieldItem *aNode)
{
  for (int i = 0; i < aNode->childCount(); i++)
  {
    FieldItem *tChild = aNode->child(i);
    _TreeItems.push_back(tChild);
    appendToTreeItemVector(tChild);
  }
}

//------------------------------------------------------------------------------
// Get the list of all tree nodes.
//------------------------------------------------------------------------------
std::vector<FieldItem *> &DataStructModel::getTreeItems()
{
  return _TreeItems;
}

//------------------------------------------------------------------------------
// Reset the tree items to their default values, e.g. unchecked, default format,
// etc.)
//------------------------------------------------------------------------------
void DataStructModel::resetTreeItems()
{
  std::vector<FieldItem *>::iterator tIter;

  /*
   * Get the default values for the columns.
   */
  std::string tDefaultTestScope = FieldItemData::getDefaultTestScope();
  FieldItemData::Format tDefaultFormat = FieldItemData::getDefaultFormat();
  std::string tDefaultPostfix = FieldItemData::getDefaultPostfix();

  /*
   * Apply the default values to the root node.
   * TODO needed?
   */
  _RootItem->setCheckState(Qt::Unchecked);
  _RootItem->setTestScope(QVariant(tDefaultTestScope.c_str()));
  _RootItem->setFieldFormat(QVariant(tDefaultFormat));
  _RootItem->setFieldPostfix(QVariant(tDefaultPostfix.c_str()));

  /*
   * Apply the default values to the other nodes.
   */
  for (tIter = _TreeItems.begin(); tIter != _TreeItems.end(); tIter++)
  {
    FieldItem *aNode = *tIter;
    aNode->setCheckState(Qt::Unchecked);
    aNode->setTestScope(QVariant(tDefaultTestScope.c_str()));
    aNode->setFieldFormat(QVariant(tDefaultFormat));
    aNode->setFieldPostfix(QVariant(tDefaultPostfix.c_str()));
  }

  /*
   * Apply the rule for setting test scope per contact designator.
   */
  setFilterScopeForDesignator();

  /*
   * Emit the modelUpdate signal to repaint the tree.
   */
  emit modelUpdated();
}

//------------------------------------------------------------------------------
// Get the top node of the tree.
//------------------------------------------------------------------------------
FieldItem *DataStructModel::getTopNode()
{
  return _TopNodeItem;
}

//------------------------------------------------------------------------------
// Get the tree node for a specific key.
//------------------------------------------------------------------------------
FieldItem *DataStructModel::getNode(std::string aKey)
{
  std::vector<FieldItem *>::iterator tIter;
  for (tIter = _TreeItems.begin(); tIter != _TreeItems.end(); tIter++)
  {
    if (!aKey.compare((*tIter)->getData().getKey()))
    {
      return *tIter;
    }
  }
  return NULL;
}

//------------------------------------------------------------------------------
// Get the list of test scopes for the tree.
//------------------------------------------------------------------------------
std::vector<std::string> DataStructModel::getTestScopes()
{
  return _TestScopes;
}

//------------------------------------------------------------------------------
// Get the list of postfixes.
//------------------------------------------------------------------------------
std::vector<std::string> DataStructModel::getPostfixes()
{
  return _Postfixes;
}

//------------------------------------------------------------------------------
// Gets the enumeration type (from the struct builder) for a a primitive value
// node in the tree.
//------------------------------------------------------------------------------
Enum *DataStructModel::getEnum(const QModelIndex &aIndex) const
{
  Enum *tEnum = NULL;

  FieldItem *tItem = static_cast<FieldItem*>(aIndex.internalPointer());
  if (tItem->getData().getNodeType() == FieldItemData::ePrimitiveValue)
  {
    std::string tType = tItem->getData().getType();
    tEnum = _StructBuilder->getEnum(tType);
  }

  return tEnum;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataStructModel::printTestNodes()
{
  std::cout << "Test Nodes:" << std::endl;
  std::vector<std::string>::iterator tIter;
  for (tIter = _TestScopes.begin(); tIter != _TestScopes.end(); tIter++)
  {
    std::cout << "   " << *tIter << std::endl;
  }
}

//TODO deep enough? replace generically?
static std::string blanks[] = {
    "",
    "  ",
    "    ",
    "      ",
    "        ",
    "          ",
    "            ",
    "              ",
    "                ",
    "                  ",
};

//------------------------------------------------------------------------------
// Gets the match for the first field in the data structure. This will be used
// by the reader to identify the start of the structure (since in wsdl there is
// no other identifier,like the name of the structure, e.g.).
//------------------------------------------------------------------------------
std::string DataStructModel::getFirstFieldMatch()
{
  return _TopNodeItem->child(0)->getData().getMatch();
}

static Field _lastLengthField; //TODO

//------------------------------------------------------------------------------
// Completes building a tree if field items after the root and top items have
// been created. Is initially called with the top item, and then is called
// recursively.
//------------------------------------------------------------------------------
void DataStructModel::buildTree(
    FieldItem *aParentItem,Structure *aStruct,int aLevel)
{
  boost::regex array_size_regex("^([_a-zA-Z]+)_size$");

  if (aLevel == 0)
  {
      // nothing special to do at level zero
    _TestScopes.push_back("root");
  }

  /*
   * Build a node for each child of aStruct.
   */
  vector<Field>::iterator tIter;
  for (tIter = aStruct->_Fields.begin();
       tIter != aStruct->_Fields.end(); tIter++)
  {
    boost::match_results<std::string::const_iterator> what;

    // TODO this if block needed?
    // For array size line,
    // print the array type and name.
    if (boost::regex_match(tIter->_Name,what,array_size_regex))
    {
      //std::string tName = what[1];
      DEBUG(sLogger,blanks[aLevel] << "array size entry: "
          << tIter->_Type << ":" << tIter->_Name);

      /*
       * The array length field occurs before the actual array field.
       * So save it off, so that the array node can create a child node
       * for it.
       */
      if (aParentItem->getData().getNodeType() == FieldItemData::eStructArray)
      {
        _lastLengthField = *tIter;
      }
      else if (aParentItem->getData().getNodeType() == FieldItemData::ePrimitiveArray)
      {
        _lastLengthField = *tIter;
      }
    }
    else if (tIter->_IsPointer)
    {
      if (_StructBuilder->isPrimitive(tIter->_Type))
      {
        buildPrimitiveArrayNode(*tIter,aParentItem,aLevel);
      }
      else
      {
        buildStructArrayNode(*tIter,aParentItem,aLevel);
      }
    }
    else if (!_StructBuilder->isPrimitive(tIter->_Type))
    {
      buildStructNode(*tIter,aParentItem,aLevel);
    }
    else
    {
      buildPrimitiveNode(*tIter,aParentItem,aLevel);
    }
  }
}

//------------------------------------------------------------------------------
// Build a tree node for a struct field.
//------------------------------------------------------------------------------
void DataStructModel::buildStructNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "struct node: "
      << aField._Type << ":" << aField._Name);

  Structure *tStruct = _StructBuilder->getStructure(aField._Type);

  if (tStruct == NULL)
  {
    ERROR(sLogger,blanks[aLevel] << "Can't find struct " << aField._Name);
    return;
  }

  std::string tKey = buildKey(aField,aParentItem,false);
  std::string tMatch = buildMatchForStructField(aField,aLevel);
  FieldItemData tData(FieldItemData::eStruct,aLevel,tKey,
      aField._Name,aField._Type,tMatch);
  FieldItem *dataItem = new FieldItem(tData,aParentItem);
  aParentItem->appendChild(dataItem);

  //
  // Set the test node.
  //
  _TestScopes.push_back(tData.getKey());

  buildTree(dataItem,tStruct,++aLevel);
  --aLevel;
}

//------------------------------------------------------------------------------
// Build a struct array node for a struct field.
//------------------------------------------------------------------------------
void DataStructModel::buildStructArrayNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "struct array node: "
      << aField._Type << ":" << aField._Name);

  Structure *tStruct = _StructBuilder->getStructure(aField._Type);
  if (tStruct == NULL)
  {
    ERROR(sLogger,blanks[aLevel]
        << "Can't find struct array node " << aField._Name);
    return;
  }

  std::string tKey = buildKey(aField,aParentItem,true);
  std::string tMatch = buildMatchForStructArrayField(aField,aLevel);

  FieldItemData tData(FieldItemData::eStructArray,aLevel,tKey,
      aField._Name,aField._Type,tMatch);

  FieldItem *dataItem = new FieldItem(tData,aParentItem);

  aParentItem->appendChild(dataItem);

  //
  // Set the test nodes - one test node for specifying the whole array, and one
  // for specifying individual array elements.
  //
  _TestScopes.push_back(tData.getKey());
  _TestScopes.push_back(tData.getKey() + ".Element");

  /*
   * Build the array length node, which will be a child to this node.
   */
  int tNextLevel = aLevel + 1;
  buildStructArrayLengthNode(_lastLengthField,dataItem,tNextLevel);

  buildTree(dataItem,tStruct,++aLevel);
  --aLevel;
}

//------------------------------------------------------------------------------
// Build a struct array length node for a struct field.
//------------------------------------------------------------------------------
void DataStructModel::buildStructArrayLengthNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "struct array length node: "
      << aField._Type << ":" << aField._Name);

  std::string tKey = buildArrayLengthKey(aParentItem);
  std::string tMatch = buildMatchForArrayLengthField(aField,aLevel);

  FieldItemData tData(FieldItemData::eStructArrayLength,aLevel,tKey,
      "array_size","-",tMatch);

  FieldItem *dataItem = new FieldItem(tData,aParentItem);
  aParentItem->appendChild(dataItem);
}

//------------------------------------------------------------------------------
// Build a primitive array node for a struct field.
//------------------------------------------------------------------------------
void DataStructModel::buildPrimitiveArrayNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "primitive array node: "
      << aField._Type << ":" << aField._Name);

  std::string tKey = buildKey(aField,aParentItem,true);
  std::string tMatch = buildMatchForPrimitiveArrayField(aField,aLevel);

  FieldItemData tData(FieldItemData::ePrimitiveArray,aLevel,tKey,
      aField._Name,aField._Type,tMatch);

  FieldItem *dataItem = new FieldItem(tData,aParentItem);
  aParentItem->appendChild(dataItem);

  //
  // Set the test nodes - one test node for specifying the whole array, and one
  // for specifying individual array elements.
  //
  _TestScopes.push_back(tData.getKey());
  _TestScopes.push_back(tData.getKey() + ".Element");

  /*
   * Build the array length node, which will be a child to this node.
   */
  int tNextLevel = aLevel + 1;
  buildPrimitiveArrayLengthNode(_lastLengthField,dataItem,tNextLevel);
}

//------------------------------------------------------------------------------
// Build a primitive array length node for a struct field.
// Design note: This is basically the same as the buildStructArrayLengthNode()
// method. A single method with an additional FieldItemData::NodeType parameter
// could be abstracted and used in both cases.
//------------------------------------------------------------------------------
void DataStructModel::buildPrimitiveArrayLengthNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "struct array length node: "
      << aField._Type << ":" << aField._Name);

  std::string tKey = buildArrayLengthKey(aParentItem);
  std::string tMatch = buildMatchForArrayLengthField(aField,aLevel);

  FieldItemData tData(FieldItemData::ePrimitiveArrayLength,aLevel,tKey,
      "array_size","-",tMatch);

  FieldItem *dataItem = new FieldItem(tData,aParentItem);
  aParentItem->appendChild(dataItem);
}

//------------------------------------------------------------------------------
// Build a tree node for a primitive field.
//------------------------------------------------------------------------------
void DataStructModel::buildPrimitiveNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "primitive node: "
      << aField._Type << ":" << aField._Name);

  std::string tKey = buildKey(aField,aParentItem,false);
  std::string tMatch = buildMatchForPrimitiveField(aField,aLevel);

  FieldItemData tData(FieldItemData::ePrimitiveValue,aLevel,tKey,
      aField._Name,aField._Type,tMatch);

  FieldItem *dataItem = new FieldItem(tData,aParentItem);

  aParentItem->appendChild(dataItem);
}

//------------------------------------------------------------------------------
// Build a primitive array node for a struct field.
//------------------------------------------------------------------------------
std::string DataStructModel::buildKey(
    Field &aField,FieldItem *aParentItem,bool /*aIsArrayType*/)
{
  std::string tKey;

  FieldItemData &tParentData = aParentItem->getData();

  if (tParentData.getKey().length() > 0)
  {
    tKey = tParentData.getKey() + "." + aField._Name;
  }
  else
  {
    tKey = aField._Name;
  }

#if 0 //TODO ok remove if not used at some point
  if (aIsArrayType)
  {
  }
#endif

  return tKey;
}

//------------------------------------------------------------------------------
// Build a primitive array node for a struct field.
//------------------------------------------------------------------------------
std::string DataStructModel::buildArrayLengthKey(FieldItem *aParentItem)
{
  std::string tKey;

  FieldItemData &tParentData = aParentItem->getData();

  if (tParentData.getKey().length() > 0)
  {
    tKey = tParentData.getKey() + "_size";
  }
  else
  {
    // ERROR case
  }

  return tKey;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForField(
    const Field &aField,int aIndentLevel)
{
  char tBuffer[50];
  sprintf(tBuffer,"^[\\t]{%d}%s:",aIndentLevel,aField._Name.c_str());
  return tBuffer;
}

//------------------------------------------------------------------------------
// TODO check spaces vs. tabs
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForArrayLengthField(
    const Field &/*aField*/,int aIndentLevel)
{
  char tBuffer[50];
  sprintf(tBuffer,"^[\\t]{%d}array of len:[\\s]+(.*)",aIndentLevel);
  return tBuffer;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForStructField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForField(aField,aIndentLevel);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForStructArrayField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForField(aField,aIndentLevel);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForStructArrayLengthField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForArrayLengthField(aField,aIndentLevel);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForPrimitiveField(
    const Field &aField,int aIndentLevel)
{
  static char tBuffer[200];
  sprintf(tBuffer,"^[\\t]{%d}%s:[\\s]+(.*)",aIndentLevel,aField._Name.c_str());
  return tBuffer;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForPrimitiveArrayField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForField(aField,aIndentLevel);
}

//------------------------------------------------------------------------------
//TODO
//------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForPrimitiveArrayLengthField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForField(aField,aIndentLevel);
}

//------------------------------------------------------------------------------
// Not sure this will be used or what it was originally for.
//------------------------------------------------------------------------------
std::string DataStructModel::getMatchString(FieldItem *aFieldItem)
{
  std::string tString;

  tString += aFieldItem->getData().getType() + ","
           + aFieldItem->getData().getMatch() + "\n";

  for (int i = 0; i < aFieldItem->childCount(); i++)
  {
    tString += getMatchString(aFieldItem->child(i));
  }

  return tString;
}

//------------------------------------------------------------------------------
// Gets the match string for the simple reader.
//------------------------------------------------------------------------------
std::string DataStructModel::getMatchString()
{
  std::string tMatchString(".*");

  std::vector<FieldItem *> tCheckedFieldItems = getCheckedFields();
  int tNumCheckFields = tCheckedFieldItems.size();
  if (tNumCheckFields > 0)
  {
    tMatchString.clear();
    bool tFirstMatchField = true;
    for (int tIdx = 0; tIdx < tNumCheckFields; tIdx++)
    {
      std::string tFieldMatch = tCheckedFieldItems[tIdx]->getData().getMatch();
      if (tFieldMatch.length() == 0)
      {
        continue;
      }

      if (tFirstMatchField == true)
      {
        tFirstMatchField = false;
      }
      else
      {
        tMatchString += "|";
      }
      tMatchString += tFieldMatch;
    }
  }
  DEBUG(sLogger,"Match string: " << tMatchString);
  return tMatchString;
}

//------------------------------------------------------------------------------
// Get the list of tree nodes that are checked for the entire tree.
//------------------------------------------------------------------------------
std::vector<FieldItem *> DataStructModel::getCheckedFields()
{
  std::vector<FieldItem *> tFieldItems;
  addCheckedFields(_TopNodeItem,tFieldItems);
  return tFieldItems;
}

//------------------------------------------------------------------------------
// Get the list of tree nodes that are checked for a branch of the tree and add
// it to a list of tree nodes. This is a recursive method, and is used by the
// getCheckedFields method to get the list of checked fields for the entire tree.
//------------------------------------------------------------------------------
void DataStructModel::addCheckedFields(
    FieldItem *aNode,std::vector<FieldItem *> &aFieldItems)
{
  if (aNode->getData().getCheckState() == Qt::Checked)
  {
    aFieldItems.push_back(aNode);
  }
  for (int tIdx = 0; tIdx < aNode->childCount(); tIdx++)
  {
    addCheckedFields(aNode->child(tIdx),aFieldItems);
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QVariant DataStructModel::data(const QModelIndex &index,int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }

  FieldItem *item = static_cast<FieldItem*>(index.internalPointer());

  switch (role) {
    case Qt::DisplayRole:
    {
      if (index.column() == eColFormat)
      {
        FieldItemData::Format tFormatEnum =
            static_cast<FieldItemData::Format>(
                item->data(eColFormat).toInt());
        return FieldItemData::getFormatString(tFormatEnum);
      }
      return item->data(index.column());
      break;
    }
    case Qt::EditRole:
    {
      Enum *tEnum = NULL;
      if (index.column() == eColTestScope)
      {
        std::string tTestScope = item->data(eColTestScope).toString().toStdString();
        uint tTestNodeIndex = getTestScopeIndex(tTestScope);

        return tTestNodeIndex;
      }
      else if (index.column() == eColPostfix)
      {
        std::string tPostfix = item->data(eColPostfix).toString().toStdString();
        uint tPostfixIndex = getPostfixIndex(tPostfix);

        return tPostfixIndex;
      }
      else if (index.column() == eColTestRegex && (tEnum = getEnum(index)))
      {
        uint tTestRegexIndex = getEnumIndex(tEnum,item->getData().getTest());
        return tTestRegexIndex;
      }
      return item->data(index.column());
      break;
    } case Qt::CheckStateRole:
      if (index.column() == eColFieldName)
      {
        return static_cast<int>(item->getData().getCheckState());
      }
      else if (index.column() == eColTestRegex)
      {
        return static_cast<int>(item->getData().getTestCheckState());
      }
      break;
    case Qt::FontRole:
      if( (index.column() == eColFieldName) &&
       (item->getData().getNodeType() == FieldItemData::eStructArray ||
        item->getData().getNodeType() == FieldItemData::eStructArrayLength ||
        item->getData().getNodeType() == FieldItemData::ePrimitiveArray ||
        item->getData().getNodeType() == FieldItemData::ePrimitiveArrayLength))
      {
        return kArrayFont;
      }
      break;
  }

  return QVariant();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool DataStructModel::setData(
    const QModelIndex &index,const QVariant &value,int role)
{
//  int aRow = index.row();
  int aCol = index.column();

  if( role == Qt::CheckStateRole &&
      (aCol == eColFieldName ||
      aCol == eColTestRegex))
  {
    FieldItem *item = static_cast<FieldItem*>(index.internalPointer());

    Qt::CheckState tNewState = Qt::PartiallyChecked;

    if (value == Qt::Checked)
    {
      tNewState = Qt::Checked;
    }
    else if (value == Qt::Unchecked)
    {
      tNewState = Qt::Unchecked;
    }

    if (aCol == eColFieldName)
    {
      item->setCheckState(tNewState);
      emit dataChanged(index,index);

      if ((_PropogateFieldChecks || item == _TopNodeItem)
          && item->childCount() > 0)
      {
        setChildrenCheckStates(index,tNewState);
      }

#ifdef UPDATE_PARENT_CHECK_STATES
      updateParentCheckState(index,tNewState);
#endif
    }
    else if (aCol == eColTestRegex)
    {
      item->setTestCheckState(tNewState);
      emit dataChanged(index,index);
    }
  }
  else if (role == Qt::EditRole)
  {
    FieldItem *item = static_cast<FieldItem*>(index.internalPointer());
    if (aCol ==  eColMatchRegex)
    {
      item->setFieldMatch(value);
      emit dataChanged(index,index);
    }
    else if (aCol == eColTestRegex)
    {
      std::string tValue = value.toString().toStdString();
      transformTestRegex(tValue);
      item->setFieldTest(tValue.c_str());
      emit dataChanged(index,index);
    }
    else if (aCol == eColTestScope)
    {
      std::string tStr = _TestScopes.at(value.toUInt());
      item->setTestScope(QVariant(tStr.c_str()));
      emit dataChanged(index,index);
    }
    else if (aCol == eColFormat)
    {
      item->setFieldFormat(value);
      emit dataChanged(index,index);
    }
    else if (aCol == eColPostfix)
    {
      std::string tStr = _Postfixes.at(value.toUInt());
      item->setFieldPostfix(QVariant(tStr.c_str()));
      emit dataChanged(index,index);
    }
  }
  return true;
}

//------------------------------------------------------------------------------
// Replace any '*' not preceeded by '.', with a ".*".
// This prevents a crash on some invalid regex expressions, and also
// provides the convenience of setting a search string like "*something*".
// Since we never expect a literal * to be part of a field value, this should
// be ok.
//------------------------------------------------------------------------------
void DataStructModel::transformTestRegex(std::string &aValue)
{
  /*
   * Search and replace leading '*'.
   */
  boost::regex matchFirst("^\\*");
  std::string tFirstFormat(".*");
  aValue = boost::regex_replace(aValue,matchFirst,tFirstFormat,
      boost::format_first_only | boost::format_all);

  /*
   * Search and replace other '*' not preceeded by a '.'.
   */
  boost::regex matchTheRest("([^\\.])\\*");
  std::string tTheRestFormat("$1.*");
  aValue = boost::regex_replace(aValue,matchTheRest,tTheRestFormat,
      //          boost::format_first_only | boost::format_all);
      boost::format_all);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Qt::ItemFlags DataStructModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags tFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (index.column() == eColFieldName ||
      index.column() == eColTestRegex)
  {
    tFlags |= Qt::ItemIsUserCheckable;
  }

  if (index.column() == eColMatchRegex ||
      index.column() == eColTestRegex ||
      index.column() == eColTestScope ||
      index.column() == eColFormat ||
      index.column() == eColPostfix)
  {
    tFlags |= Qt::ItemIsEditable;
  }

  return tFlags;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QVariant DataStructModel::headerData(int section,Qt::Orientation orientation,
      int role) const
{
  switch (role)
  {
  case Qt::DisplayRole:
    if (orientation == Qt::Horizontal)
    {
      if (section == eColFieldName)
      {
        return QVariant("Field Name");
      }
      else if (section == eColFieldKey)
      {
        return QVariant("Field Key");
      }
      else if (section == eColFieldType)
      {
        return QVariant("Field Type");
      }
      else if (section == eColMatchRegex)
      {
        return QVariant("Test Regex");
      }
      else if (section == eColTestRegex)
      {
        return QVariant("Test Filter");
      }
      else if (section == eColTestScope)
      {
        return QVariant("Filter Subtree");
      }
      else if (section == eColFormat)
      {
        return QVariant("Format");
      }
      else if (section == eColPostfix)
      {
        return QVariant("Postfix");
      }
      break;
  case Qt::ToolTipRole:
    if (orientation == Qt::Horizontal)
    {
      QString tTitle("");
      switch (section)
      {
      case DataStructModel::eColFieldName:
        break;
      case DataStructModel::eColTestRegex:
        static const char *kTestRegexToolTip =
            "Regular expression to test<br>"
            " field value. Can contain<br>"
            "wildcards (\"*\").";
        tTitle = QString(kTestRegexToolTip);
        break;
      case DataStructModel::eColTestScope:
        static const char *kTestScopeToolTip =
            "Branch to suppress when<br>"
            "Test Regex fails";
        tTitle = QString(kTestScopeToolTip);
        break;
      case DataStructModel::eColFormat:
        tTitle = QString("Format applied to<br>field output");
        break;
      case DataStructModel::eColPostfix:
        static const char *kPostfixToolTip =
            "String to append to field<br>"
            "output (to achieve desired format)";
        tTitle = QString(kPostfixToolTip);
        break;
      case DataStructModel::eColFieldKey:
        tTitle = QString(
            "Key used to identify field. (Used in WSDL filter files.)");
        break;
      case DataStructModel::eColFieldType:
        tTitle = QString("C++ type of field");
        break;
      case DataStructModel::eColMatchRegex:
        tTitle = QString(
            "Expression used to matching field on incoming stream");
        break;
      }
      return tTitle;
    }
    break;
    }
  }
  return QVariant();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QModelIndex DataStructModel::index(FieldItem *aFieldItem,int aColumn)
{
  QModelIndex tModelIndex =
            createIndex(aFieldItem->row(),aColumn,aFieldItem);
  return tModelIndex;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QModelIndex DataStructModel::index(int row,int col,
      const QModelIndex &parent) const
{
  if (!hasIndex(row,col,parent))
  {
    return QModelIndex();
  }

  FieldItem *parentItem;

  if (!parent.isValid())
  {
    parentItem = _RootItem;
  }
  else
  {
    parentItem = static_cast<FieldItem*>(parent.internalPointer());
  }

  FieldItem *childItem = parentItem->child(row);
  if (childItem)
  {
    return createIndex(row,col,childItem);
  }
  else
  {
    return QModelIndex();
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QModelIndex DataStructModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return QModelIndex();
  }

  FieldItem *childItem = static_cast<FieldItem *>(index.internalPointer());
  FieldItem *parentItem = childItem->parentItem();

  if (parentItem == _RootItem)
  {
    return QModelIndex();
  }

  return createIndex(parentItem->row(),0,parentItem);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int DataStructModel::rowCount(const QModelIndex &parent) const
{
  FieldItem *parentItem;
  if (parent.column() > 0) // what's purpose of this test?
  {
    return 0;
  }

  if (!parent.isValid())
  {
    parentItem = _RootItem;
  }
  else
  {
    parentItem = static_cast<FieldItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int DataStructModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
  {
    return static_cast<FieldItem*>(parent.internalPointer())->columnCount();
  }
  else
  {
    return _RootItem->columnCount();
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint DataStructModel::getEnumIndex(Enum *aEnum,std::string aValue) const
{
  uint tEnumIdx = 0;
  for (size_t tIdx = 0; tIdx < aEnum->_Values.size(); tIdx++)
  {
    if (!aValue.compare(aEnum->_Values.at(tIdx)))
    {
      tEnumIdx = tIdx;
      break;
    }
  }
  return tEnumIdx;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint DataStructModel::getStringVectorIndex(
    const std::vector<std::string> &aStringVector,
    const std::string &aString) const
{
 uint tIdx = 0;
 std::vector<std::string>::const_iterator tIter;
 for (tIter = aStringVector.begin(); tIter != aStringVector.end(); tIter++)
 {
   if (!aString.compare(*tIter))
   {
     return tIdx;
   }
   tIdx++;
 }
 return 0; //TODO should never reach, add warning or err
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint DataStructModel::getTestScopeIndex(std::string &aTestScope) const
{
  return getStringVectorIndex(_TestScopes,aTestScope);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataStructModel::setupPostfixes()
{
  _Postfixes.push_back("\\n");
  _Postfixes.push_back("\\t");
  _Postfixes.push_back(" ");
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
uint DataStructModel::getPostfixIndex(std::string &aPostfix) const
{
  return getStringVectorIndex(_Postfixes,aPostfix);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataStructModel::applyFormatMode(
    int aFormatMode,bool aSelectedOnly,QModelIndexList aSelectList)
{
  RecordProcessor::FormatMode tFormatMode =
      static_cast<RecordProcessor::FormatMode>(aFormatMode);

  if (tFormatMode == RecordProcessor::eAsIs)
  {
    QVariant tFormat(FieldItemData::eAsIs);
    QVariant tPostfix("\\n");
    if (aSelectedOnly)
    {
      applyFormatMode(tFormat,tPostfix,aSelectList);
    }
    else
    {
      applyFormatMode(tFormat,tPostfix,_TopNodeItem);
    }
  }
  else if (tFormatMode == RecordProcessor::eLongname)
  {
    QVariant tFormat(FieldItemData::eLongnameValue);
    QVariant tPostfix("\\n");
    if (aSelectedOnly)
    {
      applyFormatMode(tFormat,tPostfix,aSelectList);
    }
    else
    {
      applyFormatMode(tFormat,tPostfix,_TopNodeItem);
    }
  }
  else if (tFormatMode == RecordProcessor::eTable)
  {
    QVariant tFormat(FieldItemData::eValue);
    QVariant tPostfix("\\t");
    if (aSelectedOnly)
    {
      applyFormatMode(tFormat,tPostfix,aSelectList);
    }
    else
    {
      applyFormatMode(tFormat,tPostfix,_TopNodeItem);
    }
  }
  else
  {
    std::cerr << "ERROR: unknown format mode" << std::endl;
  }
  emit modelUpdated();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataStructModel::onPropogateToggled(bool aPropogateToChildren)
{
  _PropogateFieldChecks = aPropogateToChildren;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataStructModel::applyFormatMode(QVariant aFormat,QVariant aPostfix,
    QModelIndexList aSelectedList)
{
  for(int i=0; i< aSelectedList.count(); i++)
  {
      QModelIndex index = aSelectedList.at(i);
      FieldItem *tFieldItem = static_cast<FieldItem*>(index.internalPointer());

      tFieldItem->setFieldFormat(aFormat);
      tFieldItem->setFieldPostfix(aPostfix);
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void DataStructModel::applyFormatMode(QVariant aFormat,QVariant aPostfix,
    FieldItem *aFieldItem)
{
  aFieldItem->setFieldFormat(aFormat);
  aFieldItem->setFieldPostfix(aPostfix);

  for (int tIdx = 0; tIdx < aFieldItem->childCount(); tIdx++)
  {
    applyFormatMode(aFormat,aPostfix,aFieldItem->child(tIdx));
  }
}

//------------------------------------------------------------------------------
// Recursively sets the check state of all descendants of a specified node to a
// specified value.
//------------------------------------------------------------------------------
void DataStructModel::setChildrenCheckStates(
    const QModelIndex &aParentIndex,Qt::CheckState aNewState)
{
  FieldItem *aParentItem =
      static_cast<FieldItem*>(aParentIndex.internalPointer());

  int tChildCount = aParentItem->childCount();
  if (tChildCount == 0)
  {
    return;
  }

  for (int i = 0; i < tChildCount; i++)
  {
    FieldItem *aChild = aParentItem->child(i);
    aChild->setCheckState(aNewState);
    QModelIndex aChildIndex = index(i,0,aParentIndex);
    setChildrenCheckStates(aChildIndex,aNewState);
  }

  QModelIndex tFirstIndex = index(0,0,aParentIndex);
  QModelIndex tLastIndex = index(tChildCount-1,0,aParentIndex);
  emit dataChanged(tFirstIndex,tLastIndex);
}

//------------------------------------------------------------------------------
// Updates a parent node's check state, and recursively updates its ancestors
// as well, in response to a new check state that was just applied to one of
// its children.
// @param aChildIndex Model index of the child whose check state has changed.
// @param aNewState The new check state of the child.
//------------------------------------------------------------------------------
void DataStructModel::updateParentCheckState(
    const QModelIndex &aChildIndex,Qt::CheckState aNewState)
{
  QModelIndex aParentIndex = parent(aChildIndex);

  if (!aParentIndex.isValid())
  {
    return;
  }

  FieldItem *tParentItem =
      static_cast<FieldItem*>(aParentIndex.internalPointer());

  bool tAllChildrenMatch = true;
  int tChildCount = tParentItem->childCount();
  for (int i = 0; i < tChildCount; i++)
  {
    FieldItem *tChild = tParentItem->child(i);
    if (tChild->getData().getCheckState() != aNewState)
    {
      tAllChildrenMatch = false;
      break;
    }
  }

  bool tChangedParent = false;
  if (tAllChildrenMatch)
  {
    if (tParentItem->getData().getCheckState() != aNewState)
    {
      tParentItem->setCheckState(aNewState);
      emit dataChanged(aParentIndex,aParentIndex);
      tChangedParent = true;
    }
  }
  else
  {
    DEBUG(sLogger,"updateParentCheckState(): all children DO NOT match");
    if (tParentItem->getData().getCheckState() != Qt::PartiallyChecked)
    {
      tParentItem->setCheckState(Qt::PartiallyChecked);
      emit dataChanged(aParentIndex,aParentIndex);
      tChangedParent = true;
    }
  }

  // If we've changed the check state of the parent, then recursively call
  // this function on the parent of the parent.
  if (tChangedParent)
  {
    updateParentCheckState(aParentIndex,aNewState);
  }
}
