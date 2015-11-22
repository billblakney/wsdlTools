#include <boost/regex.hpp>
#include "SimpleLineMatcher.hh"

SimpleLineMatcher::SimpleLineMatcher()
  : _MatchRegex(".+")
{
}

SimpleLineMatcher::SimpleLineMatcher(std::string aMatchRegex)
  : _MatchRegex(aMatchRegex)
{
}

SimpleLineMatcher::~SimpleLineMatcher()
{
}

bool SimpleLineMatcher::match(std::string &aStr)
{

    if (boost::regex_match(aStr,_What,_MatchRegex,boost::format_all))
    {
#if 0
if (what.size()>1)
{
//  std::cout << "WHAT[1]: " << what[1] << std::endl; //TODO
  _What = what[1];
}
#endif
      return true;
    }
    else
    {
      return false;
    }
}
void SimpleLineMatcher::setMatchRegex(std::string aMatchRegex)
{
  boost::regex tMatchRegex(aMatchRegex);
  _MatchRegex = tMatchRegex;
}
