#ifndef RecordProcessor_hh
#define RecordProcessor_hh

#include <QAction>
#include <QMutex>
#include <QWidget>
#include "FieldItem.hh"
#include "RecordLine.hh"
#include "SimpleLineMatcher.hh"
#include "Logger.hh"

/*
 * Processes records of input read by a StreamReader.
 * Can be instantiated once, and re-used. For example, after instantiation
 * a typical use-case sequence would be:
 *    RecordProcessor *p = new RecordProcessor(theTopNode,theLinesToBeProcessed);
 *    while (1){
 *      p->configure(theTopNode);
 *      theLinesToBeProcessed.clear();
 *      theLinesToBeProcessed = readNextLines();
 *      p->process(&theLinesToBeProcessed);
 *      lineOuts = p->getLines();
 *      print(linesOut);
 *      linesOut.clear();
 *    }
 */
class RecordProcessor : public QObject
{
  Q_OBJECT;

public:

  //-------------------------
  // Format mode to apply to records.
  //-------------------------
  enum FormatMode {
    eAsIs,
    eLongname,
    eTable,
    eCustom
  };
  static FormatMode getFormatMode(QString aFormatMode);
  static QString getFormatModeString(FormatMode aFormatMode);
  static QStringList getFormatModeStringList();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  RecordProcessor(FormatMode aFormatMode = eCustom);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  virtual ~RecordProcessor();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  void configure(FieldItem *aTopNode);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::string getFirstFieldMatch();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  bool process(std::vector<std::string> *aLinesIn);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  bool passedFilterTests();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::vector<std::string> &getOutLines();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  FormatMode getFormatMode();

public slots:

  void onFormatModeAction(QAction* aAction);

protected:

  static ccl::Logger sLogger;

  static QStringList _FormatModeNames;
  static QStringList formatModeNames();

  QMutex _Mutex;

  FormatMode             _FormatMode;
  FieldItem             *_TopNode;
  std::vector<RecordLine> _RecordLines;

  std::vector<std::string>           *_LinesIn;
  std::vector<std::string>            _LinesOut;
  std::vector<std::string>::iterator  _LineIter;

  SimpleLineMatcher _Matcher;

  void reset();

  bool process(FieldItem *aNode,std::string &aDotString);
  bool processRootNode(FieldItem *aNode,std::string &aDotString);
  bool processStructNode(FieldItem *aNode,std::string &aDotString);
  bool processStructArrayNode(FieldItem *aNode,std::string &aDotString);
  bool processPrimitiveNode(FieldItem *aNode,std::string &aDotString);
  bool processPrimitiveArrayNode(FieldItem *aNode,std::string &aDotString);
  bool processPrimitiveArrayLine(FieldItem *aNode,std::string &aDotString,int aIdx);
  bool processChildren(FieldItem *aNode,std::string &aDotString,
      bool aSkipFirstChild = false);

  bool isLineIterOk();
  bool testForMatch(std::string &aValue,std::string &aTest);

  void formatLines();
  void applyFormat(RecordLine &aRecordLine,FieldItemData::Format aFormat);
  void applyTestResults();
  void parseScope(std::string aTestScope,std::string &aBaseScope,
      bool &aIsEltScope);
  void setLinesOut();
  void excludeAllLines();
  void excludeAllLinesMatchingScope(std::string aBaseScope);
  void excludeAllLinesMatchingElementScope(
      std::string aBaseScope,std::string aLineDotString);
  std::vector<std::string> getBaseScopeComponents(std::string aBaseScope);

  void appendToDotString( std::string &aSuffix,std::string &aDotString);

  void printRecordLines();
  void print(const std::string &s1,const std::string &s2);
};

#endif /* RecordProcessor_hh */
