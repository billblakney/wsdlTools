#include "RecordProcessor.hh"

ccl::Logger RecordProcessor::sLogger("RecordProcessor");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RecordProcessor::RecordProcessor(
    FieldItem *aTopNode,std::vector<std::string> &aLinesIn)
  : TreeProcessor(aTopNode),
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
  return TreeProcessor::process();
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
  _LineIter = _LinesIn.begin();
  _LinesOut.clear();
}

//-----------------------------------------------------------------------------
// Processes the set of lines for record.
// Doesn't directly consume any lines. Indirectly consumes the set of lines
// for the entire record represented by the root node.
//-----------------------------------------------------------------------------
bool RecordProcessor::processRootNode(FieldItem *aNode)
{
  DEBUG(sLogger,"processing eRoot: " << aNode->getData().getName());

  return processChildren(aNode);
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
bool RecordProcessor::processStructNode(FieldItem *aNode)
{
  DEBUG(sLogger,"processing eStruct: " << aNode->getData().getMatch());

  /*
   * If the node being processed is a regular struct node, then the set of
   * lines to be processed will include a header line with the struct name.
   * If the node being processed is a struct array node, then the set of lines
   * for a struct does not include a header line.
   */
  if (aNode->getData().getNodeType() != FieldItemData::eStructArray)
  {
    std::string &tLine = *_LineIter;
    _LineIter++;

    _Matcher.setMatchRegex(aNode->getData().getMatch());
    if (_Matcher.match(tLine))
    {
      DEBUG(sLogger,"struct node matched: " << aNode->getData().getMatch());
    }
    else
    {
      ERROR(sLogger,"struct node <" << tLine << "> didn't match: <"
          << aNode->getData().getMatch() << ">");
      return false;
    }
  }
  return processChildren(aNode);
}

//-----------------------------------------------------------------------------
// Processes the set of lines for a struct array node.
// Directly consumes two lines: one specifying the type of the array elements,
// and one specifying the array length. Indirectly consumes all of the lines
// for all of the struct array elements.
//-----------------------------------------------------------------------------
bool RecordProcessor::processStructArrayNode(FieldItem *aNode)
{
  DEBUG(sLogger,"Looking for struct array with match: " << aNode->getData().getMatch());

  /*
   * Going to test the line that indicates the start of the struct array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  _Matcher.setMatchRegex(aNode->getData().getMatch());
  if (_Matcher.match(tLine))
  {
    DEBUG(sLogger,"struct array node matched: " << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"struct array node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  /*
   * Going to test the line that specifies the number of elements in the
   * struct array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *_LineIter;
  _LineIter++;

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

  int tArrayLen = atoi(_Matcher.getWhat().c_str());

  /*
   * Process the structures that follow per the array length.
   */
  for (int tIdx = 0; tIdx < tArrayLen; tIdx++)
  {
    bool tResult = processStructNode(aNode);
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
bool RecordProcessor::processPrimitiveNode(FieldItem *aNode)
{
  /*
   * Going to test the single line that represents a primitive field.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  _Matcher.setMatchRegex(aNode->getData().getMatch());

  if (_Matcher.match(tLine))
  {
    if (aNode->getData().getCheckState() == Qt::Checked)
    {
      _LinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"primitive node matched: " << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"Primitive node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
// Processes the set of lines for a primitive array node.
// Directly consumes two lines: one specifying the name of the array and one
// specifying the array length. Indirectly consumes all of the lines for the
// entire primitive array.
//-----------------------------------------------------------------------------
bool RecordProcessor::processPrimitiveArrayNode(FieldItem *aNode)
{
  DEBUG(sLogger,"Looking for primitive array with match: "
      << aNode->getData().getMatch());

  /*
   * Going to test the line that indicates the start of the primitive array.
   * So get a reference to it, and increment the iterator.
   * And then test it for a match.
   */
  std::string &tLine = *_LineIter;
  _LineIter++;

  _Matcher.setMatchRegex(aNode->getData().getMatch());
  if (_Matcher.match(tLine))
  {
    DEBUG(sLogger,"primitive array node matched: "
        << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"primitive array node <" << tLine << "> didn't match: <"
        << aNode->getData().getMatch() << ">");
    return false;
  }

  /*
   * Going to test the line that specifies the number of elements in the
   * primitive array. So get a reference to it, and increment the iterator.
   * And then test for a match with the array length specification, and
   * get the array length.
   */
  tLine = *_LineIter;
  _LineIter++;

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

  int tArrayLen = atoi(_Matcher.getWhat().c_str());

  /*
   * Process the array elements that follow per the array length.
   */
  for (int tIdx = 0; tIdx < tArrayLen; tIdx++)
  {
    bool tResult = processPrimitiveArrayLine(aNode);
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
bool RecordProcessor::processPrimitiveArrayLine(FieldItem *aNode)
{
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

  if (_Matcher.match(tLine))
  {
    if (aNode->getData().getCheckState() == Qt::Checked)
    {
      _LinesOut.push_back(tLine);
    }
    DEBUG(sLogger,"primitive array element matched: " << aNode->getData().getMatch());
  }
  else
  {
    ERROR(sLogger,"Primitive array element <" << tLine << "> didn't match: <"
        << tPrimitiveArrayItemRegex << ">");
    return false;
  }

  return true;
}
