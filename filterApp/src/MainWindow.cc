#include <cstdio>
#include <iostream>
#include <vector>
#include <QFileSystemModel>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include "ComboBoxDelegate.hh"
#include "DataStructModel.hh"
#include "StructorBuilder.hh"
#include "TreeProcessor.hh"
#include "MainWindow.hh"

using namespace std;

extern StructorBuilder *lex_main(char *aHeaderFile);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    int argc, char *argv[],QApplication &aApp,QWidget *aParent)
  : QWidget(aParent),
    _StructorBuilder(0),
    _DataStructModel(0),
    _StructComboBox(0),
    _StructTree(0),
#ifdef OLD //TODO
    _Writer(0),
    _StreamReader(0),
#else
    _Writer(0),
#endif
  _HeaderFileWasParsed(false)
{
  Q_UNUSED(aApp);
  Q_UNUSED(argc);

  readEnvironmentVariables();

  processCommandLine(argc,argv);

  if (!_HeaderFile.length())
  {
    std::cerr << "ERROR: CLIRCAR_H env var is not set!\n";
    exit(1);
  }
  std::cerr << "using header file " << _HeaderFile << std::endl;

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
}

//-------------------------------------------------------------------------------
// Create a struct data model for the specified struct name and load it in the
// tree.
//-------------------------------------------------------------------------------
void MainWindow::setTreeViewStruct(std::string aStructName)
{
  Structure *tStructure = _StructorBuilder->getStructure(aStructName);
  if (!tStructure)
  {
    std::cerr << "ERROR: couldn't find struct " << aStructName << std::endl;
  }
  _DataStructModel = new DataStructModel(tStructure, _StructorBuilder);
  _StructTree->setModel(_DataStructModel);
#if 0 //  seems to have no effect
  _StructTree->resizeColumnToContents(0);
  _StructTree->resizeColumnToContents(1);
#endif
}

//-------------------------------------------------------------------------------
// Create a struct data model for the specified struct name and load it in the
// tree.
//-------------------------------------------------------------------------------
void MainWindow::setInitialStructName(std::string aStructName)
{
  _InitialStruct = aStructName;
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void MainWindow::readEnvironmentVariables()
{
  if(getenv("CLIRCAR_H"))
  {
    _HeaderFile = getenv("CLIRCAR_H");
  }
}

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void MainWindow::processCommandLine(int argc,char *argv[])
{
  for (int tIdx = 0; tIdx < argc; tIdx++)
  {
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
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::parseHeaderFile()
{
  if (!_HeaderFileWasParsed)
  {
    _StructorBuilder = lex_main((char *) _HeaderFile.c_str());
    //   _StructorBuilder->printSummary();
    //   _StructorBuilder->postProcess();
    _HeaderFileWasParsed = true;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupView()
{
  /*
   * Parse the header file.
   */
  parseHeaderFile(); // ok if already called before

  /*
   * Create structure dropdown list.
   */
  std::vector<std::string> tStructNames = _StructorBuilder->getStructNames();
  _StructComboBox = new QComboBox(this);
  _StructComboBox->addItems(convertToQStringList(tStructNames));

  QString tSelectionStr(_InitialStruct.c_str());
  int tSelection = _StructComboBox->findText(tSelectionStr);
  _StructComboBox->setCurrentIndex(tSelection);

  connect(_StructComboBox, SIGNAL(activated(int)), this,
      SLOT(onStructComboBoxActivated(int)));

  /*
   * Create structure tree view.
   */
  _StructTree = new StructTreeView(this);
  setTreeViewStruct(_InitialStruct);
  _StructTree->header()->resizeSection(0, 225);

  ComboBoxDelegate *tTestScopeDelegate =
      new ComboBoxDelegate(_DataStructModel->getTestNodes(),this);
  _StructTree->setItemDelegateForColumn(
      DataStructModel::eColTestScope,tTestScopeDelegate);

  ComboBoxDelegate *tFormatDelegate =
      new ComboBoxDelegate(_DataStructModel->getFormats(),this);
  _StructTree->setItemDelegateForColumn(
      DataStructModel::eColFormat,tFormatDelegate);

  ComboBoxDelegate *tPostfixDelegate =
      new ComboBoxDelegate(_DataStructModel->getPostfixes(),this);
  _StructTree->setItemDelegateForColumn(
      DataStructModel::eColPostfix,tPostfixDelegate);



// TODO works form 4.8 on
#ifdef EXPAND_ALL
  _StructTree->expandAll();
#else
  _StructTree->expand(_StructTree->model()->index(0, 0, QModelIndex()));
#endif

  /*
   * Pushbutton for setting filter.
   */
  QPushButton *tButton = new QPushButton("Set Filter", this);

  connect(tButton, SIGNAL(clicked(bool)), this, SLOT(onSetFilterClicked(bool)));

  /*
   * Put widgets in the dialog using box layout.
   */
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(_StructComboBox);
  layout->addWidget(_StructTree);
  layout->addWidget(tButton);

  setLayout(layout);

#if 0 //TODO
  /*
   * Record writer.
   */
  _Writer = new SimpleRecordWriter();

  /*
   * Stream reader.
   */
  _StreamReader = new StreamReader(_DataStructModel, _Writer);
  _StreamReader->start();
#endif
}

//-------------------------------------------------------------------------------
// Convert a vector of strings to a QStringList.
//-------------------------------------------------------------------------------
QStringList MainWindow::convertToQStringList(std::vector<std::string> aStrings)
{
  QStringList tList;
  std::vector<std::string>::iterator tIt;
  for (tIt = aStrings.begin(); tIt != aStrings.end(); tIt++)
  {
    tList.push_back(tIt->c_str());
  }
  return tList;
}

//-------------------------------------------------------------------------------
// Load the newly user-selected structure into the tree view.
//-------------------------------------------------------------------------------
void MainWindow::onStructComboBoxActivated(int index)
{
  QString tString = _StructComboBox->itemText(index);
  setTreeViewStruct(tString.toStdString());
}

void MainWindow::onSetFilterClicked(bool)
{
  std::string tMatchString = _DataStructModel->getMatchString();
  _Writer->setMatchRegex(tMatchString);
#ifdef OLD //TODO rm
TreeProcessor *tProcessor = new TreeProcessor(_DataStructModel->getTopNode());
tProcessor->process();
#endif
  _DataStructModel->printTestNodes();
}
