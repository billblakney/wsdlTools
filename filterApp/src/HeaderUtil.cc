#include <iostream>
#include <QFile>

#include "HeaderUtil.hh"

extern StructorBuilder *lex_main(char *aHeaderFile);

HeaderUtil::HeaderUtil()
{
}

HeaderUtil::~HeaderUtil()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StructorBuilder *HeaderUtil::parseHeaderFile(QString aHeaderFile)
{
  QFile tHeaderFile(aHeaderFile);

  // need the header file to be defined
  if (!aHeaderFile.length())
  {
    std::cout << "ERROR: No header file specified." << std::endl;
    std::cout << "       Use -f option or set CLIRCAR_H env var\n"
              << std::endl;
    exit(1);
  }
  else if (!tHeaderFile.exists())
  {
    std::cout << "ERROR: Header file \"" << qPrintable(aHeaderFile) << "\""
              << " does not exist" << std::endl;
    exit(1);
  }

//  std::cout << "Using header file " << qPrintable(aHeaderFile)
//            << "..." << std::endl;
//
//  std::cout << "Parsing header file " << qPrintable(aHeaderFile)
//            << "..." << std::endl;

  StructorBuilder *tStructorBuilder = NULL;

  std::string tString = aHeaderFile.toStdString();
  tStructorBuilder = lex_main((char *)tString.c_str());
  //   _StructorBuilder->printSummary();
  //   _StructorBuilder->postProcess();

  return tStructorBuilder;
}

