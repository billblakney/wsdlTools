#ifndef LINEMATCHER_HH_
#define LINEMATCHER_HH_

#include <string>

class LineMatcher
{
public:

  LineMatcher();

  virtual ~LineMatcher();

  // Returns true if the line matches this objects criteria.
  virtual bool match(std::string &aStr) =0;
};

#endif /* LINEMATCHER_HH_ */
