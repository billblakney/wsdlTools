#ifndef FieldItemData_hh
#define FieldItemData_hh

#include <string>
#include <QCheckBox>
#include <QString>
#include <QStringList>
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
    eValue
  };
  static Format getFormat(QString aFormat);
  static QString getFormatString(Format aFormat);
  static QStringList getFormatStringList();

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
      Qt::CheckState aCheckState = Qt::Unchecked,
      Qt::CheckState aTestCheckState = Qt::Unchecked);

  virtual ~FieldItemData();

  bool isChecked();
  bool isTestChecked();

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
  Qt::CheckState getTestCheckState();

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
  void setTestCheckState(Qt::CheckState checkState);

protected:

//  static ccl::Logger sLogger;

  static QStringList _FormatNames;
  static QStringList formatNames();

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
  Qt::CheckState     _TestCheckState;
};

#endif /* FieldItemData_hh */
