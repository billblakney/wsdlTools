#ifndef _RECORDPROCESSOR_HH_
#define _RECORDPROCESSOR_HH_

#include "FieldItem.hh"
#include "SimpleLineMatcher.hh"
#include "TreeProcessor.hh"
#include "Logger.hh"

/*
 * Processes records of input read by a StreamReader.
 * Can be instantiated once, and re-used. For example, after instantiation
 * a typical use-case sequence would be:
 *    RecordProcessor *p = new RecordProcessor(theTopNode,theLinesToBeProcessed);
 *    while (1){
 *      theLinesToBeProcessed.clear();
 *      theLinesToBeProcessed = readNextLines();
 *      p->process();
 *      lineOuts = p->getLines();
 *      print(linesOut);
 *      linesOut.clear();
 *    }
 */
class RecordProcessor : public TreeProcessor
{
public:

  RecordProcessor(FieldItem *aTopNode,std::vector<std::string> &aLinesIn);

  virtual ~RecordProcessor();

  virtual bool process();

  bool passedFilterTests();

  std::vector<std::string> &getOutLines();

protected:

  struct RecLine {
    std::string line;
    std::string scope;
  };

  std::vector<RecLine> _RecLines;

  static ccl::Logger sLogger;

  std::vector<std::string>           &_LinesIn;
  std::vector<std::string>            _LinesOut;
  std::vector<std::string>::iterator  _LineIter;
  bool                                _TestResult;

  SimpleLineMatcher _Matcher;

  void reset();

  virtual bool processRootNode(FieldItem *aNode,void *aData);
  virtual bool processStructNode(FieldItem *aNode,void *aData);
  virtual bool processStructArrayNode(FieldItem *aNode,void *aData);
  virtual bool processPrimitiveNode(FieldItem *aNode,void *aData);
  virtual bool processPrimitiveArrayNode(FieldItem *aNode,void *aData);
  bool processPrimitiveArrayLine(FieldItem *aNode,void *aData,int aIdx);

  bool testForMatch(std::string &aValue,std::string &aTest);
};

#endif /* _RECORDPROCESSOR_HH_ */
