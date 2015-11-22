#ifndef STREAMREADER_HH_
#define STREAMREADER_HH_

#include <QApplication>
#include <QMutex>
#include <QThread>
#include "DataStructModel.hh"
#include "Logger.hh"
#include "RecordWriter.hh"

class StreamReader: public QThread
{
  Q_OBJECT;
public:
  StreamReader();
  StreamReader(DataStructModel *aModel,RecordWriter *aWriter);
  virtual ~StreamReader();
  void setRecordWriter(RecordWriter *aWriter);
  void setDataStructModel(DataStructModel *aModel);

signals:

  void structNameAvailable(QString aStructName);

public slots:

  void run();

  void onDataStructModelAvailable(void * aDataStructModel);

protected:
  static ccl::Logger sLogger;

  std::vector<RecordWriter *> _Writers;
  DataStructModel *_DataStructModel;
  QMutex _Mutex;

  void readForStructName(std::string &aMsgId,std::string &aStructName);
};

#endif /* STREAMREADER_HH_ */
