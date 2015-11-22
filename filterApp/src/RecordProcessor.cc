#include <sstream>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "RecordProcessor.hh"
#include "SimpleLineMatcher.hh"

ccl::Logger RecordProcessor::sLogger("RecordProcessor");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RecordProcessor::RecordProcessor(
    FieldItem *aTopNode,std::vector<std::string> &aLinesIn)
  : _TopNode(aTopNode),
    _LinesIn(aLinesIn)
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

//  printRecordLines();

  applyTestResults();
  formatLines();
  setLinesOut();

  return tResult;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RecordProcessor::formatLines()
{
  std::vector<RecordLine>::iterator tIter;
  for (tIter = _RecordLines.begin(); tIter != _RecordLines.end(); tIter++)
  {
    if (tIter->nodeIsChecked && !tIter->resultLineExcluded)
    {
      if (tIter->nodeFormat != FieldItemData::eAsIs)
      {
        applyFormat(*tIter,tIter->nodeFormat);
      }
      /*
       * Add the postfix, i.e. line-ending.
       */
      boost::regex matchAll("(.*)");
      std::string tFormat("$1");
      tFormat += tIter->nodePostfix;
      tIter->line = boost::regex_replace(tIter->line,matchAll,tFormat,
          boost::format_first_only | boost::format_all);
    }
  }

}

//-----------------------------------------------------------------------------
// TODO at this point, we are changing the referenced line. This may mess
// things up if later more things are done with the original lines.
//-----------------------------------------------------------------------------
void RecordProcessor::applyFormat(RecordLine &aRecordLine,
    FieldItemData::Format aFormat)
{
  switch (aFormat) {
    case FieldItemData::eValue:
      break;
    case FieldItemData::eLongnameValue:
      aRecordLine.line = aRecordLine.lineDotString + ": " + aRecordLine.lineFieldValue;
      break;
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
// Look for record lines that have test conditions, and apply those to the
// include/exclude as appropriate.
// TODO rm root hard-coding
//-----------------------------------------------------------------------------
void RecordProcessor::applyTestResults()
{
  std::vector<RecordLine>::iterator tIter;
  for (tIter = _RecordLines.begin(); tIter != _RecordLines.end(); tIter++)
  {
    /*
     * If no test was specified for this line, go to the next line.
     */
    if (tIter->nodeTestRegex.length() < 1)
      continue;

    /*
     * If a line failed its test, and the test scope is root, then exclude
     * all lines and return.
     */
      if (tIter->resultPassedTest == false && !tIter->nodeTestScope.compare("root"))
      {
        excludeAllLines();
        return;
      }

    /*
     * Get the test base scope and whether it is an element scope.
     */
    std::string tBaseScope;
    bool tIsEltScope = false;

    parseScope(tIter->nodeTestScope,tBaseScope,tIsEltScope);

    /*
     * If this is a regular test scope (i.e. scope is not on array element test
     * scope), then if the test failed for this line, exclude all of the lines
     * that match the test scope.
     */
    if (!tIsEltScope)
    {
      if (tIter->resultPassedTest == false)
      {
        excludeAllLinesMatchingScope(tBaseScope + ".*"); //TODO mv dotstar to meth
      }
    }
    /*
     * If this is an element test scope, then if the test failed for this line,
     * exclude all of the lines in scope for this particular element.
     */
    else // is an Element test scope
    {
      if (tIter->resultPassedTest == false)
      {
        excludeAllLinesMatchingElementScope(tBaseScope,tIter->lineDotString);
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Parse a test scope to get the test scope base and determine if it is an
// array element scope or not. For example. if the test scope is
// "mailingList.contacts.Element", the base scope will be set to
// "mailingList.contacts", and the scope-is-array-element flag will be set
// to true.
// TODO return false, and propogate up?
//-----------------------------------------------------------------------------
void RecordProcessor::parseScope(
    std::string aTestScope,std::string &aBaseScope,bool &aIsEltScope)
{
  /*
   * Create a regex to capture the base scope and optional "array element"
   * keyword. If there is a match, then what[] will hold three elements:
   * 1) the whole test scope
   * 2) the base scope
   * 3) optional "array element" keyword
   */
  std::string tStr("(.*?)(\\.Element){0,1}");
  boost::regex tScopeRegex(tStr);

  boost::match_results<std::string::const_iterator> what;
  if (boost::regex_match(aTestScope,what,tScopeRegex))
  {
//    std::cout << "got match on " << aBaseScope << std::endl;
    aBaseScope = what[1];
    if (what[2].length())
    {
      aIsEltScope = true;
    }
    //      std::cout << "tBaseScope,isElt: " << tBaseScope << ","
    //          << tIsEltScope << std::endl;
  }
  else
  {
    std::cout << "ERROR: failure applying test results" << std::endl;
    return;
  }
}

//-----------------------------------------------------------------------------
// Load "lines out" from the processed record line set. To be loaded into the
// lines out list, the record line must have it's tree node checked and have
// its "is excluded" flag set to false. This method is called after the
// processing of the record lines is complete.
//-----------------------------------------------------------------------------
void RecordProcessor::setLinesOut()
{
  std::vector<RecordLine>::iterator tIter;
  for (tIter = _RecordLines.begin(); tIter != _RecordLines.end(); tIter++)
  {
    if (tIter->nodeIsChecked && !tIter->resultLineExcluded)
      _LinesOut.push_back(tIter->line);
  }
}

//-----------------------------------------------------------------------------
// Sets all record lines to the "is excluded" state. Also sets the "test
// result" flag to false.
//-----------------------------------------------------------------------------
void RecordProcessor::excludeAllLines()
{
  std::vector<RecordLine>::iterator tIter;
  for (tIter = _RecordLines.begin(); tIter != _RecordLines.end(); tIter++)
  {
    tIter->resultLineExcluded = true;
  }
}

//-----------------------------------------------------------------------------
// Excludes all lines that match a test scope. A match exists if the record
// line's dot string matches the test scope (up through the length of the
// test scope).
//-----------------------------------------------------------------------------
void RecordProcessor::excludeAllLinesMatchingScope(std::string aBaseScope)
{
//  std::cout << "calling excludeAllLinesMatchingScope: "
//      << aBaseScope  << std::endl;

  std::vector<RecordLine>::iterator tIter;
  for (tIter = _RecordLines.begin(); tIter != _RecordLines.end(); tIter++)
  {
    if (tIter->nodeIsChecked)
    {
      SimpleLineMatcher *tMatcher = new SimpleLineMatcher(aBaseScope);
      if (tMatcher->match(tIter->lineDotString))
      {
//        std::cout << "excluding: " << tIter->lineDotString << std::endl;
        tIter->resultLineExcluded = true;
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Excludes all lines that match a test scope for an array element.
//-----------------------------------------------------------------------------
void RecordProcessor::excludeAllLinesMatchingElementScope(
    std::string aBaseScope,std::string aLineDotString)
{
//  std::cout << "calling excludeAllLinesMatchingELEMENTScope:"
//      << aBaseScope << "," << aLineDotString << std::endl;

  /*
   * From the test scope string we'll need to build a string to test against
   * the record line dot strings. The scope string a concatenation of node
   * names separated by periods. The dot string for a record line is
   * similar, but has the complication that it may contain array indices
   * (at any node level that is an array). So to build a match string from
   * the test scope that will match a record line dot string, we'll need to
   * build a custom match string.
   *
   * To start with, get the list of node names from the test scope string.
   */
  std::vector<std::string> tComponents =
      getBaseScopeComponents(aBaseScope);

  /*
   * Build a string that we'll use to extract the portion of the line dot
   * string that can be used to exclude the other lines that match the same
   * arry element. It will be similar to the base test scope string, but it
   * will include an optional array indices at each level.
   * (The array indices may or may not even be applicable at the various
   * node levels, so just allow for them optionally to be there to extract
   * the desired match string.
   */
  std::string tStr("(");
  std::vector<std::string>::size_type tIdx;
  for (tIdx = 0; tIdx < tComponents.size(); tIdx++)
  {
    bool tIsLastPass = ((tIdx+1 == tComponents.size())? true:false);

    if (tIsLastPass)
    {
      tStr += tComponents[tIdx] + "[^\\.]*).*";
    }
    else
    {
      tStr += tComponents[tIdx] + "[^\\.]*\\.";
    }
  }
//  std::cout << "MATCH STRING: " << tStr << std::endl;

  /*
   * Use the match string constructed above to extract the match string that
   * will be used to exclude record lines.
   */
  std::string tExcludeMatch;

  boost::regex tScopeRegex(tStr);
  boost::match_results<std::string::const_iterator> what;
  if (boost::regex_match(aLineDotString,what,tScopeRegex))
  {
//    std::cout << "got match on " << aLineDotString << std::endl;

    tExcludeMatch = what[1] + ".*";
//    std::cout << "tExcludeMatch: " << tExcludeMatch << std::endl;
  }
  else
  {
//    std::cout << "ERROR: failure applying test results" << std::endl;
    return;
  }

  /*
   * Need to escape some of the characters to create the final match string.
   */
  boost::replace_all(tExcludeMatch,"\[","\\\[");
  boost::replace_all(tExcludeMatch,"]","\\]");

  /*
   * Finally, exclude all of the record lines that match the array element
   * that originally failed the test.
   */
  excludeAllLinesMatchingScope(tExcludeMatch);
}

//-----------------------------------------------------------------------------
// Tokenizes a base scope string to get its components.
//-----------------------------------------------------------------------------
std::vector<std::string> RecordProcessor::
getBaseScopeComponents(std::string aBaseScope)
{
  std::vector<std::string> tComponents;

  boost::char_separator<char> tSeparator(".");
  boost::tokenizer<boost::char_separator<char> > tTokens(aBaseScope,tSeparator);
  boost::tokenizer<boost::char_separator<char> >::iterator tIter;
  for (tIter = tTokens.begin(); tIter != tTokens.end(); tIter++)
  {
    tComponents.push_back(*tIter);
  }
  return tComponents;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool RecordProcessor::passedFilterTests()
{
  return (_LinesOut.size() > 0);
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
  _RecordLines.clear();
  _LineIter = _LinesIn.begin();
  _LinesOut.clear();
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
  else if (aNode->getData().getNodeType() == FieldItemData::ePrimitiveValue)
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

  bool tSkipFirstChild = false;

  /*
   * If the node being processed is a regular struct node, then the set of
   * lines to be processed will include a header line with the struct name.
   * If the node being processed is a struct array node, then the set of lines
   * for a struct does not include a header line.
   */
  if (aNode->getData().getNodeType() == FieldItemData::eStructArray)
  {
    tSkipFirstChild = true;
  }
  else
  {
    appendToDotString(tDotString,aNode->getData().getName());

    std::string &tLine = *_LineIter;
    _LineIter++;

    _Matcher.setMatchRegex(aNode->getData().getMatch());
    if (!_Matcher.match(tLine))
    {
      ERROR(sLogger,"struct node <" << tLine << "> didn't match: <"
          << aNode->getData().getMatch() << ">");
      return false;
    }

    RecordLine tRecordLine(aNode->getData(),tLine,tDotString,"");
    _RecordLines.push_back(tRecordLine);
  }

  return processChildren(aNode,tDotString,tSkipFirstChild);
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

  /*
   * Going to test the line that indicates the start of the struct array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  _Matcher.setMatchRegex(aNode->getData().getMatch());
  if (!_Matcher.match(tLine))
  {
    ERROR(sLogger,"struct array node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  RecordLine tRecordLine(aNode->getData(),tLine,tDotString,"");
  _RecordLines.push_back(tRecordLine);

  /*
   * Going to test the line that specifies the number of elements in the
   * struct array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *_LineIter;
  _LineIter++;

  FieldItem *aFirstChildNode = aNode->child(0);
  std::string tSizeDotString = tDotString + "_size";

  _Matcher.setMatchRegex(aFirstChildNode->getData().getMatch());
  if (_Matcher.match(tLine))
  {
    DEBUG(sLogger,"array of len matched: " << tLine);
  }
  else
  {
    ERROR(sLogger,"array of len didn't match: <" << tLine);
    return false;
  }

  bool tTestResult = true;

  std::string tFieldValue = _Matcher.getWhat();
  if (!testForMatch(tFieldValue,aFirstChildNode->getData().getTest()))
  {
    DEBUG(sLogger,"test FAILED for " << aFirstChildNode->getData().getName());
    tTestResult = false;
  }

  RecordLine tArrayRecordLine(aFirstChildNode->getData(),tLine,tSizeDotString,
      tFieldValue,tTestResult);
  _RecordLines.push_back(tArrayRecordLine);

  int tArrayLen = atoi(_Matcher.getWhat().c_str());

  /*
   * Process the structures that follow per the array length.
   */
  for (int tIdx = 0; tIdx < tArrayLen; tIdx++) //TODO process first "length" node
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

  /*
   * Going to test the single line that represents a primitive field.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  _Matcher.setMatchRegex(aNode->getData().getMatch());
  if (!_Matcher.match(tLine))
  {
    ERROR(sLogger,"Primitive node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  bool tTestResult = true;

  std::string tFieldValue = _Matcher.getWhat();
  if (!testForMatch(tFieldValue,aNode->getData().getTest()))
  {
    DEBUG(sLogger,"test FAILED for " << aNode->getData().getName());
    tTestResult = false;
  }

  RecordLine tRecordLine(aNode->getData(),tLine,tDotString,tFieldValue,
      tTestResult);
  _RecordLines.push_back(tRecordLine);

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

  /*
   * Going to test the line that indicates the start of the primitive array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  _Matcher.setMatchRegex(aNode->getData().getMatch());
  if (!_Matcher.match(tLine))
  {
    ERROR(sLogger,"primitive array node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  RecordLine tRecordLine(aNode->getData(),tLine,tDotString,"",true);
  _RecordLines.push_back(tRecordLine);

  /*
   * Going to test the line that specifies the number of elements in the
   * primitive array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *_LineIter;
  _LineIter++;

  FieldItem *aFirstChildNode = aNode->child(0);
  std::string tSizeDotString = tDotString + "_size";

  // TODO better to use exact match of leading tabs in match regex?
  _Matcher.setMatchRegex(aFirstChildNode->getData().getMatch());
  if (_Matcher.match(tLine))
  {
    DEBUG(sLogger,"array of len matched: " << tLine);
  }
  else
  {
    ERROR(sLogger,"array of len didn't match: <" << tLine);
    return false;
  }

  bool tTestResult = true;

  std::string tFieldValue = _Matcher.getWhat();
  if (!testForMatch(tFieldValue,aFirstChildNode->getData().getTest()))
  {
    DEBUG(sLogger,"test FAILED for " << aFirstChildNode->getData().getName());
    tTestResult = false;
  }

  RecordLine tArrayRecordLine(aFirstChildNode->getData(),tLine,tSizeDotString,
      tFieldValue,tTestResult);
  _RecordLines.push_back(tArrayRecordLine);

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

  // TODO better to use exact match of leading tabs in match regex?
  std::string tPrimitiveArrayItemRegex(".*\\[\\d+\\]\\s+=\\s+(.*)");

  _Matcher.setMatchRegex(tPrimitiveArrayItemRegex);

  if (!_Matcher.match(tLine))
  {
    ERROR(sLogger,"Primitive array element <" << tLine << "> didn't match: <"
        << tPrimitiveArrayItemRegex << ">");
    return false;
  }

  bool tTestResult = true;

  std::string tFieldValue = _Matcher.getWhat();
  if (!testForMatch(tFieldValue,aNode->getData().getTest()))
  {
    DEBUG(sLogger,"test FAILED");
    tTestResult = false;
  }

  std::string tScope;
  std::stringstream tStream;
  tStream << aIdx;
  tScope = aNode->getData().getKey() + "[" + tStream.str() + "]";

  RecordLine tRecordLine;
  tRecordLine.nodeKey = tScope;
  tRecordLine.nodeTestRegex = aNode->getData().getTest();
  tRecordLine.nodeTestScope = "";
  tRecordLine.nodeIsChecked = aNode->getData().isChecked();
  tRecordLine.line = tLine;
  tRecordLine.lineDotString = aDotString;
  tRecordLine.lineFieldValue = tFieldValue;
  tRecordLine.resultPassedTest = tTestResult;
  tRecordLine.resultLineExcluded = false;
  _RecordLines.push_back(tRecordLine);

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
// TODO comment on aSkipFirstChild
//-----------------------------------------------------------------------------
bool RecordProcessor::processChildren(FieldItem *aNode,std::string &aDotString,
    bool aSkipFirstChild)
{
  int tIdx = (aSkipFirstChild ? 1 : 0);

  for (; tIdx < aNode->childCount(); tIdx++)
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
void RecordProcessor::printRecordLines()
{
  char buff[1000];
  sprintf(buff,"%3s %-20s %-10s %-10s: %5s, %-30s, %-30s",
      "nCk","nKey","nTstRgx","nTScp","lTRes","lDotStr","line");
  std::cout << buff << std::endl;

  /*
   * Print the record lines.
   */
  std::vector<RecordLine>::iterator tIter;
  for (tIter = _RecordLines.begin(); tIter != _RecordLines.end(); tIter++)
  {
    sprintf(buff,"%3d %-20s %-10s %-10s: %5d, %-30s, %-30s",
        tIter->nodeIsChecked,
        tIter->nodeKey.c_str(),
        tIter->nodeTestRegex.c_str(),
        tIter->nodeTestScope.c_str(),
        tIter->resultPassedTest,
        tIter->lineDotString.c_str(),
        tIter->line.c_str());
    std::cout << buff << std::endl;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void RecordProcessor::print(const std::string &s1,const std::string &s2)
{
  std::cout << s1 << ": " << s2 << std::endl;
}
