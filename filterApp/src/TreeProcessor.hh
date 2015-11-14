#ifndef _TREEPROCESSOR_HH_
#define _TREEPROCESSOR_HH_

#include "FieldItem.hh"
#include "Logger.hh"

class TreeProcessor
{
public:
  TreeProcessor();
  virtual ~TreeProcessor();
  virtual bool process(FieldItem *aRootItem);
protected:
  ccl::Logger sLogger;
  virtual bool processRootNode(FieldItem *aNode);
  virtual bool processStructNode(FieldItem *aNode);
  virtual bool processStructArrayNode(FieldItem *aNode);
  virtual bool processPrimitiveNode(FieldItem *aNode);
  virtual bool processPrimitiveArrayNode(FieldItem *aNode);
  virtual bool processChildren(FieldItem *aNode);
};

#endif /* _TREEPROCESSOR_HH_ */
