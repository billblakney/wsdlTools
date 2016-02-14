#ifndef FILEMANAGER_HH_
#define FILEMANAGER_HH_

#include <QXmlStreamReader>
#include "DataStructModel.hh"

class FileManager
{
public:
  FileManager();
  virtual ~FileManager();
  static void saveConfiguration(DataStructModel *aModel);
  static void openConfiguration(DataStructModel *aModel);
static void skipUnknownElement();

static void readWsdlfilterElement();
static void readFieldElements();
static void readConfigElements();

static void readFieldElement(); // TODO not used

static QXmlStreamReader reader;
};

#endif /* FILEMANAGER_HH_ */
