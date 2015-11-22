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
  MainWindow *window = new MainWindow(argc,argv,app,0);
//  window->setGeometry(1920 + 530,135,625,900);
  window->setGeometry(1920      ,135,900,900);
  window->setupView();
  window->show();
}

StreamReader *_StreamReader;
MainWindow *_MainWindow;

std::string _StructName;

bool isReady = false;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void launchGUI(std::string aStructName)
{
std::cout << "============launching" << std::endl;
//  _MainWindow->setTreeViewStruct(aStructName);
  _MainWindow->setInitialStructName(aStructName);
  _MainWindow->setupView();
  _StreamReader->setDataStructModel(_MainWindow->getDataStructModel());
  _MainWindow->show();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setStructName(std::string aStructName)
{
  std::cout << "============setStructName" << std::endl;
#if 0
  launchGUI(aStructName);
#endif
  _StructName = aStructName;
  isReady = true;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  ccl::Logger::initialize();

  QApplication app(argc, argv);

//  processCommandLine(argc,argv);

//#define JUST_BROWSE
#ifdef JUST_BROWSE
  justBrowse(app,argc,argv);
#else
#if 1
std::cout << "creating new main window" << std::endl;
  _MainWindow = new MainWindow(argc,argv,app,0);
//  window->setGeometry(1920 + 530,135,625,900);
  _MainWindow->setGeometry(1920      ,135,900,900);
//  _MainWindow->setupView();
#endif
//  justBrowse(app,argc,argv);

#if 1
//  StreamReader *tStreamReader = new StreamReader(argc,argv,app,0);
std::cout << "running stream reader" << std::endl;
  _StreamReader = new StreamReader(&setStructName);
  _StreamReader->start();
#endif
#endif
std::cout << "going to check isReady" << std::endl;
  while (!isReady){
std::cout << "NOT READY" << std::endl;
    sleep(1);
  }
  launchGUI(_StructName);
  return app.exec();
}
