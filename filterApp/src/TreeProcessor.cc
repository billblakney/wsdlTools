#include "TreeProcessor.hh"

ccl::Logger TreeProcessor::sLogger("TreeProcessor");

TreeProcessor::TreeProcessor()
{
}

TreeProcessor::~TreeProcessor()
{
}

bool TreeProcessor::traverse(FieldItem *aNode)
{
  if ( aNode->getData().getNodeType() == FieldItemData::eRoot )
  {
    traverseRootNode(aNode);
  }
  else if ( aNode->getData().getNodeType() == FieldItemData::eStruct)
  {
    traverseStructNode(aNode);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::eStructArray)
  {
    traverseStructArrayNode(aNode);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::ePrimitive)
  {
    traversePrimitiveNode(aNode);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::ePrimitiveArray)
  {
    traversePrimitiveArrayNode(aNode);
  }
  else
  {
    ERROR(sLogger,"<traverse> unknown node type");
    return false;
  }

  return true;
}

bool TreeProcessor::traverseRootNode(FieldItem *aNode)
{
  INFO(sLogger,"<traverse> eRoot: " << aNode->getData().getName());
  return traverseChildren(aNode);
}

bool TreeProcessor::traverseStructNode(FieldItem *aNode)
{
  INFO(sLogger,"<traverse> eStruct: " << aNode->getData().getName());
  return traverseChildren(aNode);
}

bool TreeProcessor::traverseStructArrayNode(FieldItem *aNode)
{
  INFO(sLogger,"<traverse> eStructArray: " << aNode->getData().getName());
  return traverseChildren(aNode);
}

bool TreeProcessor::traversePrimitiveNode(FieldItem *aNode)
{
  INFO(sLogger,"<traverse> ePrimitive: " << aNode->getData().getName());
  return true;
}

bool TreeProcessor::traversePrimitiveArrayNode(FieldItem *aNode)
{
  INFO(sLogger,"<traverse> ePrimitiveArray: " << aNode->getData().getName());
  return true;
}

bool TreeProcessor::traverseChildren(FieldItem *aNode)
{
  for (int tIdx = 0; tIdx < aNode->childCount(); tIdx++)
  {
    bool tResult = traverse(aNode->child(tIdx));
    if (tResult == false)
    {
      ERROR(sLogger,"<traverse> traversing child failed");
      return false;
    }
  }
  return true;
}
