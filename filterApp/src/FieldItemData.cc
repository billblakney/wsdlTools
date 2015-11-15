#include <iostream>
#include "FieldItemData.hh"

//ccl::Logger FieldItemData::sLogger("FieldItemData");

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::FieldItemData()
  : _NodeType(eNone),
    _CheckState(Qt::Unchecked),
    _Key(""),
    _Name(""),
    _Type(""),
    _FieldMatch(""),
    _FieldTest(""),
    _Postfix("")
{
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::FieldItemData(
      NodeType aNodeType,
      std::string aKey,
      std::string aName,
      std::string aType,
      std::string aMatch,
      std::string aTest,
      std::string aPostfix,
      Qt::CheckState aCheckState)
  : _NodeType(aNodeType),
    _Key(aKey),
    _Name(aName),
    _Type(aType),
    _FieldMatch(aMatch),
    _FieldTest(aTest),
    _Postfix(aPostfix),
    _CheckState(aCheckState)
  {

  }

#if 0
  void setNodeType(NodeType nodeType);
  void setKey(const std::string& key);
  void setName(const std::string& name);
  void setType(const std::string& type);
  void setMatch(const std::string& match);
  void setTest(const std::string& test);
  void setPostfix(const std::string& postfix);
  void setCheckState(Qt::CheckState checkState);
#endif
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::~FieldItemData()
{
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

FieldItemData::NodeType FieldItemData::getNodeType() const
{
  return _NodeType;
}

const std::string& FieldItemData::getKey() const
{
  return _Key;
}

const std::string& FieldItemData::getName() const
{
  return _Name;
}

const std::string& FieldItemData::getType() const
{
  return _Type;
}

const std::string& FieldItemData::getMatch() const
{
  return _FieldMatch;
}

const std::string& FieldItemData::getTest() const
{
  return _FieldTest;
}

const std::string& FieldItemData::getPostfix() const
{
  return _Postfix;
}

Qt::CheckState FieldItemData::getCheckState() const
{
  return _CheckState;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

void FieldItemData::setNodeType(NodeType nodeType)
{
  _NodeType = nodeType;
}

void FieldItemData::setKey(const std::string& key)
{
  _Key = key;
}

void FieldItemData::setName(const std::string& name)
{
  _Name = name;
}

void FieldItemData::setType(const std::string& type)
{
  _Type = type;
}

void FieldItemData::setMatch(const std::string& match)
{
  _FieldMatch = match;
}

void FieldItemData::setTest(const std::string& test)
{
  _FieldTest = test;
}

void FieldItemData::setPostfix(const std::string& postfix)
{
  _Postfix = postfix;
}

void FieldItemData::setCheckState(Qt::CheckState checkState)
{
  _CheckState = checkState;
}
