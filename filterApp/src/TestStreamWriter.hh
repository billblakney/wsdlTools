#ifndef TestStreamWriter_hh
#define TestStreamWriter_hh

#include <iostream>
#include <sstream>
#include <QThread>

class TestStreamWriter: public QThread
{
  Q_OBJECT;

public:

  TestStreamWriter();

  ~TestStreamWriter();

  void run();

  std::stringstream &getTestStream();

protected:

  std::stringstream _TestStream;
};

#endif /* TestStreamWriter_hh */
