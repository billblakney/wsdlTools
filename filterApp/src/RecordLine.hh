#ifndef _RECORDLINE_HH_
#define _RECORDLINE_HH_

#include <string>
#include "FieldItemData.hh"

class RecordLine
{
public:

  std::string nodeKey;
  std::string nodeTestRegex;
  std::string nodeTestScope;
  FieldItemData::Format nodeFormat;
  std::string nodePostfix;
  bool        nodeIsChecked;
  std::string line;
  std::string lineDotString;
  std::string lineFieldValue;
  bool        resultPassedTest;
  bool        resultLineExcluded;

  RecordLine();

  RecordLine(FieldItemData &aFieldItemData);

  RecordLine(FieldItemData &aFieldItemData,
      std::string &aLine,std::string &aLineDotString,
      std::string aLineFieldValue);

  RecordLine(FieldItemData &aFieldItemData,
      std::string &aLine,std::string &aLineDotString,
      std::string aLineFieldValue,
      bool aResultPassedTest,bool aResultLineExcluded = false);

  virtual ~RecordLine();

protected:

  void setNodeFields(FieldItemData &aFieldItemData);

  void initNodeFields();

  void initLineFields();

  void initResultFields();
};

#endif /* _RECORDLINE_HH_ */
