#ifndef SIMPLELINEMATCHER_HH_
#define SIMPLELINEMATCHER_HH_

#include <boost/regex.hpp>
#include "LineMatcher.hh"

class SimpleLineMatcher: public LineMatcher
{
public:
  SimpleLineMatcher();
  SimpleLineMatcher(std::string aMatchRegex);
  virtual ~SimpleLineMatcher();
  virtual bool match(std::string &aStr);
  void setMatchRegex(std::string aMatchRegex);

  std::string getWhat() {return _What; } //TODO
protected:
  boost::regex _MatchRegex;

  std::string _What; //TODO
};

#endif /* SIMPLELINEMATCHER_HH_ */
