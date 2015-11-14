#include <iostream>
#include "FieldItemData.hh"

//ccl::Logger FieldItemData::sLogger("FieldItemData");

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::FieldItemData()
  : _NodeType(eNone),
    _CheckState(Qt::Unchecked),
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
      std::string aName,
      std::string aType,
      std::string aMatch,
      std::string aTest,
      std::string aPostfix,
      Qt::CheckState aCheckState)
  : _NodeType(aNodeType),
    _Name(aName),
    _Type(aType),
    _FieldMatch(aMatch),
    _FieldTest(aTest),
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

Qt::CheckState FieldItemData::getCheckState() const
{
  return _CheckState;
}

const std::string& FieldItemData::getMatch() const
{
  return _FieldMatch;
}

const std::string& FieldItemData::getTest() const
{
  return _FieldTest;
}

const std::string& FieldItemData::getName() const
{
  return _Name;
}

FieldItemData::NodeType FieldItemData::getNodeType() const
{
  return _NodeType;
}

const std::string& FieldItemData::getPostfix() const
{
  return _Postfix;
}

const std::string& FieldItemData::getType() const
{
  return _Type;
}

void FieldItemData::setCheckState(Qt::CheckState checkState)
{
  _CheckState = checkState;
}

void FieldItemData::setMatch(const std::string& match)
{
  _FieldMatch = match;
}

void FieldItemData::setTest(const std::string& test)
{
  _FieldTest = test;
}

void FieldItemData::setName(const std::string& name)
{
  _Name = name;
}

void FieldItemData::setNodeType(NodeType nodeType)
{
  _NodeType = nodeType;
}

void FieldItemData::setPostfix(const std::string& postfix)
{
  _Postfix = postfix;
}

void FieldItemData::setType(const std::string& type)
{
  _Type = type;
}
