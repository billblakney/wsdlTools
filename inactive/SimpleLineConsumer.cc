#include "SimpleLineConsumer.hh"

//-----------------------------------------------------------------------------
// @aStream Reference to input stream.
// @aLineBuffer Buffer to store line read from input stream.
// @aMatchRegex Pattern match to be used by consume().
// @aContinueOnMismatch Whether to continue reading lines on mismatch.
//-----------------------------------------------------------------------------
SimpleLineConsumer::SimpleLineConsumer(
    std::istream &aStream,std::string &aLineBuffer,std::string aMatchRegex,bool aContinueOnMismatch)
  : LineConsumer(aStream,aLineBuffer),
    _MatchRegex(aMatchRegex),
    _ContinueOnMismatch(aContinueOnMismatch)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SimpleLineConsumer::~SimpleLineConsumer()
{
}

//-----------------------------------------------------------------------------
// Attempt to consume a line of input.
// Reads a line from the input stream. If a match is found, does onMatch()
// processing, clears the line buffer, and returns 'true'.
// If a match is not found, behavior is determined by the "continue on
// mismatch" flag. If true, this method will continue reading lines from the
// input stream until a match is found, will clear the line buffer, and will
// return true. Otherwise, the line buffer is left in tact (holding the
// mismatched line), and 'false' is returned.
// Note that onMismatch() will be called on all mismatches, which may be
// multiple times ifthe "continue of mismatch" flag is true.
// @return True if the line was consumed. Otherwise, false.
//-----------------------------------------------------------------------------
bool SimpleLineConsumer::consume()
{
  bool tSucceeded = false;

  bool tContinue = true;
  while (tContinue)
  {
    std::getline(_Stream,_LineBuffer);

//std::cout << "testing [" << _LineBuffer << "] against [" << _MatchRegex << "]" << std::endl;
    boost::match_results<std::string::const_iterator> what;
    if (boost::regex_match(_LineBuffer,what,_MatchRegex))
    {
      onMatch();
      tSucceeded = true;
      tContinue = false;
    }
    else
    {
      onMismatch();
      if (!_ContinueOnMismatch)
      {
        tContinue = false;
      }
    }
  }
  if (tSucceeded)
  {
    _LineBuffer.clear();
  }
  return tSucceeded;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SimpleLineConsumer::onMatch()
{
//  std::cout << "match: " << _LineBuffer << std::endl;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SimpleLineConsumer::onMismatch()
{
//  std::cout << "MISMATCH: |" << _MatchString << "| didn't match |"
//      << _LineBuffer << "|" << std::endl;
}
