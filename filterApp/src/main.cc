#include <cstdio>
#include <unistd.h>
#include <QApplication>
#include <QSettings>
#include "MainWindow.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "Logger.hh"

extern StructorBuilder *lex_main(char *aHeaderFile);

static bool _BrowseMode = false;

static std::string _InitialStruct;

std::string _HeaderFile("/opt/idp/cots/iec/rtf/static/CLIR_CAR_cxsd.H");

StructorBuilder *_StructorBuilder(0);

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void processCommandLine(int argc,char *argv[])
{
  for (int tIdx = 0; tIdx < argc; tIdx++)
  {
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
  if(getenv("CLIRCAR_H"))
  {
    _HeaderFile = getenv("CLIRCAR_H");
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void parseHeaderFile()
{
  std::cout << "parsing header file " << _HeaderFile << std::endl;
  _StructorBuilder = lex_main((char *) _HeaderFile.c_str());
  //   _StructorBuilder->printSummary();
  //   _StructorBuilder->postProcess();
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runBrowseMode(QApplication &app)
{
  MainWindow *window = new MainWindow(app,0,_StructorBuilder);
//  window->setGeometry(1920 + 530,135,625,900);
  window->setGeometry(1920      ,135,900,900);
  window->setupView(_InitialStruct);
  window->show();
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runStreamReaderMode(QApplication &app)
{
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
  MainWindow *tMainWindow = new MainWindow(app,0,_StructorBuilder,
      true,tStreamReader,tRecordProcessor);
  // tMainWindow->setGeometry(1920 + 530,135,625,900);
  // tMainWindow->setGeometry(1920      ,135,900,900);
  tMainWindow->setGeometry(1920      ,135,650,700);

  /*
   * Setup connection to let the main window know when the reader has found
   * the type of structure to be processed.
   */
  QApplication::connect(
      tStreamReader,SIGNAL(structNameAvailable(QString)),
      tMainWindow,SLOT(onStructNameAvailable(QString)));

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

void test()
{
  QString tWsdlRoot("/home/bill/workspace/IEC_APB_FA_REALSIM/APB/toolbox/utilities/wsdlTools/");
  QSettings settings(tWsdlRoot + "config/defaults.ini", QSettings::IniFormat);

  QVariant tVariant("test.cfg");
  QVariant tVariant2("aName");
  QVariant tVariant3("aSchool");
  settings.setValue("default_wcf/ssifg_STUDENT_MSG", tVariant);
  settings.setValue("default_wcf/ssifg_STUDENT_MSG.name", tVariant2);
  settings.setValue("default_wcf/ssifg_STUDENT_MSG.school", tVariant3);
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

test();

  /*
   * Read the environment variables.
   * Currently only one: CLIRCAR_H. It may optionally be set by command-line
   * instead.
   */
  readEnvironmentVariables();

  processCommandLine(argc,argv);

  /*
   * Must have a header file specified.
   */
  if (!_HeaderFile.length())
  {
    std::cerr << "ERROR: CLIRCAR_H env var is not set!\n";
    exit(1);
  }
  std::cerr << "using header file " << _HeaderFile << std::endl;

  /*
   * Parse the header file to get _StructBuilder.
   */
  parseHeaderFile();


  if (_BrowseMode)
  {
    std::cout << "running in browse mode" << std::endl;
    runBrowseMode(app);
  }
  else
  {
    runStreamReaderMode(app);
  }
  return app.exec();
}
