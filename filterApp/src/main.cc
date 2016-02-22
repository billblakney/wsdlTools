#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <QApplication>
#include <QFile>
#include <QSettings>

#include "AppConfigReader.hh"
#include "MainWindow.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "Logger.hh"

extern StructorBuilder *lex_main(char *aHeaderFile);

typedef struct CmdLineArgs {
  bool isBrowseMode;
  QString appConfigFile;
  QString headerFile;
  QString initialStruct;
} CmdLineArgs;

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void printUsage()
{
  static const char *tUsage =
  "\nFilter mode:"
  "app_iec_wsdlFilter [-a <app_config_file>]\n"
  "   where <app_config_file> is the application configuration file, which\n"
  "   takes its default value from the environment variable\n"
  "   WSDL_FILTER_CONFIG_FILE\n"
  "\nBrowse mode:\n"
  "app_iec_wsdlFilter -b [-f <header_file>] [-s <struct_name>]\n"
  "   where <header_file> is header file to be browsed, which takes its\n"
  "   default value from the environment variable CLIRCAR_H\n"
  "   where <struct_name> is initial struct to be browsed\n"
  ;
  std::cout << tUsage << std::endl;
}

//-------------------------------------------------------------------------------
// Determine the name of the app config file to be used in filter mode.
// The name of the app config file is set with the following precedence:
// 1. Use value specified by the command-line argument.
// 2. Use the value specified by the WSDL_FILTER_CONFIG_FILE.
// 3. Use the hard-coded value.
//-------------------------------------------------------------------------------
QString determineAppConfigFilename(CmdLineArgs aArgs)
{
  static const char *kAppConfigFilename =
      //"/opt/idp/cots/iec/rtf/static/wsdlTools/wsdlFilter.conf";
      "/opt/idp/cots/iec/rtf/static/wsdlFilter.conf";

  /*
   * Default will be the hard-coded value.
   */
  QString tAppConfigFilename(kAppConfigFilename);

  /*
   * Use command line arg, if available.
   */
  if (aArgs.appConfigFile.length() > 0)
  {
    return aArgs.appConfigFile;
  }

  /*
   * Use the WSDL_FILTER_CONFIG_FILE environment variable, if available.
   */
  if(getenv("WSDL_FILTER_CONFIG_FILE"))
  {
    return getenv("WSDL_FILTER_CONFIG_FILE");
  }

  return tAppConfigFilename;
}

//-------------------------------------------------------------------------------
// Determine the header file to be used in browse mode.
// The name of the header file is set with the following precedence:
// 1. Use value specified by the command-line.
// 2. Use the value specified by the app config file.
// 3. Use the hard-coded value.
//-------------------------------------------------------------------------------
QString determineHeaderFile(CmdLineArgs aArgs,AppConfigReader *aAppConfigReader)
{
  static const char *kHeaderFile ="/opt/idp/cots/iec/rtf/static/CLIR_CAR_cxsd.H";

  QString tHeaderFile(kHeaderFile);

  /*
   * Use the command-line value, if available.
   */
  if (aArgs.headerFile.length() > 0)
  {
    return aArgs.headerFile;
  }

  /*
   * Use the value in the app config file, if available.
   */
  if (aAppConfigReader)
  {
    QString tHeadersDir = aAppConfigReader->appConfig().getHeadersDir();
    QString tHeader = aAppConfigReader->appConfig().getDefaultHeader();
    if (tHeadersDir.length() > 0 && tHeader.length() > 0)
    {
      QString tHeaderFilePath(tHeadersDir + "/" + tHeader);
      return tHeaderFilePath;
    }
  }

  /*
   * Use the hard-coded value.
   */
  return tHeaderFile;
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
CmdLineArgs processCommandLine(int argc,char *argv[])
{
  CmdLineArgs tCmdLineArgs;
  tCmdLineArgs.isBrowseMode = false;

  for (int tIdx = 0; tIdx < argc; tIdx++)
  {
    if (!strcmp(argv[tIdx],"-h"))
    {
      printUsage();
      exit(0);
    }
    if (!strcmp(argv[tIdx],"-b"))
    {
      tCmdLineArgs.isBrowseMode = true;
    }
    if (!strcmp(argv[tIdx],"-s"))
    {
      if (++tIdx < argc)
      {
        tCmdLineArgs.initialStruct = argv[tIdx];
      }
      else
      {
        std::cerr << "ERROR: missing struct name after -s\n";
        exit(0);
      }
    }
    if (!strcmp(argv[tIdx],"-f"))
    {
      if (++tIdx < argc)
      {
        tCmdLineArgs.headerFile = argv[tIdx];
      }
      else
      {
        std::cerr << "ERROR: missing header name after -f\n";
        exit(0);
      }
    }
  }
  return tCmdLineArgs;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void printAppConfigFile(AppConfigReader *aAppConfigReader)
{
  std::map<QString,MessageSpec> &tMessageMap = aAppConfigReader->messageMap();

  std::map<QString,MessageSpec>::iterator tIter;
  for (tIter = tMessageMap.begin(); tIter != tMessageMap.end(); tIter++)
  {
    MessageSpec tSpec = tIter->second;
    std::cout << qPrintable(tSpec.toQString()) << std::endl;
  }
  AppConfig tAppConfig = aAppConfigReader->appConfig();
  std::cout << "settings:" << std::endl;
  std::cout << qPrintable(tAppConfig.toQString()) << std::endl;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runBrowseMode(QApplication &app,CmdLineArgs aArgs,
    AppConfigReader *aAppConfigReader)
{
  std::cout << "Running in browse mode..." << std::endl;

  QString tHeaderFile = determineHeaderFile(aArgs,aAppConfigReader);

  // Parse the header file to get _StructBuilder.
  StructorBuilder *tStructorBuilder = MainWindow::parseHeaderFile(tHeaderFile);

  // create and launch main window
  std::cout << "Launching main window..." << std::endl;
  MainWindow *window = new MainWindow(
      app,0,aAppConfigReader->appConfig(),aAppConfigReader->messageMap(),
      tStructorBuilder);
//  window->setGeometry(1920 + 530,135,625,900);
  window->setGeometry(1920      ,135,900,900);
  window->setupView(aArgs.initialStruct.toStdString());
  window->show();
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runStreamReaderMode(
    QApplication &app,CmdLineArgs aArgs,AppConfigReader *aAppConfigReader)
{
  Q_UNUSED(aArgs);

  std::cout << "Running in filter mode..." << std::endl;

  /*
   * Create the record processor.
   */
  RecordProcessor *tRecordProcessor = new RecordProcessor();

  /*
   * Create the stream reader.
   */
  StreamReader *tStreamReader = new StreamReader(tRecordProcessor);

  /*
   * Create the main window. It won't be launched until later though, after
   * the stream reader has determined which data structure type is being read.
   */
  MainWindow *tMainWindow = new MainWindow(app,0,aAppConfigReader->appConfig(),
      aAppConfigReader->messageMap(),tStreamReader,tRecordProcessor);
  // tMainWindow->setGeometry(1920 + 530,135,625,900);
  // tMainWindow->setGeometry(1920      ,135,900,900);
  tMainWindow->setGeometry(1920      ,135,650,700);

  /*
   * Setup connection to let the main window know when the reader has found
   * the type of structure to be processed.
   */
  QApplication::connect(
      tStreamReader,SIGNAL(structNameAvailable(QString,QString)),
      tMainWindow,SLOT(onStructNameAvailable(QString,QString)));

  /*
   * Setup connection to let the stream reader have access to the data struct
   * model.
   */
  QApplication::connect(
      tMainWindow,SIGNAL(dataStructModelAvailable(void *)),
      tStreamReader,SLOT(onDataStructModelAvailable(void *)));

  /*
   * Run the stream reader.
   */
  tStreamReader->start();
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
//  app.setApplicationDisplayName(QString("WSDL FIlter"));

  // set the palette
//  QPalette tPalette(Qt::lightGray);
//  app.setPalette(tPalette);

  // read the command-line args
  CmdLineArgs tArgs = processCommandLine(argc,argv);

  // initialize logger
  ccl::Logger::initialize();

  // read the app config file
  QString tFilename = determineAppConfigFilename(tArgs);
  std::cout << "Reading app config file " << qPrintable(tFilename)
            << "..." << std::endl;
  AppConfigReader *tAppConfigReader = new AppConfigReader(tFilename);
  tAppConfigReader->openConfiguration();
  printAppConfigFile(tAppConfigReader);

  // run app in specified mode
  if (tArgs.isBrowseMode)
  {
    runBrowseMode(app,tArgs,tAppConfigReader);
  }
  else
  {
    runStreamReaderMode(app,tArgs,tAppConfigReader);
  }

  return app.exec();
}
