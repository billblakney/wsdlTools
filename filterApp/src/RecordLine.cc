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
      std::string &aLine,std::string &aLineDotString)
{
  setNodeFields(aFieldItemData);

  line = aLine;
  lineDotString = aLineDotString;

  initResultFields();
}

RecordLine::RecordLine(FieldItemData &aFieldItemData,
      std::string &aLine,std::string &aLineDotString,
      bool aResultPassedTest,bool aResultLineExcluded)
{
  setNodeFields(aFieldItemData);

  line = aLine;
  lineDotString = aLineDotString;

  resultPassedTest = aResultPassedTest;
  resultLineExcluded = aResultLineExcluded;
}

RecordLine::~RecordLine()
{
}

void RecordLine::setNodeFields(FieldItemData &aFieldItemData)
{
  nodeKey = aFieldItemData.getKey();
  nodeTestRegex = aFieldItemData.getTest();
  nodeTestScope = aFieldItemData.getTestScope();
  nodeIsChecked = aFieldItemData.isChecked();
}

void RecordLine::initNodeFields()
{
  nodeKey = "";
  nodeTestRegex = "";
  nodeTestScope = "";
  nodeIsChecked = false;
}

void RecordLine::initLineFields()
{
  line = "";
  lineDotString = "";
}

void RecordLine::initResultFields()
{
  resultPassedTest = true;
  resultLineExcluded = false;
}

