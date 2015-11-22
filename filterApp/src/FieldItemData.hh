#ifndef FIELDITEMDATA_HH_
#define FIELDITEMDATA_HH_

#include <string>
#include <QCheckBox>
#include "Logger.hh"

/**
 * FieldItemData represents a node in the data structure tree.
 */
class FieldItemData
{
public:

  enum NodeType {
    eNone,
    eRoot,
    eStruct,
    eStructArray,
    eStructArrayLength,
    ePrimitiveArray,
    ePrimitiveArrayLength,
    ePrimitiveValue,
    ePrimitiveArrayValue,
    eNodeTypeEnd
  };

  enum Format {
    eAsIs,
    eNameValue,
    eLongnameValue,
    eValue,
    eFormatEnd
  };

  FieldItemData();

  FieldItemData(
      NodeType aNodeType,
      std::string aKey,
      std::string aName,
      std::string aType,
      std::string aMatch = "",
      std::string aTest = "",
      std::string aTestScope = "root",
      Format      aFormat = eAsIs,
      std::string aPostfix = "\\n",
      Qt::CheckState aCheckState = Qt::Unchecked);

  virtual ~FieldItemData();

  bool isChecked();

  FieldItemData::NodeType getNodeType();
  std::string& getKey();
  std::string& getName();
  std::string& getType();
  std::string& getMatch();
  std::string& getTest();
  std::string& getTestScope();
  Format&      getFormat();
  std::string& getPostfix();
  Qt::CheckState getCheckState();

  void setNodeType(NodeType nodeType);
  void setKey(const std::string& key);
  void setName(const std::string& name);
  void setType(const std::string& type);
  void setMatch(const std::string& match);
  void setTest(const std::string& test);
  void setTestScope(const std::string& testScope);
  void setFormat(const Format& format);
  void setPostfix(const std::string& postfix);
  void setCheckState(Qt::CheckState checkState);

protected:

//  static ccl::Logger sLogger;

  NodeType           _NodeType;
  std::string        _Key;
  std::string        _Name;
  std::string        _Type;
  std::string        _FieldMatch;
  std::string        _FieldTest;
  std::string        _TestScope;
  Format             _Format;
  std::string        _Postfix;
  Qt::CheckState     _CheckState;
};

#endif /* FIELDITEMDATA_HH_ */
