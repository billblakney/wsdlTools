#ifndef APPCONFIGFILE_HH_
#define APPCONFIGFILE_HH_

#include <map>
#include <QXmlStreamReader>
#include "MessageSpec.hh"

class AppConfigFile
{
public:
  static std::string kTag;

  static const char *kWsdlFilterConfigTag;
  static const char *kDefaultsTag;
  static const char *kDefaultOperateModeTag;
  static const char *kDefaultDelimitModeTag;
  static const char *kMessagesTag;
  static const char *kMessageTag;
  static const char *kAttrIdTag;
  static const char *kAttrStructTag;
  static const char *kAttrHeaderTag;

  AppConfigFile(QString aFilename);
  virtual ~AppConfigFile();
  void openConfiguration();
#if 0
  void saveConfiguration();
#endif
  std::map<QString,MessageSpec> &messageMap();

protected:
  QXmlStreamReader reader;
  QString _ConfigFilename;
  std::map<QString,MessageSpec> _MessageMap;

  void readWsdlFilterConfigElement();
  void readDefaultsElements();
  void readMessagesElements();
  void skipUnknownElement();

#if 0
  void writeWsdlFilterConfigDocument(QXmlStreamWriter &aWriter);
  void writeDefaultsElements(QXmlStreamWriter &aWriter);
  void writeMessagesElements(QXmlStreamWriter &aWriter);
#endif
};

#endif /* APPCONFIGFILE_HH_ */
