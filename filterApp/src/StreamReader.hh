#ifndef StreamReader_hh
#define StreamReader_hh

#include <iostream>
#include <QApplication>
#include <QMutex>
#include <QThread>
#include "DataStructModel.hh"
#include "Logger.hh"
#include "RecordProcessor.hh"
#include "RecordWriter.hh"

typedef void (*RecordProcessedCallbackType)();


class StreamReader: public QThread
{
  Q_OBJECT;

public:

  enum DelimitMode {
    eAllRecords,
    eOutputRecords,
    eNoRecords
  };
  static DelimitMode getDelimitMode(QString aDelimitMode);
  static QString getDelimitModeString(DelimitMode aDelimitMode);
  static QStringList getDelimitModeStringList();

  enum OperateMode {
    eGo,
    eStop,
    eBypass
  };
  static OperateMode getOperateMode(QString aOperateMode);
  static QString getOperateModeString(OperateMode aOperateMode);
  static QStringList getOperateModeStringList();

  StreamReader(std::istream &aStream,RecordProcessor *aRecordProcessor,
      RecordProcessedCallbackType aRecordProcessedCallback,
      OperateMode aOperateMode = eGo,
      DelimitMode aDelimitMode = eOutputRecords);
  virtual ~StreamReader();
  void setRecordWriter(RecordWriter *aWriter);

  RecordProcessor *getRecordProcessor();

  OperateMode getOperateMode();
  void setOperateMode(OperateMode aOperateMode);

  DelimitMode getDelimitMode();
  void setDelimitMode(DelimitMode aDelimitRecordsMode);

signals:

  void structNameAvailable(QString aMsgId,QString aStructName);

public slots:

  void onDataStructModelAvailable(void * aDataStructModel);

  void onOperateModeAction(QAction* aAction);

  void onEnterSpaceAction();

  void onDelimitModeAction(QAction* aAction);


  void run();

protected:
  static ccl::Logger sLogger;

  static QStringList _OperateModeNames;
  static QStringList operateModeNames();

  static QStringList _DelimitModeNames;
  static QStringList delimitModeNames();

  std::istream                &_InStream;
  RecordProcessor             *_RecordProcessor;
  RecordProcessedCallbackType  _RecordProcessedCallback;
  int                          _NumRecordsTotal;
  int                          _NumRecordsOutput;
  std::vector<RecordWriter *>  _Writers;
  DataStructModel             *_DataStructModel;
  OperateMode                  _OperateMode;
  DelimitMode                  _DelimitMode;

  QMutex _Mutex;

  void readForStructName(std::string &aMsgId,std::string &aStructName);
  void waitUntilDataModelAvailable();
  void readAndProcessStructLines();

  void setDataStructModel(DataStructModel *aModel);
};

#endif /* StreamReader_hh */
