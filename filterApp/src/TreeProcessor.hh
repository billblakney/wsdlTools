#ifndef _TREEPROCESSOR_HH_
#define _TREEPROCESSOR_HH_

#include "FieldItem.hh"
#include "Logger.hh"

class TreeProcessor
{
public:
  TreeProcessor();
  virtual ~TreeProcessor();
  static bool traverse(FieldItem *aRootItem);
protected:
  static ccl::Logger sLogger;
  static bool traverseRootNode(FieldItem *aNode);
  static bool traverseStructNode(FieldItem *aNode);
  static bool traverseStructArrayNode(FieldItem *aNode);
  static bool traversePrimitiveNode(FieldItem *aNode);
  static bool traversePrimitiveArrayNode(FieldItem *aNode);
  static bool traverseChildren(FieldItem *aNode);
};

#endif /* _TREEPROCESSOR_HH_ */
