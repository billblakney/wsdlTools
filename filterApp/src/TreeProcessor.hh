#ifndef _TREEPROCESSOR_HH_
#define _TREEPROCESSOR_HH_

#include "FieldItem.hh"
#include "Logger.hh"

class TreeProcessor
{
public:
  TreeProcessor(FieldItem *aTopNode);
  virtual ~TreeProcessor();
  virtual bool process();
protected:
  static ccl::Logger sLogger;
  FieldItem *_TopNode;
  virtual bool process(FieldItem *aNode);
  virtual bool processRootNode(FieldItem *aNode);
  virtual bool processStructNode(FieldItem *aNode);
  virtual bool processStructArrayNode(FieldItem *aNode);
  virtual bool processPrimitiveNode(FieldItem *aNode);
  virtual bool processPrimitiveArrayNode(FieldItem *aNode);
  virtual bool processChildren(FieldItem *aNode);
};

#endif /* _TREEPROCESSOR_HH_ */
