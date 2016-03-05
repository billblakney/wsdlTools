#ifndef TestRecordBuilder_hh
#define TestRecordBuilder_hh

#include <string>
#include <QString>

#include "FieldItem.hh"
#include "FieldItemData.hh"

class TestRecordBuilder
{
public:
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  TestRecordBuilder(QString aMsgId,QString aStructName,QString aHeaderPath);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  virtual ~TestRecordBuilder();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::string getTestRecord();

protected:
  QString          _MsgId;
  QString          _StructName;
  QString          _HeaderPath;
  FieldItem       *_TopNode;

  std::string getTestString(FieldItem *aNode);
  void addTabs(int aNum,std::string &aStr);
  void addChildren(FieldItem *aParentNode,int aFirstChild,std::string &aStr);
  void addHeader(FieldItemData &aData,std::string &aStr);
  void addArrayLenNode(FieldItemData &aData,int aLength,std::string &aStr);
  void addPrimitiveArrayValue(FieldItemData &aData,int aIdx,std::string &aStr);
  void addPrimitiveValue(FieldItemData &aData,std::string &aStr);
};

#endif /* TestRecordBuilder_hh */
