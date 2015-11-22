#include <cstdio>
#include <unistd.h>
#include <QApplication>
#include "MainWindow.hh"
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
int main(int argc, char *argv[])
{
  ccl::Logger::initialize();

  QApplication app(argc, argv);

  processCommandLine(argc,argv);

//  bool tLaunchGUI = false;

  MainWindow *window = new MainWindow(argc,argv,app,0);
//  window->setGeometry(1920 + 530,135,625,900);
  window->setGeometry(1920      ,135,900,900);
  window->setupView();

  window->show();
  return app.exec();
}
