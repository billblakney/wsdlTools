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

  std::vector<std::string> &getOutLines();

protected:
  static ccl::Logger sLogger;

  std::vector<std::string>           &_LinesIn;
  std::vector<std::string>            _LinesOut;
  std::vector<std::string>::iterator  _LineIter;

  SimpleLineMatcher _Matcher;

  void reset();

//  virtual bool process(FieldItem *aNode);
  virtual bool processRootNode(FieldItem *aNode);
  virtual bool processStructNode(FieldItem *aNode);
  virtual bool processStructArrayNode(FieldItem *aNode);
  virtual bool processPrimitiveNode(FieldItem *aNode);
  virtual bool processPrimitiveArrayNode(FieldItem *aNode);
  bool processPrimitiveArrayLine(FieldItem *aNode);
//  virtual bool processChildren(FieldItem *aNode);
};

#endif /* _RECORDPROCESSOR_HH_ */
