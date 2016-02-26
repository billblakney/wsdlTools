#ifndef FieldSpec_hh
#define FieldSpec_hh

#include <QString>

class FieldSpec
{
public:
  FieldSpec();
  virtual ~FieldSpec();

  QString key;
  QString isChecked;
  QString testScope;
  QString format;
  QString postfix;
};

#endif /* FieldSpec_hh */
