#include <cstdio>
#include <iostream>
#include <vector>
#include <QFileSystemModel>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include "DataStructModel.hh"
#include "StructorBuilder.hh"
#include "MainWindow.hh"

using namespace std;

extern StructorBuilder *lex_main(char *aHeaderFile);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    int argc, char *argv[], QApplication &aApp,QWidget *aParent)
  : QWidget(aParent),
    _StructorBuilder(0),
    _DataStructModel(0),
    _StructComboBox(0),
    _StructTree(0),
    _Writer(0),
    _StreamReader(0)
{
  _HFile = argv[1];
  _InitialStruct = argv[2];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupView()
{
  //-------------------------
  // Parse the header file.
  //-------------------------
  _StructorBuilder = lex_main((char *) _HFile.c_str());
//   _StructorBuilder->printSummary();
//   _StructorBuilder->postProcess();

  //-------------------------
  // Create structure dropdown list.
  //-------------------------
  std::vector<std::string> tStructNames = _StructorBuilder->getStructNames();
  _StructComboBox = new QComboBox(this);
  _StructComboBox->addItems(convertToQStringList(tStructNames));

  QString tSelectionStr(_InitialStruct.c_str());
  int tSelection = _StructComboBox->findText(tSelectionStr);
  _StructComboBox->setCurrentIndex(tSelection);

  connect(_StructComboBox, SIGNAL(activated(int)), this,
      SLOT(onStructComboBoxActivated(int)));

  //-------------------------
  // Create structure tree view.
  //-------------------------
  _StructTree = new StructTreeView(this);
  setTreeViewStruct(_InitialStruct);
  _StructTree->header()->resizeSection(0, 225);

// TODO works form 4.8 on
#ifdef EXPAND_ALL
  _StructTree->expandAll();
#else
  _StructTree->expand(_StructTree->model()->index(0, 0, QModelIndex()));
#endif

  //-------------------------
  // Pushbutton for setting filter.
  //-------------------------
  QPushButton *tButton = new QPushButton("Set Filter", this);

  connect(tButton, SIGNAL(clicked(bool)), this, SLOT(onSetFilterClicked(bool)));

  //-------------------------
  // Put widgets in the dialog using box layout.
  //-------------------------
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(_StructComboBox);
  layout->addWidget(_StructTree);
  layout->addWidget(tButton);

  setLayout(layout);

  //-------------------------
  // Record writer.
  //-------------------------
  _Writer = new SimpleRecordWriter();

  //-------------------------
  // Stream reader.
  //-------------------------
  _StreamReader = new StreamReader(_DataStructModel, _Writer);
  _StreamReader->start();
}

//-------------------------------------------------------------------------------
// Create a struct data model for the specified struct name and load it in the
// tree.
//-------------------------------------------------------------------------------
void MainWindow::setTreeViewStruct(std::string aStructName)
{
  Structure *tStructure = _StructorBuilder->getStructure(aStructName);
  _DataStructModel = new DataStructModel(tStructure, _StructorBuilder);
  _StructTree->setModel(_DataStructModel);
#if 0 //  seems to have no effect
  _StructTree->resizeColumnToContents(0);
  _StructTree->resizeColumnToContents(1);
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
}
