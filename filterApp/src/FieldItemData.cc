#include <iostream>
#include "FieldItemData.hh"

//ccl::Logger FieldItemData::sLogger("FieldItemData");

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::FieldItemData()
  : _NodeType(eNone),
    _Key(""),
    _Name(""),
    _Type(""),
    _FieldMatch(""),
    _FieldTest(""),
    _TestKey(0),
    _Postfix(""),
    _CheckState(Qt::Unchecked)
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
      uint aTestKey,
      std::string aPostfix,
      Qt::CheckState aCheckState)
  : _NodeType(aNodeType),
    _Key(aKey),
    _Name(aName),
    _Type(aType),
    _FieldMatch(aMatch),
    _FieldTest(aTest),
    _TestKey(aTestKey),
    _Postfix(aPostfix),
    _CheckState(aCheckState)
  {

  }

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::~FieldItemData()
{
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

FieldItemData::NodeType FieldItemData::getNodeType()
{
  return _NodeType;
}

std::string& FieldItemData::getKey()
{
  return _Key;
}

std::string& FieldItemData::getName()
{
  return _Name;
}

std::string& FieldItemData::getType()
{
  return _Type;
}

std::string& FieldItemData::getMatch()
{
  return _FieldMatch;
}

std::string& FieldItemData::getTest()
{
  return _FieldTest;
}

uint& FieldItemData::getTestKey()
{
  return _TestKey;
}

std::string& FieldItemData::getPostfix()
{
  return _Postfix;
}

Qt::CheckState FieldItemData::getCheckState()
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

void FieldItemData::setTestKey(const uint& testKey)
{
  _TestKey = testKey;
}

void FieldItemData::setPostfix(const std::string& postfix)
{
  _Postfix = postfix;
}

void FieldItemData::setCheckState(Qt::CheckState checkState)
{
  _CheckState = checkState;
}
