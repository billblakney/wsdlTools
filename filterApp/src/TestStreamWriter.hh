#ifndef TestStreamWriter_hh
#define TestStreamWriter_hh

#include <iostream>
#include <sstream>

class TestStreamWriter
{
public:

  TestStreamWriter();

  ~TestStreamWriter();

  std::stringstream &getTestStream();

protected:

  std::stringstream _TestStream;
};

#endif /* TestStreamWriter_hh */
