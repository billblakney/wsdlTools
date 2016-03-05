#include <iostream>
#include <map>

#include "DataStructModel.hh"
#include "HeaderUtil.hh"
#include "TestRecordBuilder.hh"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TestRecordBuilder::TestRecordBuilder(QString aMsgId,QString aStructName,
    QString aHeaderPath)
  : _MsgId(aMsgId),
    _StructName(aStructName),
    _HeaderPath(aHeaderPath)
{
  // Parse the header file to get _StructBuilder.
  StructorBuilder *tStructorBuilder =
      HeaderUtil::parseHeaderFile(_HeaderPath);

  Structure *tStructure =
      tStructorBuilder->getStructure(_StructName.toStdString());

  if (!tStructure)
  {
    std::cerr << "ERROR: couldn't find struct "
              << qPrintable(_StructName) << std::endl;
    exit(0);
  }

  /*
   * Create the data structure model for the specified data structure.
   */
  DataStructModel *tModel = new DataStructModel(tStructure,tStructorBuilder);
  _TopNode = tModel->getTopNode();
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TestRecordBuilder::~TestRecordBuilder()
{
}

//-------------------------------------------------------------------------------
// TODO shouldn't need to pass aStructName
// TODO format
// TODO ssifg
// TODO separate class
//-------------------------------------------------------------------------------
std::string TestRecordBuilder::getTestRecord()
{
  std::string tStr;
  tStr += "==============RECEIVED MESSAGE=========\n";
  tStr += ".......................................\n";
  tStr += "... " + _MsgId.toStdString() + " ... " + _StructName.toStdString();
  tStr += "\n";
  tStr += ".......................................\n";

  tStr += getTestString(_TopNode);

  tStr += "==============END MESSAGE=========\n";

  return tStr;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string TestRecordBuilder::getTestString(FieldItem *aNode)
{
  std::string tReturn;

  FieldItemData &tData = aNode->getData();
  FieldItemData::NodeType tType = tData.getNodeType();

  switch (tType)
  {
  case FieldItemData::eRoot:
    addChildren(aNode,0,tReturn);
    break;
  case FieldItemData::eStruct:
    addHeader(tData,tReturn);
    addChildren(aNode,0,tReturn);
    break;
  case FieldItemData::eStructArray:
  {
    const int kStructArrayLength = 2;
    addHeader(tData,tReturn);
    addArrayLenNode(
        aNode->child(0)->getData(),kStructArrayLength,tReturn);
    for (int i = 0; i < kStructArrayLength; i++)
    {
      addChildren(aNode,1,tReturn);
    }
  }
  break;
  case FieldItemData::ePrimitiveArray:
  {
    const int kPrimitiveArrayLength = 3;
    addHeader(tData,tReturn);
    addArrayLenNode(
        aNode->child(0)->getData(),kPrimitiveArrayLength,tReturn);
    for (int i = 0; i < kPrimitiveArrayLength; i++)
    {
      addPrimitiveArrayValue(tData,i,tReturn);
    }
  }
  break;
  case FieldItemData::ePrimitiveValue:
  {
    addPrimitiveValue(tData,tReturn);
  }
  break;
  default:
    std::cout << "ERROR: unexpected node type while creating test string"
              << std::endl;
    break;
  }

  return tReturn;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void TestRecordBuilder::addTabs(int aNum,std::string &aStr)
{
  for (int tIdx = 0; tIdx < aNum; tIdx++)
  {
    aStr += "\t";
  }
}

//-------------------------------------------------------------------------------
// TODO comment
//-------------------------------------------------------------------------------
void TestRecordBuilder::addChildren(FieldItem *aParentNode,int aFirstChild,
    std::string &aStr)
{
    for (int i = aFirstChild; i < aParentNode->childCount(); i++)
    {
      aStr += getTestString(aParentNode->child(i));
    }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void TestRecordBuilder::addHeader(FieldItemData &aData,std::string &aStr)
{
    addTabs(aData.getLevel(),aStr);
    aStr += aData.getName();
    aStr += ":\n";
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void TestRecordBuilder::addArrayLenNode(FieldItemData &aData,int aLength,
    std::string &aStr)
{
  addTabs(aData.getLevel(),aStr);
  std::stringstream tStream;
  tStream << "array of len: " << aLength << std::endl;
  aStr += tStream.str();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void TestRecordBuilder::addPrimitiveArrayValue(FieldItemData &aData,int aIdx,
    std::string &aStr)
{
  addTabs(aData.getLevel()+1/*TODO add comment*/,aStr);
  std::stringstream tStream;
  tStream << "[" << aIdx << "] =\t{" << aData.getName() << aIdx << "}\n";
  aStr += tStream.str();
//  aStr += "[0] =\tvalue\n";//TODO rm
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void TestRecordBuilder::addPrimitiveValue(FieldItemData &aData,std::string &aStr)
{
  /*
   * Map of keys to counts. Will add the count to the value string to give some
   * unique values to the field values
   */
  static std::map<std::string,int> kCountMap;

  /*
   * Add tabs.
   */
  addTabs(aData.getLevel(),aStr);

  /*
   * Get the count.
   */
  std::string tKey = aData.getKey();
  std::map<std::string,int>::iterator tIter = kCountMap.find(tKey);
  int tCount = 0;
  if (tIter != kCountMap.end())
  {
    tCount = ++tIter->second;
  }
  else
  {
    kCountMap[tKey] = 0;
  }
  std::stringstream tStream;
  tStream << tCount;

  /*
   * Build the value string.
   */
  aStr += aData.getName();
  aStr += ":\t{" + aData.getName() + tStream.str() + "}\n";
}
