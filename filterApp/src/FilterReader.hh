#ifndef FilterReader_hh
#define FilterReader_hh

#include <QXmlStreamReader>
#include "DataStructModel.hh"
#include "FilterSpec.hh"

class FilterReader
{
public:
  static std::string kTag;

  static const char *kWsdlfilterconfigTag;
  static const char *kDefaultsTag;
  static const char *kOperateModeTag;
  static const char *kDelimitModeTag;
  static const char *kFieldsTag;
  static const char *kFieldTag;
  static const char *kAttrKeyTag;
  static const char *kAttrCheckedTag;
  static const char *kAttrFilterScopeTag;
  static const char *kAttrFormatTag;
  static const char *kAttrPostfixTag;
  static const char *kValueChecked;
  static const char *kValueNotChecked;

  FilterReader();
  virtual ~FilterReader();
  FilterSpec openFilter(QString aDir);
  FilterSpec openFilter(QString aDir,QString aFilterSpec);
  void saveFilter(QString aDir,DataStructModel *aDataStructModel,
      StreamReader *aStreamReader);

protected:
  QXmlStreamReader reader;

  void readWsdlfilterElement(FilterSpec &aFilterSpec);
  void readFieldElements(FilterSpec &aFilterSpec);
  void readConfigElements(FilterSpec &aFilterSpec);
  void skipUnknownElement();

  void writeWsdlfilterDocument(
      QXmlStreamWriter &aWriter,DataStructModel *aDataStructModel,
      StreamReader *aStreamReader);
  void writeConfigElements(QXmlStreamWriter &aWriter,
      StreamReader *aStreamReader);
  void writeFieldElements(
      QXmlStreamWriter &aWriter,DataStructModel *aDataStructModel);
};

#endif /* FilterReader_hh */
