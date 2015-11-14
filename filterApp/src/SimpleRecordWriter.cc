#include "SimpleRecordWriter.hh"

SimpleRecordWriter::SimpleRecordWriter()
{
  setMatchRegex(".*");
}

SimpleRecordWriter::~SimpleRecordWriter()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void SimpleRecordWriter::process(std::vector<std::string> &aRecordStrings)
{
std::cout << "in SimpleRecordWriter::process()" << std::endl;
  std::vector<std::string>::iterator tIt;
  for (tIt = aRecordStrings.begin(); tIt != aRecordStrings.end(); tIt++)
  {
    boost::match_results<std::string::const_iterator> what;

    if (boost::regex_match(*tIt,what,_MatchRegex))
    {
      std::cout << *tIt << std::endl;
    }
  }
}

void SimpleRecordWriter::setMatchRegex(std::string aMatchRegex)
{
  boost::regex tMatchRegex(aMatchRegex);
  _MatchRegex = tMatchRegex;
}
