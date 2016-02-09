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

  enum OutputMode {eNormal, eBypass, eFreezeDrop};

  StreamReader(RecordProcessor *aRecordProcessor);
#if 0
//  StreamReader(DataStructModel *aModel,RecordWriter *aWriter);
#endif
  virtual ~StreamReader();
  void setRecordWriter(RecordWriter *aWriter);

  RecordProcessor *getRecordProcessor();

  OutputMode getOutputMode();
  void setOutputMode(OutputMode aOutputMode);

  bool inDelimitRecordsMode();
  void setDelimitRecordsMode(bool aDelimitRecordsMode);

signals:

  void structNameAvailable(QString aStructName);

public slots:

  void onDataStructModelAvailable(void * aDataStructModel);

  void onOutputModeSelected(int aOutputMode);

  void onDelimitRecordsToggle(bool aIsChecked);


  void run();

protected:
  static ccl::Logger sLogger;

  int                          _RecordCount;
  RecordProcessor             *_RecordProcessor;
  std::vector<RecordWriter *>  _Writers;
  DataStructModel             *_DataStructModel;
  OutputMode                   _OutputMode;
  bool                         _InDelimitRecordsMode;

  QMutex _Mutex;

  void readForStructName(std::string &aMsgId,std::string &aStructName);
  void waitUntilDataModelAvailable();
  void readAndProcessStructLines();

  void setDataStructModel(DataStructModel *aModel);
};

#endif /* STREAMREADER_HH_ */
