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

  enum DelimitMode {eAllRecords, eOutputRecords, eNoRecords};

  enum OutputMode {eNormal, eBypass, eFreezeDrop};

  StreamReader(RecordProcessor *aRecordProcessor);
  virtual ~StreamReader();
  void setRecordWriter(RecordWriter *aWriter);

  RecordProcessor *getRecordProcessor();

  OutputMode getOutputMode();
  void setOutputMode(OutputMode aOutputMode);

  bool inDelimitRecordsMode();
  void setDelimitMode(DelimitMode aDelimitRecordsMode);

signals:

  void structNameAvailable(QString aMsgId,QString aStructName);

public slots:

  void onDataStructModelAvailable(void * aDataStructModel);

  void onOutputModeAction(QAction* aAction);

  void onDelimitModeAction(QAction* aAction);


  void run();

protected:
  static ccl::Logger sLogger;

  int                          _NumRecordsTotal;
  int                          _NumRecordsOutput;
  RecordProcessor             *_RecordProcessor;
  std::vector<RecordWriter *>  _Writers;
  DataStructModel             *_DataStructModel;
  DelimitMode                  _DelimitMode;
  OutputMode                   _OutputMode;
  bool                         _InDelimitRecordsMode;

  QMutex _Mutex;

  void readForStructName(std::string &aMsgId,std::string &aStructName);
  void waitUntilDataModelAvailable();
  void readAndProcessStructLines();

  void setDataStructModel(DataStructModel *aModel);
};

#endif /* STREAMREADER_HH_ */
