#ifndef APPCONFIGFILE_HH_
#define APPCONFIGFILE_HH_

#include <map>
#include <QXmlStreamReader>
#include "AppConfig.hh"
#include "MessageSpec.hh"

class AppConfigFile
{
public:
  static std::string kTag;

  static const char *kWsdlFilterConfigTag;
  static const char *kConfigTag;
  static const char *kHeadersDirTag;
  static const char *kDefaultHeaderTag;
  static const char *kDefaultFiltersDirTag;
  static const char *kCustomFiltersDirTag;
  static const char *kDefaultOperateModeTag;
  static const char *kDefaultDelimitModeTag;
  static const char *kMessagesTag;
  static const char *kMessageTag;
  static const char *kAttrIdTag;
  static const char *kAttrStructTag;
  static const char *kAttrHeaderTag;
  static const char *kAttrDefaultFilterTag;

  AppConfigFile(QString aFilename);
  virtual ~AppConfigFile();
  void openConfiguration();
#if 0
  void saveConfiguration();
#endif
  AppConfig &appConfig();
  MessageSpecMap &messageMap();

protected:
  QXmlStreamReader reader;
  QString _ConfigFilename;
  AppConfig _AppConfig;
  MessageSpecMap _MessageMap;

  void readWsdlFilterConfigElement();
  void readDefaultsElements();
  void readMessagesElements();
  void readMessageElements(MessageSpec &aMessageSpec);
  void skipUnknownElement();

#if 0
  void writeWsdlFilterConfigDocument(QXmlStreamWriter &aWriter);
  void writeDefaultsElements(QXmlStreamWriter &aWriter);
  void writeMessagesElements(QXmlStreamWriter &aWriter);
#endif
};

#endif /* APPCONFIGFILE_HH_ */
