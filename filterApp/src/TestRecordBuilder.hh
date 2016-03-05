#ifndef TestRecordBuilder_hh
#define TestRecordBuilder_hh

#include <string>
#include <QString>

class TestRecordBuilder
{
public:
  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  TestRecordBuilder(QString aMsgId,QString aStructName,QString aHeaderPath);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  virtual ~TestRecordBuilder();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::string getTestRecord();

protected:
  QString _MsgId;
  QString _StructName;
  QString _HeaderPath;
};

#endif /* TestRecordBuilder_hh */
