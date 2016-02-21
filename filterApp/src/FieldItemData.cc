#include <iostream>
#include "FieldItemData.hh"

//ccl::Logger FieldItemData::sLogger("FieldItemData");

QStringList FieldItemData::_FormatNames = formatNames();

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::FieldItemData()
  : _NodeType(eNone),
    _Key(""),
    _Name(""),
    _Type(""),
    _FieldMatch(""),
    _FieldTest(""),
    _TestScope(""),
    _Format(eAsIs),
    _Postfix(""),
    _CheckState(Qt::Unchecked),
    _TestCheckState(Qt::Unchecked)
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
      std::string aTestScope,
      Format      aFormat,
      std::string aPostfix,
      Qt::CheckState aCheckState,
      Qt::CheckState aTestCheckState)
  : _NodeType(aNodeType),
    _Key(aKey),
    _Name(aName),
    _Type(aType),
    _FieldMatch(aMatch),
    _FieldTest(aTest),
    _TestScope(aTestScope),
    _Format(aFormat),
    _Postfix(aPostfix),
    _CheckState(aCheckState),
    _TestCheckState(aTestCheckState)
{
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::~FieldItemData()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QStringList FieldItemData::getFormatStringList()
{
  return _FormatNames;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QString FieldItemData::getFormatString(Format aMode)
{
  if (aMode < _FormatNames.size())
  {
    return _FormatNames[aMode];
  }
  else
  {
    return QString();
  }
}

//-------------------------------------------------------------------------------
// Return format enum value represented by a string.
// Returns eAsIs if no match is found.
//-------------------------------------------------------------------------------
FieldItemData::Format FieldItemData::getFormat(QString aFormat)
{
  for (int i = 0; i < _FormatNames.size(); i++)
  {
    if (aFormat == _FormatNames[i])
    {
      return static_cast<FieldItemData::Format>(i);
    }
  }
  return eAsIs;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
QStringList FieldItemData::formatNames()
{
  QStringList tList;
  tList.push_back("as-is");
  tList.push_back("name: value");
  tList.push_back("longname: value");
  tList.push_back("value");
  return tList;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
bool FieldItemData::isChecked()
{
  return ( (_CheckState == Qt::Checked) ? true: false);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
bool FieldItemData::isTestChecked()
{
  return ( (_TestCheckState == Qt::Checked) ? true: false);
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

std::string& FieldItemData::getTestScope()
{
  return _TestScope;
}

FieldItemData::Format& FieldItemData::getFormat()
{
  return _Format;
}

std::string& FieldItemData::getPostfix()
{
  return _Postfix;
}

Qt::CheckState FieldItemData::getCheckState()
{
  return _CheckState;
}

Qt::CheckState FieldItemData::getTestCheckState()
{
  return _TestCheckState;
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

void FieldItemData::setTestScope(const std::string& testScope)
{
  _TestScope = testScope;
}

void FieldItemData::setFormat(const Format& format)
{
  _Format = format;
}

void FieldItemData::setPostfix(const std::string& postfix)
{
  _Postfix = postfix;
}

void FieldItemData::setCheckState(Qt::CheckState checkState)
{
  _CheckState = checkState;
}

void FieldItemData::setTestCheckState(Qt::CheckState checkState)
{
  _TestCheckState = checkState;
}
