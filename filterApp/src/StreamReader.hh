#ifndef STREAMREADER_HH_
#define STREAMREADER_HH_

#include <QApplication>
#include <QMutex>
#include <QThread>
#include "DataStructModel.hh"
#include "Logger.hh"
#include "RecordProcessor.hh"
#include "RecordWriter.hh"

class StreamReader: public QThread
{
  Q_OBJECT;
public:
  StreamReader();
  StreamReader(DataStructModel *aModel,RecordWriter *aWriter);
  virtual ~StreamReader();
  void setRecordWriter(RecordWriter *aWriter);
  bool inBypassMode();
  void setInBypassMode(bool aInBypassMode);

signals:

  void structNameAvailable(QString aStructName);

public slots:

  void onDataStructModelAvailable(void * aDataStructModel);

  void onBypassCheckBoxToggle(bool aIsChecked);

  void run();

protected:
  static ccl::Logger sLogger;

  vector<std::string>          _StructLines;
  std::vector<RecordWriter *>  _Writers;
  DataStructModel             *_DataStructModel;
  RecordProcessor             *_RecordProcessor;
  bool                         _InBypassMode;

  QMutex _Mutex;

  void readForStructName(std::string &aMsgId,std::string &aStructName);
  void waitUntilDataModelAvailable();
  void readAndProcessStructLines();

  void setDataStructModel(DataStructModel *aModel);
};

#endif /* STREAMREADER_HH_ */
