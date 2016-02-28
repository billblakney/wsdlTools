#ifndef RecordWriter_hh
#define RecordWriter_hh

#include <string>
#include <vector>
#include <LineMatcher.hh>

class RecordWriter
{
public:
  RecordWriter();
  virtual ~RecordWriter();
  virtual void process(std::vector<std::string> &aRecordStrings) = 0;
};

#endif /* RecordWriter_hh */
