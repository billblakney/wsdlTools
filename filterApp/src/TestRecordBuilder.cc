#include <iostream>

#include "DataStructModel.hh"
#include "HeaderUtil.hh"
#include "TestRecordBuilder.hh"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TestRecordBuilder::TestRecordBuilder(QString aMsgId,QString aStructName,
    QString aHeaderPath)
  : _MsgId(aMsgId),
    _StructName(aStructName),
    _HeaderPath(aHeaderPath)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
TestRecordBuilder::~TestRecordBuilder()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string TestRecordBuilder::getTestRecord()
{
  // Parse the header file to get _StructBuilder.
  StructorBuilder *tStructorBuilder =
      HeaderUtil::parseHeaderFile(_HeaderPath);

  Structure *tStructure =
      tStructorBuilder->getStructure(_StructName.toStdString());

  if (!tStructure)
  {
    std::cerr << "ERROR: couldn't find struct "
              << qPrintable(_StructName) << std::endl;
    exit(0);
  }

  /*
   * Create the data structure model for the specified data structure.
   */
  DataStructModel *tModel = new DataStructModel(tStructure,tStructorBuilder);
  std::string tRecord =
      tModel->getTestRecord(_MsgId.toStdString(),_StructName.toStdString());

  return tRecord;
}
