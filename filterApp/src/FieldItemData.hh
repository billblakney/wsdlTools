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

  enum NodeType {eNone, eRoot, ePrimitive, eStruct, ePrimitiveArray,
    eStructArray};

  FieldItemData();

  FieldItemData(
      NodeType aNodeType,
      std::string aKey,
      std::string aName,
      std::string aType,
      std::string aMatch = "",
      std::string aTest = "",
      uint aTestKey = 0,
      std::string aPostfix = "newline (\"\\n\")",
      Qt::CheckState aCheckState = Qt::Unchecked);

  virtual ~FieldItemData();

  FieldItemData::NodeType getNodeType();
  std::string& getKey();
  std::string& getName();
  std::string& getType();
  std::string& getMatch();
  std::string& getTest();
  uint& getTestKey();
  std::string& getPostfix();
  Qt::CheckState getCheckState();

  void setNodeType(NodeType nodeType);
  void setKey(const std::string& key);
  void setName(const std::string& name);
  void setType(const std::string& type);
  void setMatch(const std::string& match);
  void setTest(const std::string& test);
  void setTestKey(const uint& test);
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
  uint               _TestKey; // index into DataStructModel._TestNodes
  std::string        _Postfix;
  Qt::CheckState     _CheckState;
};

#endif /* FIELDITEMDATA_HH_ */
