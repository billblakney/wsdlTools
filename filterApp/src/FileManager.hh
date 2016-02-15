#ifndef FILEMANAGER_HH_
#define FILEMANAGER_HH_

#include <QXmlStreamReader>
#include "DataStructModel.hh"

class FileManager
{
public:
  static std::string kTag;

  static const char *kWsdlfilterTag;
  static const char *kConfigTag;
  static const char *kOperateModeTag;
  static const char *kDelimitModeTag;
  static const char *kFieldsTag;
  static const char *kFieldTag;
  static const char *kAttrKeyTag;
  static const char *kAttrCheckedTag;
  static const char *kValueChecked;
  static const char *kValueNotChecked;

  FileManager(DataStructModel *aModel);
  virtual ~FileManager();
  void openConfiguration();
  void saveConfiguration();

protected:
  QXmlStreamReader reader;
  DataStructModel *_DataStructModel;

  void readWsdlfilterElement();
  void readFieldElements();
  void readConfigElements();
  void skipUnknownElement();

  void writeWsdlfilterDocument(QXmlStreamWriter &aWriter);
  void writeConfigElements(QXmlStreamWriter &aWriter);
  void writeFieldElements(QXmlStreamWriter &aWriter);
};

#endif /* FILEMANAGER_HH_ */
