#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <QApplication>
#include <QFile>
#include <QSettings>

#include "AppConfigReader.hh"
#include "MainWindow.hh"
#include "RecordProcessor.hh"
#include "TestStreamWriter.hh"
#include "StreamReader.hh"
#include "Logger.hh"

extern StructorBuilder *lex_main(char *aHeaderFile);

typedef struct CmdLineArgs {
  bool isBrowseMode;
  bool isTestMode;
  QString appConfigFile;
  QString headerFile; // specifies
  QString initialStruct;
  QString testStruct;
} CmdLineArgs;

std::ostringstream _TestInStream;

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void printUsage()
{
  static const char *tUsage =
  "\nFilter mode:"
  "app_iec_wsdlFilter [-a <app_config_file>] [-f <header_file>]\n"
  "   where <app_config_file> is the application configuration file, which\n"
  "   takes its default value from the environment variable\n"
  "   WSDL_FILTER_CONFIG_FILE\n"
  "   where <header_file> specifies a header file that overrides the\n"
  "   default header file specifed in the application configuration file\n"
  "\nBrowse mode:\n"
  "app_iec_wsdlFilter -b [-a <app_config_file>] [-f <header_file>] "
                                             " [-s <struct_name>]\n"
  "   where <header_file> is header file to be browsed, which takes its\n"
  "   default value from the environment variable CLIRCAR_H\n"
  "   where <struct_name> is initial struct to be browsed\n"
  "\nTest mode:\n"
  "app_iec_wsdlFilter -t <test_file> [-a <app_config_file>] [-f <header_file>] "
                                             " [-s <struct_name>]\n"
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
      //"/opt/idp/cots/iec/rtf/static/wsdlToolsConfig/wsdlFilter.conf";
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
// Sets the default header and default headers dir into the app config.
// Values are setting with the following precedence:
// 1. Use the header file specified in the command-line arguements.
// 2. Use the values in the app config file.
// 3. Use the hard-coded defaults
// If the header file specified in the command-line arguments specifies a path
// to the header, i.e. contains at least one "/", then the part of the command-
// line argument up to the last "/" is taken to be the default headers dir, and
// the part after it is taken to be the default header file. If no "/" is found,
// the the command-line argument is just used to set the default header file
// (and the default headers dir will be set per items (2) and (3)).
//-------------------------------------------------------------------------------
void determineHeaderFileInfo(
    CmdLineArgs aArgs,AppConfigReader *aAppConfigReader)
{
  static const char *kHeaderDir ="/opt/idp/cots/iec/rtf/static";//TODO where
  static const char *kHeaderFile ="CLIR_CAR_cxsd.H";

  QString tHeaderFile(kHeaderFile);

  /*
   * Use the command-line value, if available.
   */
  QString &tArgsHeader = aArgs.headerFile;

  if (tArgsHeader.length() > 0)
  {
    int tIdxLast = tArgsHeader.lastIndexOf('/');

    if (tIdxLast == -1) // no path component
    {
      aAppConfigReader->appConfig().SetDefaultHeader(tArgsHeader);
    }
    else
    {
      QString tDir = tArgsHeader.left(tIdxLast);
      QString tFile = tArgsHeader.right(tArgsHeader.length()-tIdxLast-1);
      aAppConfigReader->appConfig().SetHeadersDir(tDir);
      aAppConfigReader->appConfig().SetDefaultHeader(tFile);
    }
  }

  if (aAppConfigReader->appConfig().getHeadersDir().length() == 0)
  {
    aAppConfigReader->appConfig().SetHeadersDir(kHeaderDir);
  }
  if (aAppConfigReader->appConfig().getDefaultHeader().length() == 0)
  {
    aAppConfigReader->appConfig().SetDefaultHeader(kHeaderFile);
  }
//std::cout << "DIR: " << qPrintable(aAppConfigReader->appConfig().getHeadersDir()) << std::endl;
//std::cout << "FIL: " << qPrintable(aAppConfigReader->appConfig().getDefaultHeader()) << std::endl;
// exit(0);
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
CmdLineArgs processCommandLine(int argc,char *argv[])
{
  CmdLineArgs tCmdLineArgs; //TODO make CmdLineArgs a class for initalization
  tCmdLineArgs.isBrowseMode = false;
  tCmdLineArgs.isTestMode = false;

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
    if (!strcmp(argv[tIdx],"-t"))
    {
      tCmdLineArgs.isTestMode = true;
      if (++tIdx < argc)
      {
        tCmdLineArgs.testStruct = argv[tIdx];
      }
      else
      {
        std::cerr << "ERROR: missing test file name after -t\n";
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
  AppConfig tAppConfig = aAppConfigReader->appConfig();
  std::cout << endl;
  std::cout << "app config settings:" << std::endl;
  std::cout << qPrintable(tAppConfig.toQString()) << std::endl;
  std::cout << endl;

  std::cout << "message map:" << std::endl;
  std::map<QString,MessageSpec> &tMessageMap = aAppConfigReader->messageMap();
  std::map<QString,MessageSpec>::iterator tIter;
  for (tIter = tMessageMap.begin(); tIter != tMessageMap.end(); tIter++)
  {
    MessageSpec tSpec = tIter->second;
    std::cout << qPrintable(tSpec.toQString()) << std::endl;
  }
  std::cout << endl;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
QString getHeaderFilePath(AppConfig aAppConfig)
{
  QString tDir = aAppConfig.getHeadersDir();
  QString tFile = aAppConfig.getDefaultHeader();
  QString tHeaderFile = tDir + "/" + tFile;
  return tHeaderFile;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
std::string getTestRecord(AppConfigReader *aAppConfigReader,CmdLineArgs aArgs)
{
  QString tHeaderFile = getHeaderFilePath(aAppConfigReader->appConfig());

  // Parse the header file to get _StructBuilder.
  StructorBuilder *tStructorBuilder = MainWindow::parseHeaderFile(tHeaderFile);

  std::string tStructName = aArgs.testStruct.toStdString();

  Structure *tStructure =
      tStructorBuilder->getStructure(tStructName);

  if (!tStructure)
  {
    std::cerr << "ERROR: couldn't find struct " << tStructName << std::endl;
    exit(0);
  }

  /*
   * Create the data structure model for the specified data structure.
   */
  DataStructModel *tModel = new DataStructModel(tStructure,tStructorBuilder);
  std::string tRecord = tModel->getTestRecord(tStructName);
std::cout << "TEST RECORD:\n" << tRecord << std::endl;
  return tRecord;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runBrowseMode(QApplication &app,CmdLineArgs aArgs,
    AppConfigReader *aAppConfigReader)
{
  std::cout << "Running in browse mode..." << std::endl;

  determineHeaderFileInfo(aArgs,aAppConfigReader);

  QString tHeaderFile = getHeaderFilePath(aAppConfigReader->appConfig());

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
  bool tIsTestMode = aArgs.isTestMode;

  AppConfig &aAppConfig = aAppConfigReader->appConfig();
  MessageSpecMap &aMessageSpecMap = aAppConfigReader->messageMap();

  /*
   * Determine values for header file params.
   */
  determineHeaderFileInfo(aArgs,aAppConfigReader);

  /*
   * Create the record processor.
   */
  RecordProcessor *tRecordProcessor = new RecordProcessor();

  /*
   * Create the stream reader.
   */
  StreamReader::OperateMode tOperateMode =
      StreamReader::getOperateMode(aAppConfig.getDefaultOperateMode());
  StreamReader::DelimitMode tDelimitMode =
      StreamReader::getDelimitMode(aAppConfig.getDefaultDelimitMode());

  StreamReader *tStreamReader;
  TestStreamWriter *tTestStreamWriter;

  if (!tIsTestMode)
  {
    std::cout << "Running in filter mode..." << std::endl;

    tStreamReader = new StreamReader(cin,
        tRecordProcessor,tOperateMode,tDelimitMode);
  }
  else
  {
    std::cout << "Running in test mode..." << std::endl;

    std::string tTestString = getTestRecord(aAppConfigReader,aArgs);

    tTestStreamWriter = new TestStreamWriter(tTestString);
    std::stringstream &tOutStream = tTestStreamWriter->getTestStream();

    tStreamReader = new StreamReader(tOutStream,
        tRecordProcessor,tOperateMode,tDelimitMode,true);
  }

  /*
   * Create the main window. It won't be launched until later though, after
   * the stream reader has determined which data structure type is being read.
   */
  MainWindow *tMainWindow = new MainWindow(app,0,aAppConfig,aMessageSpecMap,
      tStreamReader,tRecordProcessor);
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
