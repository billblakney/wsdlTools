#ifndef MESSAGECONFIGFILE_HH_
#define MESSAGECONFIGFILE_HH_

#include <QXmlStreamReader>
#include "DataStructModel.hh"

class MessageConfigFile
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

  MessageConfigFile(DataStructModel *aModel);
  virtual ~MessageConfigFile();
  void openConfiguration(QString aDir);
  void openConfiguration(QString aDir,QString aFilter);
  void saveConfiguration(QString aDir);

protected:
  QXmlStreamReader reader;
  DataStructModel *_DataStructModel;

  void readWsdlfilterElement();
  void readFieldElements();
  void updateMessageIsChecked(FieldItem *aItem,QString aIsChecked);
  void updateMessageTestScope(FieldItem *aItem,QString aTestScope);
  void updateMessageFormat(FieldItem *aItem,QString aFormat);
  void updateMessagePostfix(FieldItem *aItem,QString aPostfix);
  void readConfigElements();
  void skipUnknownElement();

  void writeWsdlfilterDocument(QXmlStreamWriter &aWriter);
  void writeConfigElements(QXmlStreamWriter &aWriter);
  void writeFieldElements(QXmlStreamWriter &aWriter);
};

#endif /* MESSAGECONFIGFILE_HH_ */
