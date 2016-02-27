#include "RecordLine.hh"

RecordLine::RecordLine()
{
  initNodeFields();
  initLineFields();
  initResultFields();
}

RecordLine::RecordLine(FieldItemData &aFieldItemData)
{
  setNodeFields(aFieldItemData);

  initLineFields();
  initResultFields();
}

RecordLine::RecordLine(FieldItemData &aFieldItemData,
      std::string &aLine,std::string &aLineDotString,
      std::string aLineFieldValue)
{
  setNodeFields(aFieldItemData);

  line = aLine;
  lineDotString = aLineDotString;
  lineFieldValue = aLineFieldValue;

  initResultFields();
}

RecordLine::RecordLine(FieldItemData &aFieldItemData,
      std::string &aLine,std::string &aLineDotString,
      std::string aLineFieldValue,
      bool aResultPassedTest,bool aResultLineExcluded)
{
  setNodeFields(aFieldItemData);

  line = aLine;
  lineDotString = aLineDotString;
  lineFieldValue = aLineFieldValue;

  resultPassedTest = aResultPassedTest;
  resultLineExcluded = aResultLineExcluded;
}

RecordLine::~RecordLine()
{
}

void RecordLine::setNodeFields(FieldItemData &aFieldItemData)
{
  nodeKey = aFieldItemData.getKey();
  nodeName = aFieldItemData.getName();
  nodeTestChecked = aFieldItemData.isTestChecked();
  nodeTestRegex = aFieldItemData.getTest();
  nodeTestScope = aFieldItemData.getTestScope();
  nodeFormat = aFieldItemData.getFormat();
  nodePostfix = aFieldItemData.getPostfix();
  nodeIsChecked = aFieldItemData.isChecked();
}

void RecordLine::initNodeFields()
{
  nodeKey = "";
  nodeName = "";
  nodeTestChecked = false;
  nodeTestRegex = "";
  nodeTestScope = "";
  nodeFormat = FieldItemData::eAsIs;
  nodePostfix = "\n";
  nodeIsChecked = false;
}

void RecordLine::initLineFields()
{
  line = "";
  lineDotString = "";
  lineFieldValue = "";
}

void RecordLine::initResultFields()
{
  resultPassedTest = true;
  resultLineExcluded = false;
}

