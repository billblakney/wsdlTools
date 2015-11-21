#include <iostream>
#include <iterator>
#include <sstream>
#include <boost/regex.hpp>
#include <QVariant>
#include "DataStructModel.hh"

ccl::Logger DataStructModel::sLogger("DataStructModel");

QFont DataStructModel::kArrayFont;

//-------------------------------------------------------------------------------
// Constructs all of the FieldItems that comprise the tree of fields.
//-------------------------------------------------------------------------------
DataStructModel::DataStructModel(
    Structure *aStruct,StructorBuilder *aStructBuilder)
  : _StructBuilder(aStructBuilder),
    _RootItem(0),
    _TopNodeItem(0)
{
  kArrayFont.setItalic(true);

  setupPostfixes();

  // root item
  FieldItemData tRootData(FieldItemData::eRoot,"root",aStruct->_Name,aStruct->_Name);
  _RootItem = new FieldItem(tRootData);

  // top node item
  FieldItemData tTopData(FieldItemData::eRoot,"","root",aStruct->_Name);
  _TopNodeItem = new FieldItem(tTopData,_RootItem);

  _RootItem->appendChild(_TopNodeItem);

  buildTree(_TopNodeItem,aStruct);

//DEBUG(sLogger,"CTOR getDotString: "
//    << getDotString(aStructBuilder,aStructure->_Name,"aStruct"));
//DEBUG(sLogger,"CTOR getMatchString: "
//    << getMatchString(_TopNodeItem));
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItem *DataStructModel::getTopNode()
{
  return _TopNodeItem;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::vector<std::string> DataStructModel::getTestNodes()
{
  return _TestScopes;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::vector<std::string> DataStructModel::getPostfixes()
{
  return _Postfixes;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void DataStructModel::printTestNodes()
{
  std::cout << "Test Nodes:" << std::endl;
  std::vector<std::string>::iterator tIter;
  for (tIter = _TestScopes.begin(); tIter != _TestScopes.end(); tIter++)
  {
    std::cout << "   " << *tIter << std::endl;
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
DataStructModel::~DataStructModel()
{
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

//-------------------------------------------------------------------------------
// Gets the match for the first field in the data structure. This will be used
// by the reader to identify the start of the structure (since in wsdl there is
// no other identifier,like the name of the structure, e.g.).
//-------------------------------------------------------------------------------
std::string DataStructModel::getFirstFieldMatch()
{
  return _TopNodeItem->child(0)->getData().getMatch();
}

//-------------------------------------------------------------------------------
// Completes building a tree if field items after the root and top items have
// been created. Is initially called with the top item, and then is called
// recursively.
//-------------------------------------------------------------------------------
void DataStructModel::buildTree(
    FieldItem *aParentItem,Structure *aStruct,int aLevel)
{
  boost::regex array_size_regex("^([a-zA-Z]+)_size$");

  if (aLevel == 0)
  {
      // nothing special to do at level zero
    _TestScopes.push_back("root");
  }

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
#if 0
  for( it = tStructure->_Fields.begin(); it != tStructure->_Fields.end(); it++)
  {
    std::string tName = it->_Name;
    std::string tType = it->_Type;
    std::string tFieldString = aPrefix + "." + tName;
    std::string tFieldDotString = getDotString(tType,tFieldString);
    if( tFieldDotString.length() )
    {
      tReturn << tFieldDotString;
    }
    else
    {
      tReturn << tFieldString << endl;
    }
  }
#endif
}

//-------------------------------------------------------------------------------
// Build a primitive array node for a struct field.
//-------------------------------------------------------------------------------
void DataStructModel::buildPrimitiveArrayNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "primitive array node: "
      << aField._Type << ":" << aField._Name);

  std::string tKey = buildKey(aField,aParentItem,true);
  std::string tMatch = buildMatchForPrimitiveArrayField(aField,aLevel);

  FieldItemData tData(FieldItemData::ePrimitiveArray,tKey,
      aField._Name,aField._Type,tMatch);

  FieldItem *dataItem = new FieldItem(tData,aParentItem);
  aParentItem->appendChild(dataItem);

  //
  // Set the test nodes - one test node for specifying the whole array, and one
  // for specifying individual array elements.
  //
  _TestScopes.push_back(tData.getKey());
  _TestScopes.push_back(tData.getKey() + ".Element");
}

//-------------------------------------------------------------------------------
// Build a struct array node for a struct field.
// TODO what if tStruct is NULL?
//-------------------------------------------------------------------------------
void DataStructModel::buildStructArrayNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "struct array node: "
      << aField._Type << ":" << aField._Name);

  Structure *tStruct = _StructBuilder->getStructure(aField._Type);
  if (tStruct != NULL)
  {
    std::string tKey = buildKey(aField,aParentItem,true);
    std::string tMatch = buildMatchForStructArrayField(aField,aLevel);

    FieldItemData tData(FieldItemData::eStructArrayHeader,tKey,
        aField._Name,aField._Type,tMatch);

    FieldItem *dataItem = new FieldItem(tData,aParentItem);

    aParentItem->appendChild(dataItem);

  //
  // Set the test nodes - one test node for specifying the whole array, and one
  // for specifying individual array elements.
  //
  _TestScopes.push_back(tData.getKey());
  _TestScopes.push_back(tData.getKey() + ".Element");

    buildTree(dataItem,tStruct,++aLevel);
    --aLevel;
  }
}

//-------------------------------------------------------------------------------
// Build a tree node for a struct field.
//-------------------------------------------------------------------------------
void DataStructModel::buildStructNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "struct node: "
      << aField._Type << ":" << aField._Name);
  Structure *tStruct = _StructBuilder->getStructure(aField._Type);
  if (tStruct != NULL)
  {
    std::string tKey = buildKey(aField,aParentItem,false);
    std::string tMatch = buildMatchForStructField(aField,aLevel);
    FieldItemData tData(FieldItemData::eStructHeader,tKey,
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
  else
  {
    ERROR(sLogger,blanks[aLevel] << "Can't find struct " << aField._Name);
  }
}

//-------------------------------------------------------------------------------
// Build a tree node for a primitive field.
//-------------------------------------------------------------------------------
void DataStructModel::buildPrimitiveNode(
    Field &aField,FieldItem *aParentItem,int &aLevel)
{
  DEBUG(sLogger,blanks[aLevel] << "primitive node: "
      << aField._Type << ":" << aField._Name);

  std::string tKey = buildKey(aField,aParentItem,false);
  std::string tMatch = buildMatchForPrimitiveField(aField,aLevel);

  FieldItemData tData(FieldItemData::ePrimitiveValue,tKey,
      aField._Name,aField._Type,tMatch);

  FieldItem *dataItem = new FieldItem(tData,aParentItem);

  aParentItem->appendChild(dataItem);
}

//-------------------------------------------------------------------------------
// Build a primitive array node for a struct field.
//-------------------------------------------------------------------------------
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

#if 0 //TODO remove after confirm don't need anymore
  if (aIsArrayType)
  {
    tKey += "[]";
  }
#endif

  return tKey;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForField(
    const Field &aField,int aIndentLevel)
{
  char tBuffer[50];
  sprintf(tBuffer,"^[\\t]{%d}%s:",aIndentLevel,aField._Name.c_str());
  return tBuffer;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForPrimitiveArrayField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForField(aField,aIndentLevel);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForPrimitiveField(
    const Field &aField,int aIndentLevel)
{
  static char tBuffer[200];
  sprintf(tBuffer,"^[\\t]{%d}%s:[\\s]+(.*)",aIndentLevel,aField._Name.c_str());
  return tBuffer;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForStructArrayField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForField(aField,aIndentLevel);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string DataStructModel::buildMatchForStructField(
    const Field &aField,int aIndentLevel)
{
  return buildMatchForField(aField,aIndentLevel);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::string DataStructModel::getDotString(StructorBuilder *aStructBuilder,
    std::string aName,std::string aPrefix)
{
  stringstream tReturn;

  Structure *tStructure = aStructBuilder->getStructure(aName);

  if (tStructure != NULL)
  {
    vector<Field>::iterator it;
    for( it = tStructure->_Fields.begin(); it != tStructure->_Fields.end(); it++)
    {
      std::string tName = it->_Name;
      std::string tType = it->_Type;
      std::string tFieldString = aPrefix + "." + tName;
      std::string tFieldDotString = getDotString(aStructBuilder,tType,tFieldString);
      if( tFieldDotString.length() )
      {
        tReturn << tFieldDotString;
      }
      else
      {
        tReturn << tFieldString << endl;
      }
    }
  }

  return tReturn.str();
}

//-------------------------------------------------------------------------------
// Not sure this will be used or what it was originally for.
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
// Gets the match string for the simple reader.
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::vector<FieldItem *> DataStructModel::getCheckedFields()
{
  std::vector<FieldItem *> tFieldItems;
  addCheckedFields(_TopNodeItem,tFieldItems);
  return tFieldItems;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
QVariant DataStructModel::data(const QModelIndex &index,int role) const
{
  if (!index.isValid())
  {
    return QVariant();
  }

  FieldItem *item = static_cast<FieldItem*>(index.internalPointer());

  switch (role) {
    case Qt::DisplayRole:
      return item->data(index.column());
      break;
    case Qt::EditRole:
    {
      if (index.column() == eColTestKey)
      {
        std::string tTestScope = item->data(eColTestKey).toString().toStdString();
        uint tTestNodeIndex = getTestScopeIndex(tTestScope);

        return tTestNodeIndex;
      }
      else if (index.column() == eColPostfix)
      {
        std::string tPostfix = item->data(eColPostfix).toString().toStdString();
        uint tPostfixIndex = getPostfixIndex(tPostfix);

        return tPostfixIndex;
      }
      return item->data(index.column());
      break;
    }
    case Qt::CheckStateRole:
      if (index.column() == eColFieldName)
      {
        return static_cast<int>(item->getData().getCheckState());
      }
      break;
    case Qt::FontRole:
      if( item->getData().getNodeType() == FieldItemData::eStructArrayHeader
       || item->getData().getNodeType() == FieldItemData::ePrimitiveArray)
      {
        return kArrayFont;
      }
      break;
  }

  return QVariant();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
bool DataStructModel::setData(
    const QModelIndex &index,const QVariant &value,int role)
{
//  int aRow = index.row();
  int aCol = index.column();

  if( role == Qt::CheckStateRole && aCol == eColFieldName)
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

    item->setCheckState(tNewState);
    emit dataChanged(index,index);

    if (item->childCount() > 0)
    {
      setChildrenCheckStates(index,tNewState);
    }

#ifdef UPDATE_PARENT_CHECK_STATES
    updateParentCheckState(index,tNewState);
#endif
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
      item->setFieldTest(value);
      emit dataChanged(index,index);
    }
    else if (aCol == eColTestKey)
    {
      std::string tStr = _TestScopes.at(value.toUInt());
      item->setTestScope(QVariant(tStr.c_str()));
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
Qt::ItemFlags DataStructModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
  {
    return 0;
  }

  Qt::ItemFlags tFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (index.column() == eColFieldName)
  {
    tFlags |= Qt::ItemIsUserCheckable;
  }

  if (index.column() == eColMatchRegex ||
      index.column() == eColTestRegex ||
      index.column() == eColTestKey ||
      index.column() == eColPostfix)
  {
    tFlags |= Qt::ItemIsEditable;
  }

  return tFlags;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
QVariant DataStructModel::headerData(int section,Qt::Orientation orientation,
      int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
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
      return QVariant("Match Regex");
    }
    else if (section == eColTestRegex)
    {
      return QVariant("Test Regex");
    }
    else if (section == eColTestKey)
    {
      return QVariant("Test Scope");
    }
    else if (section == eColPostfix)
    {
      return QVariant("Postfix");
    }
  }

  return QVariant();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
uint DataStructModel::getTestScopeIndex(std::string &aTestScope) const
{
  return getStringVectorIndex(_TestScopes,aTestScope);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void DataStructModel::setupPostfixes()
{
  _Postfixes.push_back("\\n");
  _Postfixes.push_back("\\t");
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
uint DataStructModel::getPostfixIndex(std::string &aPostfix) const
{
  return getStringVectorIndex(_Postfixes,aPostfix);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
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
