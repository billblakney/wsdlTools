#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <QApplication>
#include <QFile>
#include <QSettings>

#include "AppConfigReader.hh"
#include "HeaderUtil.hh"
#include "MainWindow.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "TestRecordBuilder.hh"
#include "Logger.hh"

extern StructorBuilder *lex_main(char *aHeaderFile);

typedef struct CmdLineArgs {
  bool isDebug;
  bool isBrowseMode;
  bool isTestMode;
  bool isPrintTestRecMode;
  QString appConfigFile;
  QString headerFile; // specifies
  QString initialStruct;
  QString testMsgId;
  QString testStruct;
} CmdLineArgs;

static bool debug = false;

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void printUsage()
{
  static const char *tUsage =
  "Overview\n"
  "app_iec_wsdlFilter runs in a filter mode, a browse mode, or in a couple\n"
  "of other modes useful for testing. The filter mode is used to filter the\n"
  "output of a WSDLConsumer output stream. The browse mode is used to browse\n"
  "data structures from a header file. A test mode is available which\n"
  "simulates the filter mode. It can be used to build and test filters.\n"
  "A \"print test record\" mode is available to also help with testing.\n"
  "When that mode is run, the app prints out a record similar in format to\n"
  "the WSDLConsumer output. This file can then be fed into the app (using\n"
  "a script, for example) while running in filter mode, to test the actual\n"
  "filter mode.\n"
  "\n"
  "Optional command-line options common to all run modes:\n"
  "-a <app_config_file>\n"
  "   where <app_config_file> is the app config file, which is read when the\n"
  "   app starts up. When set, this command-line argument overrides the value\n"
  "   that may have been set by the WSDL_FILTER_CONFIG_FILE environment\n"
  "   variable, which in turn, if present, overrides the default hard-coded\n"
  "   value.\n"
  "\n"
  "-f <header_file>\n"
  "   where the <header_file> specification overrides the default header file\n"
  "   defined in the app config file. If <header_file> is a full pathname,\n"
  "   then the path up to the header file is used to replace the default\n"
  "   headers dir defined in the app config file.\n"
  "\n"
  "-d\n"
  "   prints out some debug information\n"
  "\n"
  "Filter mode: filter WSDLConsumer output stream\n"
  "app_iec_wsdlFilter\n"
  "\n"
  "Browse mode: browse data structures\n"
  "app_iec_wsdlFilter -b [-s <struct_name>]\n"
  "   where <struct_name> is initial struct to be browsed\n"
  "\n"
  "Test mode: run in simulated mode without WSDLConsumer\n"
  "app_iec_wsdlFilter -t <msg_id> <struct_name>\n"
  "   where <msg_id> and <struct_name> are the values from the WSDLConsumer\n"
  "   stream to be simulated\n"
  "\n"
  "Print test rec mode: print a record similar WSDLConsumer output format\n"
  "app_iec_wsdlFilter -p <msg_id> <struct_name>\n"
  "   where <msg_id> and <struct_name> are the values from the WSDLConsumer\n"
  "   stream to be simulated\n"
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
    /*
     * "-h" for help.
     */
    if (!strcmp(argv[tIdx],"-h"))
    {
      printUsage();
      exit(0);
    }
    /*
     * "-d" to enable some extra debug output.
     */
    if (!strcmp(argv[tIdx],"-d"))
    {
      tCmdLineArgs.isDebug = true;
      debug = true;
    }
    /*
     * "-b" for browse mode
     */
    if (!strcmp(argv[tIdx],"-b"))
    {
      tCmdLineArgs.isBrowseMode = true;
    }
    /*
     * "-s <struct_name>" for initial struct name in browse mode
     */
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
    /*
     * "-f <header_file>" to override default header file.
     * If <header_file> is a full path, it also overrides the default headers
     * dir.
     */
    //
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
    /*
     * "-t <msg_id> <struct_name>" for test mode.
     * "-p <msg_id> <struct_name>" for print test record mode.
     */
    if (!strcmp(argv[tIdx],"-t") || !strcmp(argv[tIdx],"-p"))
    {
      if (!strcmp(argv[tIdx],"-t"))
      {
        tCmdLineArgs.isTestMode = true;
      }
      else
      {
        tCmdLineArgs.isPrintTestRecMode = true;
      }
      if (++tIdx < argc)
      {
        tCmdLineArgs.testMsgId = argv[tIdx];
      }
      else
      {
        std::cerr << "ERROR: missing test message id name after -t\n";
        exit(0);
      }
      if (++tIdx < argc)
      {
        tCmdLineArgs.testStruct = argv[tIdx];
      }
      else
      {
        std::cerr << "ERROR: missing test struct name after message id\n";
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
void printTestRecord(CmdLineArgs aArgs,AppConfigReader *aAppConfigReader)
{
  determineHeaderFileInfo(aArgs,aAppConfigReader);
  QString tHeaderFile = getHeaderFilePath(aAppConfigReader->appConfig());

  // Get the test string.
  TestRecordBuilder *tBuilder = new TestRecordBuilder(aArgs.testMsgId,
      aArgs.testStruct,tHeaderFile);
  std::string tTestString = tBuilder->getTestRecord();

  std::cout << tTestString << std::endl;
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
  StructorBuilder *tStructorBuilder = HeaderUtil::parseHeaderFile(tHeaderFile);

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
void runFilterMode(
    QApplication &app,CmdLineArgs aArgs,AppConfigReader *aAppConfigReader)
{
  bool tIsTestMode = aArgs.isTestMode;

  AppConfig &aAppConfig = aAppConfigReader->appConfig();
  MessageSpecMap &aMessageSpecMap = aAppConfigReader->messageMap();

  /*
   * Determine values for header file params.
   */
  determineHeaderFileInfo(aArgs,aAppConfigReader);
  QString tHeaderFile = getHeaderFilePath(aAppConfigReader->appConfig());

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

  if (!tIsTestMode)
  {
    std::cout << "Running in filter mode..." << std::endl;

    tStreamReader = new StreamReader(cin,
        tRecordProcessor,tOperateMode,tDelimitMode);
  }
  else
  {
    std::cout << "Running in test mode..." << std::endl;

    // Get the test string.
    TestRecordBuilder *tBuilder = new TestRecordBuilder(aArgs.testMsgId,
        aArgs.testStruct,tHeaderFile);
    std::string tTestString = tBuilder->getTestRecord();
    std::cout << "TEST RECORD:\n" << tTestString << std::endl;

    // Write the test string into the test stringstream.
    std::stringstream *tTestStream = new std::stringstream();
    *tTestStream << tTestString;

    tStreamReader = new StreamReader(*tTestStream,
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

  // set the palette
//  QPalette tPalette(Qt::lightGray);
//  app.setPalette(tPalette);

  // read the command-line args
  CmdLineArgs tArgs = processCommandLine(argc,argv);

  // initialize logger
  ccl::Logger::initialize();

  // read the app config file
  QString tFilename = determineAppConfigFilename(tArgs);
  if (debug)
  {
    std::cout << "Reading app config file " << qPrintable(tFilename)
              << "..." << std::endl;
  }
  AppConfigReader *tAppConfigReader = new AppConfigReader(tFilename);
  tAppConfigReader->openConfiguration();
  if (debug)
  {
    printAppConfigFile(tAppConfigReader);
  }

  // run app in specified mode
  if (tArgs.isBrowseMode)
  {
    runBrowseMode(app,tArgs,tAppConfigReader);
  }
  else if (tArgs.isPrintTestRecMode)
  {
    printTestRecord(tArgs,tAppConfigReader);
    return 0;
  }
  else
  {
    runFilterMode(app,tArgs,tAppConfigReader);
  }

  return app.exec();
}
