#include "TreeProcessor.hh"

ccl::Logger TreeProcessor::sLogger("TreeProcessor");

TreeProcessor::TreeProcessor()
{
}

TreeProcessor::~TreeProcessor()
{
}

bool TreeProcessor::process(FieldItem *aNode)
{
  if ( aNode->getData().getNodeType() == FieldItemData::eRoot )
  {
    processRootNode(aNode);
  }
  else if ( aNode->getData().getNodeType() == FieldItemData::eStruct)
  {
    processStructNode(aNode);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::eStructArray)
  {
    processStructArrayNode(aNode);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::ePrimitive)
  {
    processPrimitiveNode(aNode);
  }
  else if (aNode->getData().getNodeType() == FieldItemData::ePrimitiveArray)
  {
    processPrimitiveArrayNode(aNode);
  }
  else
  {
    ERROR(sLogger,"<process> unknown node type");
    return false;
  }

  return true;
}

bool TreeProcessor::processRootNode(FieldItem *aNode)
{
  INFO(sLogger,"<process> eRoot: " << aNode->getData().getName());
  return processChildren(aNode);
}

bool TreeProcessor::processStructNode(FieldItem *aNode)
{
  INFO(sLogger,"<process> eStruct: " << aNode->getData().getName());
  return processChildren(aNode);
}

bool TreeProcessor::processStructArrayNode(FieldItem *aNode)
{
  INFO(sLogger,"<process> eStructArray: " << aNode->getData().getName());
  return processChildren(aNode);
}

bool TreeProcessor::processPrimitiveNode(FieldItem *aNode)
{
  INFO(sLogger,"<process> ePrimitive: " << aNode->getData().getName());
  return true;
}

bool TreeProcessor::processPrimitiveArrayNode(FieldItem *aNode)
{
  INFO(sLogger,"<process> ePrimitiveArray: " << aNode->getData().getName());
  return true;
}

bool TreeProcessor::processChildren(FieldItem *aNode)
{
  for (int tIdx = 0; tIdx < aNode->childCount(); tIdx++)
  {
    bool tResult = process(aNode->child(tIdx));
    if (tResult == false)
    {
      ERROR(sLogger,"<process> traversing child failed");
      return false;
    }
  }
  return true;
}
