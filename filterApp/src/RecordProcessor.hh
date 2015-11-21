#ifndef _RECORDPROCESSOR_HH_
#define _RECORDPROCESSOR_HH_

#include "FieldItem.hh"
#include "RecordLine.hh"
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
class RecordProcessor
{
public:

  RecordProcessor(FieldItem *aTopNode,std::vector<std::string> &aLinesIn);

  virtual ~RecordProcessor();

  bool process();

  bool passedFilterTests();

  std::vector<std::string> &getOutLines();

protected:

  FieldItem *_TopNode;

  std::vector<RecordLine> _RecordLines;

  static ccl::Logger sLogger;

  std::vector<std::string>           &_LinesIn;
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
  bool processChildren(FieldItem *aNode,std::string &aDotString);

  bool testForMatch(std::string &aValue,std::string &aTest);

  void formatLines();
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

#endif /* _RECORDPROCESSOR_HH_ */
