#include <cstdio>
#include <unistd.h>
#include <QApplication>
#include "MainWindow.hh"
#include "StreamReader.hh"
#include "Logger.hh"

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void processCommandLine(int argc,char *argv[])
{
  Q_UNUSED(argv);

  if( argc < 2 )
  {
    printf("Usage: pp <in_file>\n");
    exit(0);
  }
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void justBrowse(QApplication &app,int argc,char *argv[])
{
  std::cout << "just browsing" << std::endl;
  MainWindow *window = new MainWindow(argc,argv,app,0);
//  window->setGeometry(1920 + 530,135,625,900);
  window->setGeometry(1920      ,135,900,900);
  window->setupView();
  window->show();
}

/*
 * TODO maybe use some class(es) to better structure everything here in main.
 */
static StreamReader *_StreamReader;
static MainWindow *_MainWindow;
static std::string _StructName;
static bool structTypeHasBeenSet = false;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void launchGUI(std::string aStructName)
{
//_MainWindow->parseHeaderFile(); //TODO mutex issues
  /*
   * Tell the main window what the structure type being processed is, and then
   * tell it to setup its view.
   */
  std::cout << "============launching " << aStructName << std::endl;
  _MainWindow->setInitialStructName(aStructName);
  _MainWindow->setupView();

  /*
   * Give the stream reader the data struct model so that it can start
   * processing incoming lines.
   * TODO maybe can get rid of the getFirstField function to eliminate this
   * step? Need to check if that first field is needed anymore now that
   * RecordProcessor has been implemented.
   */
  std::cout << "============launching" << std::endl;
  _StreamReader->setDataStructModel(_MainWindow->getDataStructModel());

  /*
   * Everthing ready to go, so show the main window.
   */
  std::cout << "============launching" << std::endl;
  _MainWindow->show();
}

//-----------------------------------------------------------------------------
// Used by the stream reader to set the structure name that is being read.
// TODO can this be made into a slot on MainWindow for better efficiency
// than the wait loop?
//-----------------------------------------------------------------------------
void setStructName(std::string aStructName)
{
  std::cout << "============setStructName " << aStructName << std::endl;
  _StructName = aStructName;
  structTypeHasBeenSet = true;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  bool tJustBrowse = false;

  ccl::Logger::initialize();

  QApplication app(argc, argv);

//  processCommandLine(argc,argv);

  if (tJustBrowse)
  {
    justBrowse(app,argc,argv);
  }
  else
  {
    /*
     * Create the stream reader.
     */
    _StreamReader = new StreamReader(&setStructName);

    /*
     * Create the main window. It won't be launched until later though, after
     * the stream reader has determined which data structure type is being read.
     */
    _MainWindow = new MainWindow(argc,argv,app,0,_StreamReader);
    //  window->setGeometry(1920 + 530,135,625,900);
    _MainWindow->setGeometry(1920      ,135,900,900);

    /*
     * Do as much work as possible before starting the stream reader, so that
     * the main window will be as ready to go as possible before the stream
     * reader starts feeding it lines to process.
     */
//    _MainWindow->parseHeaderFile(); //TODO mutex issues


    /*
     *
     */
    QApplication::connect(
        _StreamReader,SIGNAL(structNameAvailable(QString)),
        _MainWindow,SLOT(onStructNameAvailable(QString)));

    QApplication::connect(
        _MainWindow,SIGNAL(dataStructModelAvailable(void *)),
        _StreamReader,SLOT(onDataStructModelAvailable(void *)));

    /*
     * Create the stream reader and start it.
     */
    _StreamReader->start();

#if 0
    /*
     * Wait for the stream reader to determine the structure type before doing
     * the final GUI configuration and launch.
     */
    while (!structTypeHasBeenSet){
      sleep(0.001);
    }
    launchGUI(_StructName);
#endif
  }
  return app.exec();
}
