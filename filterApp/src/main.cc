#include <cstdio>
#include <unistd.h>
#include <QApplication>
#include "MainWindow.hh"
#include "StreamReader.hh"
#include "Logger.hh"

static bool _BrowseMode = false;

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
  }
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void runBrowseMode(QApplication &app,int argc,char *argv[])
{
  MainWindow *window = new MainWindow(argc,argv,app,0);
//  window->setGeometry(1920 + 530,135,625,900);
  window->setGeometry(1920      ,135,900,900);
  window->setupView();
  window->show();
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  ccl::Logger::initialize();

  QApplication app(argc, argv);

  processCommandLine(argc,argv);

  if (_BrowseMode)
  {
    std::cout << "running in browse mode" << std::endl;
    runBrowseMode(app,argc,argv);
  }
  else
  {
    /*
     * Create the main window. It won't be launched until later though, after
     * the stream reader has determined which data structure type is being read.
     */
    MainWindow *tMainWindow = new MainWindow(argc,argv,app,0);
    //  window->setGeometry(1920 + 530,135,625,900);
    tMainWindow->setGeometry(1920      ,135,900,900);

    /*
     * Do as much work as possible before starting the stream reader, so that
     * the main window will be as ready to go as possible before the stream
     * reader starts feeding it lines to process.
     */
    tMainWindow->parseHeaderFile();

    /*
     * Create the stream reader.
     */
    StreamReader * tStreamReader = new StreamReader();

    /*
     *
     */
    QApplication::connect(
        tStreamReader,SIGNAL(structNameAvailable(QString)),
        tMainWindow,SLOT(onStructNameAvailable(QString)));

    QApplication::connect(
        tMainWindow,SIGNAL(dataStructModelAvailable(void *)),
        tStreamReader,SLOT(onDataStructModelAvailable(void *)));

    /*
     * Create the stream reader and start it.
     */
    tStreamReader->start();
  }
  return app.exec();
}
