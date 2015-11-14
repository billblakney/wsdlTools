#ifndef LINECONSUMER_HH_
#define LINECONSUMER_HH_

#include <iostream>
#include <string>

class LineConsumer
{
public:

  LineConsumer(std::istream &aStream,std::string &aLineBuffer);

  virtual ~LineConsumer();

  virtual bool consume() = 0;

protected:

  std::istream &_Stream;

  std::string  &_LineBuffer;
};

#endif /* LINECONSUMER_HH_ */
