#include "RecordLine.hh"

RecordLine::RecordLine()
{
  initNodeFields();
  initLineFields();
  initResultFields();
}

RecordLine::RecordLine(FieldItemData &aFieldItemData)
{
  nodeKey = aFieldItemData.getKey();
  nodeTestRegex = aFieldItemData.getTest();
  nodeTestScope = aFieldItemData.getTestScope();
  nodeIsChecked = aFieldItemData.isChecked();

  initLineFields();
  initResultFields();
}

RecordLine::RecordLine(FieldItemData &aFieldItemData,
      std::string aLine,std::string aLineDotString)
  : RecordLine(aFieldItemData)
{
  line = aLine;
  lineDotString = aLineDotString;

  initResultFields();
}

RecordLine::~RecordLine()
{
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

