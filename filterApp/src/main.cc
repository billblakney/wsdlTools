#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include "AppConfigFile.hh"
#include "MainWindow.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "Logger.hh"

extern StructorBuilder *lex_main(char *aHeaderFile);

static bool _BrowseMode = false;

static std::string _InitialStruct;

QString _ConfigFile("/opt/idp/cots/iec/rtf/static/wsdlTools/wsdlFilter.conf");

QString _HeaderFile("/opt/idp/cots/iec/rtf/static/CLIR_CAR_cxsd.H");

StructorBuilder *_StructorBuilder(0);

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void printUsage()
{
  std::cout << "Filter mode:" << std::endl;
  std::cout << "   app_iec_wsdlFilter [-a <app_config_file>]" << std::endl;
  std::cout << "   where -a overrides the environment variable WSDL_FILTER_CONFIG_FILE" << std::endl;
  std::cout << "Browse mode:" << std::endl;
  std::cout << "   app_iec_wsdlFilter -b -f <header_file>" << std::endl;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void processCommandLine(int argc,char *argv[])
{
  for (int tIdx = 0; tIdx < argc; tIdx++)
  {
    if (!strcmp(argv[tIdx],"-h"))
    {
      printUsage();
      exit(0);
    }
    if (!strcmp(argv[tIdx],"-b"))
    {
      _BrowseMode = true;
    }
    if (!strcmp(argv[tIdx],"-s"))
    {
      if (++tIdx < argc)
      {
        _InitialStruct = argv[tIdx];
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
        _HeaderFile = argv[tIdx];
      }
      else
      {
        std::cerr << "ERROR: missing header name after -f\n";
        exit(0);
      }
    }
  }
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void readEnvironmentVariables()
{
  if(getenv("WSDL_FILTER_CONFIG_FILE"))
  {
    _ConfigFile = getenv("WSDL_FILTER_CONFIG_FILE");
  }
  if(getenv("CLIRCAR_H"))
  {
    _HeaderFile = getenv("CLIRCAR_H");
  }
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runBrowseMode(QApplication &app,AppConfigFile *aAppConfigFile)
{
  std::cout << "Running in browse mode..." << std::endl;

  // Parse the header file to get _StructBuilder.
  StructorBuilder *tStructorBuilder = MainWindow::parseHeaderFile(_HeaderFile);

  // create and launch main window
  std::cout << "Launching main window..." << std::endl;
  MainWindow *window = new MainWindow(app,0,aAppConfigFile,tStructorBuilder);
//  window->setGeometry(1920 + 530,135,625,900);
  window->setGeometry(1920      ,135,900,900);
  window->setupView(_InitialStruct);
  window->show();
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runStreamReaderMode(QApplication &app,AppConfigFile *aAppConfigFile)
{
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
  MainWindow *tMainWindow = new MainWindow(app,0,aAppConfigFile,
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
AppConfigFile *readAppConfigFile(bool aDoPrintSummary)
{
  AppConfigFile *tAppConfigFile = new AppConfigFile(_ConfigFile);

  std::cout << "Reading app config file " << qPrintable(_ConfigFile)
            << "..." << std::endl;
  tAppConfigFile->openConfiguration();

  if (aDoPrintSummary)
  {
    std::map<QString,MessageSpec> &tMessageMap = tAppConfigFile->messageMap();

    std::map<QString,MessageSpec>::iterator tIter;
    for (tIter = tMessageMap.begin(); tIter != tMessageMap.end(); tIter++)
    {
      MessageSpec tSpec = tIter->second;
      std::cout << qPrintable(tSpec.toQString()) << std::endl;
    }
    AppConfig tAppConfig = tAppConfigFile->appConfig();
    std::cout << "settings:" << std::endl;
    std::cout << qPrintable(tAppConfig.toQString()) << std::endl;
  }

  return tAppConfigFile;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  ccl::Logger::initialize();

  QApplication app(argc, argv);
//  app.setApplicationDisplayName(QString("WSDL FIlter"));

//  QPalette tPalette(Qt::lightGray);
//  app.setPalette(tPalette);

  /*
   * Read the environment variables.
   * Currently only one: CLIRCAR_H. It may optionally be set by command-line
   * instead.
   */
  readEnvironmentVariables();

  processCommandLine(argc,argv);

  if (_BrowseMode)
  {
    AppConfigFile *tAppConfigFile = readAppConfigFile(false);
    runBrowseMode(app,tAppConfigFile);
  }
  else
  {
    AppConfigFile *tAppConfigFile = readAppConfigFile(true);
    runStreamReaderMode(app,tAppConfigFile);
  }

  return app.exec();
}
