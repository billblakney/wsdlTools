#ifndef SimpleRecordWriter_hh
#define SimpleRecordWriter_hh

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

#endif /* SimpleRecordWriter_hh */
