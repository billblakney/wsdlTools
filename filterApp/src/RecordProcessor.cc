#include <sstream>
#include <boost/regex.hpp>
#include "RecordProcessor.hh"

ccl::Logger RecordProcessor::sLogger("RecordProcessor");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RecordProcessor::RecordProcessor(
    FieldItem *aTopNode,std::vector<std::string> &aLinesIn)
  : _TopNode(aTopNode),
    _LinesIn(aLinesIn),
    _TestResult(true)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RecordProcessor::~RecordProcessor()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool RecordProcessor::process()
{
  reset();
  std::string tDotString;
  bool tResult = process(_TopNode,tDotString);

#ifdef OLD
  std::cout
      << "=== % nCk, nKey, nTstRgx, tstScp, lTstRes, lDotStr, line, "
      << std::endl;
//  sprintf(buff,"%d %-20s %-10s %u: %d, %-30s, %-30s",
#endif
  char buff[1000];
  sprintf(buff,"%3s %-20s %-10s %5s: %5s, %-30s, %-30s",
      "nCk","nKey","nTstRgx","nTScp","lTRes","lDotStr","line");
  std::cout << buff << std::endl;

  /*
   * Print the rec lines.
   */
  std::vector<RecLine>::iterator tIter;
  for (tIter = _RecLines.begin(); tIter != _RecLines.end(); tIter++)
  {
#if 0
    std::cout << "% "
                      << tIter->nodeIsChecked << ","
                      << tIter->lineTestResult << ","
                      << tIter->lineDotString << ","
                      << tIter->nodeKey << ","
                      << tIter->line << ","
                      << std::endl;
#endif
    sprintf(buff,"%3d %-20s %-10s %5u: %5d, %-30s, %-30s",
        tIter->nodeIsChecked,
        tIter->nodeKey.c_str(),
        tIter->nodeTestRegex.c_str(),
        tIter->nodeTestScope,
        tIter->lineTestResult,
        tIter->lineDotString.c_str(),
        tIter->line.c_str());
    std::cout << buff << std::endl;
  }

#define OLD_SIMPLE
#ifdef OLD_SIMPLE
  /*
   * Check test results to see if this record gets printed.
   */
  bool tGoesOut = true;
  for (tIter = _RecLines.begin(); tIter != _RecLines.end(); tIter++)
  {
    if (tIter->lineTestResult == false)
    {
      tGoesOut = false;
      break;
    }
  }

  /*
   * Add lines to the output lines.
   */
  if (tGoesOut)
  {
    for (tIter = _RecLines.begin(); tIter != _RecLines.end(); tIter++)
    {
      if (tIter->nodeIsChecked)
      {
        _LinesOut.push_back(tIter->line);
//        std::cout << "%%- " << tIter->line << std::endl;
      }
    }
  }
  else
  {
    std::cout << "%%- FAILED TEST" << std::endl;
  }
#endif

  applyTestResults();

  return tResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RecordProcessor::applyTestResults()
{
#if 0
  std::vector<RecLine>::iterator tIter;
  for (tIter = _RecLines.begin(); tIter != _RecLines.end(); tIter++)
  {
    if (tIter->nodeTestRegex.length() < 1)
      continue;

    std::string tScopeRegexStr()
    boost::regex tScopeRegex("(.*)((\.Element){0,1})");

    boost::match_results<std::string::const_iterator> what;
    if (boost::regex_match(tIter->nodeTestScope,what,tScopeRegex))
    {
    }

  }
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool RecordProcessor::passedFilterTests()
{
  return _TestResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::vector<std::string> &RecordProcessor::getOutLines()
{
  return _LinesOut;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RecordProcessor::reset()
{
  _RecLines.clear();
  _LineIter = _LinesIn.begin();
  _LinesOut.clear();
  _TestResult = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool RecordProcessor::process(FieldItem *aNode,std::string &aDotString)
{
  if ( aNode->getData().getNodeType() == FieldItemData::eRoot )
  {
    processRootNode(aNode,aDotString);
  }
  else if ( aNode->getData().getNodeType() == FieldItemData::eStruct)
  {
    processStructNode(aNode,aDotString);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::eStructArray)
  {
    processStructArrayNode(aNode,aDotString);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::ePrimitive)
  {
    processPrimitiveNode(aNode,aDotString);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::ePrimitiveArray)
  {
    processPrimitiveArrayNode(aNode,aDotString);
  }
  else
  {
    ERROR(sLogger,"<process> unknown node type");
    return false;
  }

  return true;
}

//-----------------------------------------------------------------------------
// Processes the set of lines for record.
// Doesn't directly consume any lines. Indirectly consumes the set of lines
// for the entire record represented by the root node.
//-----------------------------------------------------------------------------
bool RecordProcessor::processRootNode(FieldItem *aNode,std::string &aDotString)
{
  DEBUG(sLogger,"processing eRoot: " << aNode->getData().getName());

  return processChildren(aNode,aDotString);
}

//-----------------------------------------------------------------------------
// Processes the set of lines for a struct.
// If the node being processed is a struct node, then a single header line is
// directly consumed. If the node being processed is a struct array node, then
// no lines are directly consumed, since no header line is present. In both
// cases the set of lines for the entire struct are indirectly consumed.
// When a struct array node is calling this routine, it will call this method
// repeatedly for the number of struct elements in the array.
//-----------------------------------------------------------------------------
bool RecordProcessor::processStructNode(FieldItem *aNode,std::string &aDotString)
{
  DEBUG(sLogger,"processing eStruct: " << aNode->getData().getMatch());

  std::string tDotString = aDotString;

  /*
   * If the node being processed is a regular struct node, then the set of
   * lines to be processed will include a header line with the struct name.
   * If the node being processed is a struct array node, then the set of lines
   * for a struct does not include a header line.
   */
  if (aNode->getData().getNodeType() != FieldItemData::eStructArray)
  {
    appendToDotString(tDotString,aNode->getData().getName());
    //print("<dotstring>",tDotString);

    std::string &tLine = *_LineIter;
    _LineIter++;

    bool tIsChecked = false;

    _Matcher.setMatchRegex(aNode->getData().getMatch());
    if (_Matcher.match(tLine))
    {
      if (aNode->getData().getCheckState() == Qt::Checked)
      {
        tIsChecked = true;
//        _LinesOut.push_back(tLine);
      }
      DEBUG(sLogger,"struct node matched: " << aNode->getData().getMatch());
    }
    else
    {
      ERROR(sLogger,"struct node <" << tLine << "> didn't match: <"
          << aNode->getData().getMatch() << ">");
      return false;
    }

    RecLine tRecLine;
    tRecLine.nodeKey = aNode->getData().getKey();
    tRecLine.nodeTestRegex = aNode->getData().getTest();
    tRecLine.nodeTestScope = 0;
    tRecLine.nodeIsChecked = tIsChecked;
    tRecLine.line = tLine;
    tRecLine.lineDotString = tDotString;
    tRecLine.lineTestResult = true;
    _RecLines.push_back(tRecLine);
  }
  return processChildren(aNode,tDotString);
}

//-----------------------------------------------------------------------------
// Processes the set of lines for a struct array node.
// Directly consumes two lines: one specifying the type of the array elements,
// and one specifying the array length. Indirectly consumes all of the lines
// for all of the struct array elements.
//-----------------------------------------------------------------------------
bool RecordProcessor::processStructArrayNode(FieldItem *aNode,std::string &aDotString)
{
  DEBUG(sLogger,"Looking for struct array with match: " << aNode->getData().getMatch());

  std::string tDotString = aDotString;
  appendToDotString(tDotString,aNode->getData().getName());
  //print("<dotstring>",tDotString);

  /*
   * Going to test the line that indicates the start of the struct array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  bool tIsChecked = false;

  _Matcher.setMatchRegex(aNode->getData().getMatch());
  if (_Matcher.match(tLine))
  {
    if (aNode->getData().getCheckState() == Qt::Checked)
    {
      tIsChecked = true;
//      _LinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"struct array node matched: " << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"struct array node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  RecLine tRecLine;
  tRecLine.nodeKey = aNode->getData().getKey();
  tRecLine.nodeTestRegex = aNode->getData().getTest();
  tRecLine.nodeTestScope = 0;
  tRecLine.nodeIsChecked = tIsChecked;
  tRecLine.line = tLine;
  tRecLine.lineDotString = tDotString;
  tRecLine.lineTestResult = true;
  _RecLines.push_back(tRecLine);

  /*
   * Going to test the line that specifies the number of elements in the
   * struct array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *_LineIter;
  _LineIter++;

  tIsChecked = true; // good for now; add node to tree?

  _Matcher.setMatchRegex(".*array of len: (\\d+)"); //TODO exact match of tabs?
  if (_Matcher.match(tLine))
  {
    DEBUG(sLogger,"array of len matched: " << tLine);
  }
  else
  {
    ERROR(sLogger,"array of len didn't match: <" << tLine);
    return false;
  }

  RecLine tArrayRecLine;
  tArrayRecLine.nodeKey = aNode->getData().getKey();
  tArrayRecLine.nodeTestRegex = aNode->getData().getTest();
  tArrayRecLine.nodeTestScope = 0;
  tArrayRecLine.nodeIsChecked = tIsChecked;
  tArrayRecLine.line = tLine;
  tArrayRecLine.lineDotString = tDotString;
  tArrayRecLine.lineTestResult = true;
  _RecLines.push_back(tArrayRecLine);

  int tArrayLen = atoi(_Matcher.getWhat().c_str());

  /*
   * Process the structures that follow per the array length.
   */
  for (int tIdx = 0; tIdx < tArrayLen; tIdx++)
  {
    std::stringstream tStream;
    tStream << tDotString << "[" << tIdx << "]";
    std::string tArrayDotString = tStream.str();

    bool tResult = processStructNode(aNode,tArrayDotString);
    if (tResult == false)
    {
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// Processes the line for a primitive node. The line consists of the field
// name and its value.
//-----------------------------------------------------------------------------
bool RecordProcessor::processPrimitiveNode(FieldItem *aNode,std::string &aDotString)
{
  std::string tDotString = aDotString;
  appendToDotString(tDotString,aNode->getData().getName());
  //print("<dotstring>",tDotString);

  /*
   * Going to test the single line that represents a primitive field.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  bool tIsChecked = false;

  _Matcher.setMatchRegex(aNode->getData().getMatch());

  if (_Matcher.match(tLine))
  {
    if (aNode->getData().getCheckState() == Qt::Checked)
    {
      tIsChecked = true;
//      _LinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"primitive node matched: " << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"Primitive node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  bool tTestResult = true;
  uint tTestScope = 0;

  std::string tFieldValue = _Matcher.getWhat();
  if (!testForMatch(tFieldValue,aNode->getData().getTest()))
  {
    DEBUG(sLogger,"test FAILED for " << aNode->getData().getName());
    tTestScope = aNode->getData().getTestKey();
    tTestResult = false;
    _TestResult = false;
  }

  RecLine tRecLine;
  tRecLine.nodeKey = aNode->getData().getKey();
  tRecLine.nodeTestRegex = aNode->getData().getTest();
  tRecLine.nodeTestScope = tTestScope;
  tRecLine.nodeIsChecked = tIsChecked;
  tRecLine.line = tLine;
  tRecLine.lineDotString = tDotString;
  tRecLine.lineTestResult = tTestResult;
  _RecLines.push_back(tRecLine);

  return true;
}

//-----------------------------------------------------------------------------
// Processes the set of lines for a primitive array node.
// Directly consumes two lines: one specifying the name of the array and one
// specifying the array length. Indirectly consumes all of the lines for the
// entire primitive array.
//-----------------------------------------------------------------------------
bool RecordProcessor::processPrimitiveArrayNode(FieldItem *aNode,std::string &aDotString)
{
  DEBUG(sLogger,"Looking for primitive array with match: "
      << aNode->getData().getMatch());

  std::string tDotString = aDotString;
  appendToDotString(tDotString,aNode->getData().getName());
  print("<dotstring>",tDotString);

  /*
   * Going to test the line that indicates the start of the primitive array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  bool tIsChecked = false;

  _Matcher.setMatchRegex(aNode->getData().getMatch());
  if (_Matcher.match(tLine))
  {
    if (aNode->getData().getCheckState() == Qt::Checked)
    {
      tIsChecked = true;
//      _LinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"primitive array node matched: "
        << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"primitive array node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  RecLine tRecLine;
  tRecLine.nodeKey = aNode->getData().getKey();
  tRecLine.nodeTestRegex = aNode->getData().getTest();
  tRecLine.nodeTestScope = 0;
  tRecLine.nodeIsChecked = tIsChecked;
  tRecLine.line = tLine;
  tRecLine.lineDotString = tDotString;
  tRecLine.lineTestResult = true;
  //INFO(sLogger,"<recline> " << tRecLine.scope << ": " << tRecLine.line);
  _RecLines.push_back(tRecLine);

  /*
   * Going to test the line that specifies the number of elements in the
   * primitive array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *_LineIter;
  _LineIter++;

  tIsChecked = true; // ok for now; add tree node?

  // TODO better to use exact match of leading tabs in match regex?
  _Matcher.setMatchRegex(".*array of len: (\\d+)");
  if (_Matcher.match(tLine))
  {
    DEBUG(sLogger,"array of len matched: " << tLine);
  }
  else
  {
    ERROR(sLogger,"array of len didn't match: <" << tLine);
    return false;
  }

  RecLine tArrayRecLine;
  tArrayRecLine.nodeKey = aNode->getData().getKey();
  tArrayRecLine.nodeTestRegex = aNode->getData().getTest();
  tArrayRecLine.nodeTestScope = 0;
  tArrayRecLine.nodeIsChecked = tIsChecked;
  tArrayRecLine.line = tLine;
  tArrayRecLine.lineDotString = tDotString;
  tArrayRecLine.lineTestResult = true;
  //INFO(sLogger,"<recline> " << tArrayRecLine.scope << ": " << tRecLine.line);
  _RecLines.push_back(tArrayRecLine);

  int tArrayLen = atoi(_Matcher.getWhat().c_str());

  /*
   * Process the array elements that follow per the array length.
   */
  for (int tIdx = 0; tIdx < tArrayLen; tIdx++)
  {
    std::stringstream tStream;
    tStream << tDotString << "[" << tIdx << "]";
    std::string tArrayDotString = tStream.str();

    bool tResult = processPrimitiveArrayLine(aNode,tArrayDotString,tIdx);
    if (tResult == false)
    {
      return false;
    }
  }

  return true;
}

//-----------------------------------------------------------------------------
// Processes a line that specifies an element of a primitive array.
// Directly consumes that single line. No other lines are indirectly consumed.
//-----------------------------------------------------------------------------
bool RecordProcessor::processPrimitiveArrayLine(
    FieldItem *aNode,std::string &aDotString,int aIdx)
{
//  std::string tDotString = aDotString;
//  appendToDotString(tDotString,aNode->getData().getName());
//  print("<dotstring>",tDotString);
  //print("<dotstring>",aDotString);

  /*
   * Going to test the single line that represents a primitive array item.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  bool tIsChecked = false;

  // TODO better to use exact match of leading tabs in match regex?
  std::string tPrimitiveArrayItemRegex(".*\\[\\d+\\]\\s+=\\s+(.*)");

  _Matcher.setMatchRegex(tPrimitiveArrayItemRegex);

  if (_Matcher.match(tLine))
  {
    if (aNode->getData().getCheckState() == Qt::Checked)
    {
      tIsChecked = true;
//      _LinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"primitive array element matched: " << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"Primitive array element <" << tLine << "> didn't match: <"
        << tPrimitiveArrayItemRegex << ">");
    return false;
  }

  std::string tScope;
  std::stringstream tStream;
  tStream << aIdx;
  tScope = aNode->getData().getKey() + "[" + tStream.str() + "]";

  RecLine tRecLine;
  tRecLine.nodeKey = tScope;
  tRecLine.nodeTestRegex = aNode->getData().getTest();
  tRecLine.nodeTestScope = 0;
  tRecLine.nodeIsChecked = tIsChecked;
  tRecLine.line = tLine;
  tRecLine.lineDotString = aDotString;
  tRecLine.lineTestResult = true;
  _RecLines.push_back(tRecLine);

  std::string tFieldValue = _Matcher.getWhat();
  if (!testForMatch(tFieldValue,aNode->getData().getTest()))
  {
    DEBUG(sLogger,"test FAILED");
    _TestResult = false;
  }

  return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool RecordProcessor::testForMatch(std::string &aValue,std::string &aTest)
{
  bool tMatches = true;

  if (aTest.length() > 0)
  {
    DEBUG(sLogger,"value,test: " << aValue << "," << aTest);

    boost::regex tMatchRegex(aTest);
    boost::match_results<std::string::const_iterator> what;

    if (!boost::regex_match(aValue,what,tMatchRegex))
    {
      tMatches = false;
    }
  }

  return tMatches;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool RecordProcessor::processChildren(FieldItem *aNode,std::string &aDotString)
{
  for (int tIdx = 0; tIdx < aNode->childCount(); tIdx++)
  {
    bool tResult = process(aNode->child(tIdx),aDotString);
    if (tResult == false)
    {
      ERROR(sLogger,"<process> traversing child failed");
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
// TODO is this really efficient, computing length every time?
//-----------------------------------------------------------------------------
void RecordProcessor::appendToDotString(
    std::string &aDotString,std::string &aSuffix)
{
  if (aDotString.length())
  {
    aDotString = aDotString + "." + aSuffix;
  }
  else
  {
    aDotString = aSuffix;
  }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RecordProcessor::print(const std::string &s1,const std::string &s2)
{
  std::cout << s1 << ": " << s2 << std::endl;
}
