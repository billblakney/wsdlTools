#ifndef FILEMANAGER_HH_
#define FILEMANAGER_HH_

#include <QXmlStreamReader>
#include "DataStructModel.hh"

class FileManager
{
public:

  FileManager(DataStructModel *aModel);
  virtual ~FileManager();
  void saveConfiguration();
  void openConfiguration();

protected:
  QXmlStreamReader reader;
  DataStructModel *_DataStructModel;

  void readWsdlfilterElement();
  void readFieldElements();
  void readConfigElements();
  void skipUnknownElement();
};

#endif /* FILEMANAGER_HH_ */
