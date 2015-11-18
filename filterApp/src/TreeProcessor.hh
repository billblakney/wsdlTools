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
  virtual bool process(FieldItem *aNode,void *aData = NULL);
  virtual bool processRootNode(FieldItem *aNode,void *aData = NULL);
  virtual bool processStructNode(FieldItem *aNode,void *aData = NULL);
  virtual bool processStructArrayNode(FieldItem *aNode,void *aData = NULL);
  virtual bool processPrimitiveNode(FieldItem *aNode,void *aData = NULL);
  virtual bool processPrimitiveArrayNode(FieldItem *aNode,void *aData = NULL);
  virtual bool processChildren(FieldItem *aNode,void *aData = NULL);
};

#endif /* _TREEPROCESSOR_HH_ */
