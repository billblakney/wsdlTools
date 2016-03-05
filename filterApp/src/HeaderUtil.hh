#ifndef HeaderUtil_hh
#define HeaderUtil_hh

#include <QString>

#include "StructorBuilder.hh"

class HeaderUtil
{
public:
  static StructorBuilder *parseHeaderFile(QString aHeaderFile);
  HeaderUtil();
  virtual ~HeaderUtil();
};

#endif /* HeaderUtil_hh */
