#include "LineConsumer.hh"

LineConsumer::LineConsumer(std::istream &aStream,std::string &aLineBuffer)
  : _Stream(aStream),
    _LineBuffer(aLineBuffer)
{
}

LineConsumer::~LineConsumer()
{
}
