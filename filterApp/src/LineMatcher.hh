#ifndef LineMatcher_hh
#define LineMatcher_hh

#include <string>

class LineMatcher
{
public:

  LineMatcher();

  virtual ~LineMatcher();

  // Returns true if the line matches this objects criteria.
  virtual bool match(std::string &aStr) =0;
};

#endif /* LineMatcher_hh */
