#ifndef FIELDLINEMATCHER_HH_
#define FIELDLINEMATCHER_HH_

#include <boost/regex.hpp>
#include "LineMatcher.hh"

class FieldLineMatcher: public LineMatcher
{
public:
  FieldLineMatcher();
  virtual ~FieldLineMatcher();
  virtual bool match(std::string &aStr);
  bool hasFieldName();
  bool hasFieldValue();
  std::string getFieldName();
  std::string getFieldValue();
protected:
  std::string  _FieldName;
  std::string  _FieldValue;
  boost::regex _FieldRegex;
};

#endif /* FIELDLINEMATCHER_HH_ */
