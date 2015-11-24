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
  StreamReader(RecordProcessor *aRecordProcessor);
#if 0
//  StreamReader(DataStructModel *aModel,RecordWriter *aWriter);
#endif
  virtual ~StreamReader();
  void setRecordWriter(RecordWriter *aWriter);

  RecordProcessor *getRecordProcessor();

  bool inBypassMode();
  void setInBypassMode(bool aInBypassMode);

  bool inDelimitRecordsMode();
  void setDelimitRecordsMode(bool aDelimitRecordsMode);

signals:

  void structNameAvailable(QString aStructName);

public slots:

  void onDataStructModelAvailable(void * aDataStructModel);

  void onBypassToggle(bool aIsChecked);

  void onDelimitRecordsToggle(bool aIsChecked);

  void run();

protected:
  static ccl::Logger sLogger;

  RecordProcessor             *_RecordProcessor;
  std::vector<RecordWriter *>  _Writers;
  DataStructModel             *_DataStructModel;
  bool                         _InBypassMode;
  bool                         _InDelimitRecordsMode;

  QMutex _Mutex;

  void readForStructName(std::string &aMsgId,std::string &aStructName);
  void waitUntilDataModelAvailable();
  void readAndProcessStructLines();

  void setDataStructModel(DataStructModel *aModel);
};

#endif /* STREAMREADER_HH_ */
