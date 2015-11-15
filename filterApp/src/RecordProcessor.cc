#include "RecordProcessor.hh"

ccl::Logger RecordProcessor::sLogger("RecordProcessor");

RecordProcessor::RecordProcessor(
    FieldItem *aTopNode,std::vector<std::string> &aLinesIn)
  : TreeProcessor(aTopNode),
    _LinesIn(aLinesIn)
{
}

RecordProcessor::~RecordProcessor()
{
}

std::vector<std::string> &RecordProcessor::getOutLines()
{
  return _LinesOut;
}

void RecordProcessor::reset()
{
  _LineIter = _LinesIn.begin();
  _LinesOut.clear();
}

bool RecordProcessor::process()
{
  reset();
  return TreeProcessor::process();
}

bool RecordProcessor::processRootNode(FieldItem *aNode)
{
  INFO(sLogger,"<process> eRoot: " << aNode->getData().getName());
  return processChildren(aNode);
}

bool RecordProcessor::processStructNode(FieldItem *aNode)
{
  DEBUG(sLogger,"processing struct node: " << aNode->getData().getMatch());
  /*
   * The aSkipStructName tells us if this block of lines is started with a line
   * that specifies the struct name. If it is, get a reference to the line,
   * and increment the iterator. Then test for a match on the struct name line.
   */
  if (aNode->getData().getNodeType() != FieldItemData::eStructArray)
//TODO rm   if (!aSkipStructName)
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
    bool tResult = processStructNode(aNode/*TODO true*/);
    if (tResult == false)
    {
      return false;
    }
  }

  return true;
}

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
    DEBUG(sLogger,"primitive array node matched: " << aNode->getData().getMatch());
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

#if 0
bool RecordProcessor::processChildren(FieldItem *aNode)
{
  for (int tIdx = 0; tIdx < aNode->childCount(); tIdx++)
  {
    bool tResult = process(aNode->child(tIdx));
    if (tResult == false)
    {
      ERROR(sLogger,"<process> traversing child failed");
      return false;
    }
  }
  return true;
}
#endif
