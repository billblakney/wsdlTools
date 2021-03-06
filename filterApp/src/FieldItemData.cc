#include <iostream>
#include "FieldItemData.hh"

//ccl::Logger FieldItemData::sLogger("FieldItemData");

QStringList FieldItemData::_FormatNames = formatNames();

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::FieldItemData()
  : _NodeType(eNone),
    _Level(0),
    _Key(""),
    _Name(""),
    _Type(""),
    _FieldMatch(""),
    _FieldTest(""),
    _TestScope(""),
    _Format(),
    _Postfix(""),
    _CheckState(Qt::Unchecked),
    _TestCheckState(Qt::Unchecked)
{
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::FieldItemData(
      NodeType    aNodeType,
      int         aLevel,
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
    _Level(aLevel),
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QString FieldItemData::getFormatString(Format aFormat)
{
  if (aFormat < _FormatNames.size())
  {
    return _FormatNames[aFormat];
  }
  else
  {
    return QString();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QStringList FieldItemData::getFormatStringList()
{
  return _FormatNames;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
QStringList FieldItemData::formatNames()
{
  QStringList tList;
  tList.push_back("eAsIs");
  tList.push_back("eNameValue");
  tList.push_back("eLongnameValue");
  tList.push_back("eValue");
  return tList;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string FieldItemData::getDefaultTestScope()
{
  return "root";
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
FieldItemData::Format FieldItemData::getDefaultFormat()
{
  return eAsIs;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string FieldItemData::getDefaultPostfix()
{
  return "\\n";
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

int FieldItemData::getLevel()
{
  return _Level;
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

void FieldItemData::setLevel(int level)
{
  _Level = level;
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
