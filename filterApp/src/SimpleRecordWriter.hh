#ifndef SIMPLERECORDWRITER_HH_
#define SIMPLERECORDWRITER_HH_

#include "RecordWriter.hh"
#include <boost/regex.hpp>

class SimpleRecordWriter: public RecordWriter
{
public:
  SimpleRecordWriter();
  virtual ~SimpleRecordWriter();
  virtual void process(std::vector<std::string> &aRecordStrings);
  void setMatchRegex(std::string aMatchRegex);
protected:
  boost::regex _MatchRegex;
};

#endif /* SIMPLERECORDWRITER_HH_ */
