#include <cstdio>
#include <iostream>
#include <vector>
#include <QAbstractItemModel>
#include <QActionGroup>
#include <QButtonGroup>
#include <QDockWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include "ComboBoxDelegate.hh"
#include "TestRegexDelegate.hh"
#include "MainWindow.hh"
#include "FileManager.hh"

using namespace std;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    QApplication &aApp,QWidget *aParent,
    StructorBuilder *aStructorBuilder,bool aIsFilterMode,
    StreamReader *aStreamReader,RecordProcessor *aRecordProcessor)
  : QMainWindow(aParent),
    _CentralWidget(0),
    _StructorBuilder(aStructorBuilder),
    _IsFilterMode(aIsFilterMode),
    _StreamReader(aStreamReader),
    _RecordProcessor(aRecordProcessor),
    _DataStructModel(0),
    _StructComboBox(0),
    _ConfigureWidget(0),
    _StructTree(0),
    _PropagateCheckBox(0),
    _CustomFormatToolsWidget(0),
    _FormatAsIsButton(0),
    _FormatLongnameButton(0),
    _FormatTableButton(0),
    _FormatCustomButton(0),
    _OutputNormalButton(0),
    _OutputBypassButton(0),
    _OutputFreezeDropButton(0),
    _OutputFreezeQueueButton(0),
    _AsIsCheckBox(0),
    _LongnameCheckBox(0),
    _TableCheckBox(0)
{
  Q_UNUSED(aApp);

  setStyleSheet(
      "QGroupBox { "
      "border: 1px solid black; "
      "margin-top: 0.5em "
      "}"
      "QGroupBox:title { "
      "subcontrol-origin: margin; "
      "left: 10px; "
      "padding: 0 3px 0 3px"
      "}"
      );
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
  setTreeViewStruct(_StructTree,aStructName);
}

//-------------------------------------------------------------------------------
// Create a struct data model for the specified struct name and load it in the
// tree.
//-------------------------------------------------------------------------------
void MainWindow::setTreeViewStruct(
    StructTreeView *aStructTreeView,std::string aStructName)
{
  Structure *tStructure = _StructorBuilder->getStructure(aStructName);
  if (!tStructure)
  {
    std::cerr << "ERROR: couldn't find struct " << aStructName << std::endl;
  }
  _DataStructModel = new DataStructModel(tStructure, _StructorBuilder);
  aStructTreeView->setModel(_DataStructModel);
#if 0 //  seems to have no effect
  _StructTree->resizeColumnToContents(0);
  _StructTree->resizeColumnToContents(1);
#endif

  connect(_DataStructModel, SIGNAL(modelUpdated()),
      this,SLOT(onModelUpdate()));
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onModelUpdate()
{
//  _StructTree->viewport()->repaint();
  _StructTree->viewport()->update();
}

//-------------------------------------------------------------------------------
// Create a struct data model for the specified struct name and load it in the
// tree.
//-------------------------------------------------------------------------------
void MainWindow::setInitialStructName(std::string aStructName)
{
  _StructName = aStructName;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onFileOpenAction()
{
  FileManager::openConfiguration(_DataStructModel);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onFileSaveAction()
{
  FileManager::saveConfiguration(_DataStructModel);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onOutputModeAction(QAction *aAction)
{
  StreamReader::OutputMode aMode =
      static_cast<StreamReader::OutputMode>(aAction->data().toInt());

  QPalette tPalette;

  switch (aMode) {
    case StreamReader::eNormal:
      tPalette.setColor(QPalette::Window, Qt::white);
      tPalette.setColor(QPalette::WindowText, Qt::black);
      setStatusLabel("Normal",tPalette);
      break;
    case StreamReader::eFreezeDrop:
      tPalette.setColor(QPalette::Window, Qt::darkRed);
      tPalette.setColor(QPalette::WindowText, Qt::white);
      setStatusLabel("Freeze",tPalette);
      break;
    case StreamReader::eBypass:
      tPalette.setColor(QPalette::Window, Qt::darkGray);
      tPalette.setColor(QPalette::WindowText, Qt::white);
      setStatusLabel("Bypass",tPalette);
      break;
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setStatusLabel(QString aStatus,QPalette aPalette)
{
  _StatusLabel->setPalette(aPalette);
  _StatusLabel->setText("Mode: " + aStatus);
}

//-----------------------------------------------------------------------------
// Set up the main GUI window.
// In filter mode, the call to the slot onStructNameAvailable provides the
// struct name and triggers the call to this method. In browse mode, this
// method is called after the MainWindow is instantiated. If the struct name
// is empty in that case (e.g. because no struct name was specified via the
// command-line, then this method will set it to one of the structs in the
// structor builder.
//-----------------------------------------------------------------------------
void MainWindow::setupView(std::string aStructName)
{
  if (_IsFilterMode)
  {
    setupMenuAndToolbar();
  }

   _CentralWidget = new QWidget(this);
   setCentralWidget(_CentralWidget);

  /*
   * Set the name of the structure to be used for the tree view.
   * When running in the filter mode, aStructName should always have a value.
   * But when running in browse mode, if the initial structure for display
   * has not already been set (e.g. via command-line argument), then set it
   * to some initial value.
   */
  if (aStructName.length() == 0)
  {
    _StructName = _StructorBuilder->getStructNames().at(0);
  }
  else
  {
    _StructName = aStructName;
  }

  /*
   * Create structure dropdown list.
   */
  _StructComboBox = createStructComboBox(_CentralWidget);

  /*
   * Create structure tree view.
   * Need to create the tree view here since it creates _DataStructModel, and
   * other methods used below rely on _DataStructModel being set before they
   * are called.
   */
  _StructTree = createTreeView(_CentralWidget);

  /*
   * The options will only be populated when in filter mode.
   */
  if (_IsFilterMode)
  {
    /*
     * Create bypass checkbox and connect it to the stream reader.
     */
    _PropagateCheckBox = new QCheckBox(
        "Propagate check on field name node to children",_CentralWidget);

    connect(_PropagateCheckBox,SIGNAL(toggled(bool)),
        _DataStructModel,SLOT(onPropogateToggled(bool)));
  }

  /*
   * Put widgets in the dialog using box layout.
   */
  QVBoxLayout *tWindowLayout = new QVBoxLayout;
  tWindowLayout->addWidget(_StructComboBox);
  if (_IsFilterMode)
  {
    tWindowLayout->addWidget(_PropagateCheckBox);
  }
  tWindowLayout->addWidget(_StructTree);

  _CentralWidget->setLayout(tWindowLayout);

  // always hide these debug columns
  _StructTree->hideColumn(DataStructModel::eColFieldKey);
  _StructTree->hideColumn(DataStructModel::eColMatchRegex);

  if (!_IsFilterMode)
  {
    _StructTree->hideColumn(DataStructModel::eColFormat);
    _StructTree->hideColumn(DataStructModel::eColMatchRegex);
    _StructTree->hideColumn(DataStructModel::eColPostfix);
    _StructTree->hideColumn(DataStructModel::eColTestRegex);
    _StructTree->hideColumn(DataStructModel::eColTestScope);
  }
  else
  {
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupOperateActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixmaps
  QPixmap tGreenLight("green.png");
  QPixmap tRedLight("red.png");
  //   QPixmap tBypass("bypass3.png");
  QPixmap tBypass("bypass_arrow.png");

  // actions
  QAction *tGoAction = new QAction(QIcon(tGreenLight),"&Go", this);
  QAction *tStopAction = new QAction(QIcon(tRedLight),"&Stop", this);
  QAction *tBypassAction = new QAction(QIcon(tBypass),"&Bypass", this);

  // short-cuts
  tGoAction->setShortcut(QKeySequence(Qt::Key_G));
  tStopAction->setShortcut(QKeySequence(Qt::Key_S));
  tBypassAction->setShortcut(QKeySequence(Qt::Key_B));

  // check boxes
  tGoAction->setCheckable(true);
  tStopAction->setCheckable(true);
  tBypassAction->setCheckable(true);

  tGoAction->setChecked(true);

  // action data for action group trigger
  tGoAction->setData(QVariant(StreamReader::eNormal));
  tStopAction->setData(QVariant(StreamReader::eFreezeDrop));
  tBypassAction->setData(QVariant(StreamReader::eBypass));

  // action group
  QActionGroup *tRunGroup = new QActionGroup(this);
  tRunGroup->addAction(tGoAction);
  tRunGroup->addAction(tStopAction);
  tRunGroup->addAction(tBypassAction);

  // action signal-slot
  connect(tRunGroup, SIGNAL(triggered(QAction*)), _StreamReader, SLOT(onOutputModeAction(QAction*)));
  connect(tRunGroup, SIGNAL(triggered(QAction*)), this, SLOT(onOutputModeAction(QAction*)));

  // menu
  aMenu = menuBar()->addMenu("&Operate");
  aMenu->addAction(tGoAction);
  aMenu->addAction(tStopAction);
  aMenu->addAction(tBypassAction);

  // toolbar
  aToolBar->addAction(tGoAction);
  aToolBar->addAction(tStopAction);
  aToolBar->addAction(tBypassAction);

  onOutputModeAction(tGoAction); // initializes status label to the default //TODO ok here?
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupDelimitActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixamps
  QPixmap tDelimAll("delim_all.png");
  QPixmap tDelimOut("delim_out.png");
  QPixmap tDelimNone("delim_none.png");

  // actions
  QAction *tDelimitOutputAction = new QAction(QIcon(tDelimOut),"&Delimit Output Records", this);
  QAction *tDelimitNoneAction = new QAction(QIcon(tDelimNone),"&No Record Delimiters", this);
  QAction *tDelimitAllAction = new QAction(QIcon(tDelimAll),"&Delimit All Records", this);

  // check boxes
  tDelimitAllAction->setCheckable(true);
  tDelimitOutputAction->setCheckable(true);
  tDelimitNoneAction->setCheckable(true);

  tDelimitOutputAction->setChecked(true);

  // action data for action group trigger
  tDelimitAllAction->setData(QVariant(StreamReader::eAllRecords));
  tDelimitOutputAction->setData(QVariant(StreamReader::eOutputRecords));
  tDelimitNoneAction->setData(QVariant(StreamReader::eNoRecords));

  // action group
  QActionGroup *tDelimitGroup = new QActionGroup(this);
  tDelimitGroup->addAction(tDelimitOutputAction);
  tDelimitGroup->addAction(tDelimitNoneAction);
  tDelimitGroup->addAction(tDelimitAllAction);

  // action signal-slot
  connect(tDelimitGroup, SIGNAL(triggered(QAction*)), _StreamReader, SLOT(onDelimitModeAction(QAction*)));

  // menu
  aMenu = menuBar()->addMenu("&Delimit Records");
  aMenu->addAction(tDelimitOutputAction);
  aMenu->addAction(tDelimitNoneAction);
  aMenu->addAction(tDelimitAllAction);

  // toolbar
  aToolBar->addAction(tDelimitOutputAction);
  aToolBar->addAction(tDelimitNoneAction);
  aToolBar->addAction(tDelimitAllAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupFormatActions(QMenu *aMenu,QToolBar *aToolBar)
{
  //pixmaps
  QPixmap tFormatAsIs("fmt_asis.png");
  QPixmap tFormatLongname("fmt_longname.png");
  QPixmap tFormatTable("fmt_table.png");
  QPixmap tFormatCustom("fmt_custom.png");

  // actions
  QAction *tAsIsFormatAction = new QAction(QIcon(tFormatAsIs),"&As-is", this);
  QAction *tLongnameFormatAction = new QAction(QIcon(tFormatLongname),"&Long name", this);
  QAction *tTableFormatAction = new QAction(QIcon(tFormatTable),"&Table", this);
  QAction *tCustomFormatAction = new QAction(QIcon(tFormatCustom),"&Custom", this);

  // short-cuts
  tAsIsFormatAction->setShortcut(QKeySequence(Qt::Key_A));
  tLongnameFormatAction->setShortcut(QKeySequence(Qt::Key_L));
  tTableFormatAction->setShortcut(QKeySequence(Qt::Key_T));
  tCustomFormatAction->setShortcut(QKeySequence(Qt::Key_C));

  // check boxes
  tAsIsFormatAction->setCheckable(true);
  tLongnameFormatAction->setCheckable(true);
  tTableFormatAction->setCheckable(true);
  tCustomFormatAction->setCheckable(true);

  tAsIsFormatAction->setChecked(true);

  // action data for action group trigger
  tAsIsFormatAction->setData(QVariant(RecordProcessor::eAsIs));
  tLongnameFormatAction->setData(QVariant(RecordProcessor::eLongname));
  tTableFormatAction->setData(QVariant(RecordProcessor::eTable));
  tCustomFormatAction->setData(QVariant(RecordProcessor::eCustom));

  // action group
  QActionGroup *tFormatGroup = new QActionGroup(this);
  tFormatGroup->addAction(tAsIsFormatAction);
  tFormatGroup->addAction(tLongnameFormatAction);
  tFormatGroup->addAction(tTableFormatAction);
  tFormatGroup->addAction(tCustomFormatAction);

  // action signal-slot
  connect(tFormatGroup, SIGNAL(triggered(QAction*)), _RecordProcessor, SLOT(onFormatModeAction(QAction*)));

  // menu
  aMenu = menuBar()->addMenu("&Format Mode");
  aMenu->addAction(tAsIsFormatAction);
  aMenu->addAction(tLongnameFormatAction);
  aMenu->addAction(tTableFormatAction);
  aMenu->addAction(tCustomFormatAction);

  // toolbar
  aToolBar->addAction(tAsIsFormatAction);
  aToolBar->addAction(tLongnameFormatAction);
  aToolBar->addAction(tTableFormatAction);
  aToolBar->addAction(tCustomFormatAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupToolActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixamps
  QPixmap tCustomFormat("custom_fmt_tool.png");

  // actions
  QAction *tCustomFormatAction =
      new QAction(QIcon(tCustomFormat),"Custom format tool", this);

  // check boxes
  tCustomFormatAction->setCheckable(false);

  // action signal-slot
  connect(tCustomFormatAction,SIGNAL(triggered()),
      this,SLOT(onCustomFormatToolAction()));

  // menu
  aMenu = menuBar()->addMenu("&Tools");
  aMenu->addAction(tCustomFormatAction);

  // toolbar
  aToolBar->addAction(tCustomFormatAction);
}

//-----------------------------------------------------------------------------
// TODO move
//-----------------------------------------------------------------------------
void MainWindow::setupFileActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixamps
  QPixmap tFileOpen("document_open.png");
  QPixmap tFileSave("document_save.png");

  // actions
  QAction *tFileOpenAction =
      new QAction(QIcon(tFileOpen),"Open &file", this);
  QAction *tFileSaveAction =
      new QAction(QIcon(tFileSave),"Save &file", this);

  // short-cuts
  tFileOpenAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
  tFileSaveAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));

  // check boxes
  tFileOpenAction->setCheckable(false);
  tFileSaveAction->setCheckable(false);

  // action signal-slot
  connect(tFileOpenAction,SIGNAL(triggered()),
      this,SLOT(onFileOpenAction()));
  connect(tFileSaveAction,SIGNAL(triggered()),
      this,SLOT(onFileSaveAction()));

  // menu
  aMenu = menuBar()->addMenu("&File");
  aMenu->addAction(tFileOpenAction);
  aMenu->addAction(tFileSaveAction);

  // toolbar
  aToolBar->addAction(tFileOpenAction);
  aToolBar->addAction(tFileSaveAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupMenuAndToolbar()
{
  QMenu *tMenu = new QMenu();
  QToolBar *tToolBar = addToolBar("Main Toolbar");

  _StatusLabel = new QLabel();
  _StatusLabel->setAutoFillBackground(true);

  statusBar()->addWidget(_StatusLabel);
  statusBar()->addPermanentWidget(new QLabel("-counts-"));

  setupFileActions(tMenu,tToolBar);
  tToolBar->addSeparator();
  setupOperateActions(tMenu,tToolBar);
  tToolBar->addSeparator();
  setupDelimitActions(tMenu,tToolBar);
  tToolBar->addSeparator();
  setupFormatActions(tMenu,tToolBar);
  tToolBar->addSeparator();
  setupToolActions(tMenu,tToolBar);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StructTreeView *MainWindow::createTreeView(QWidget *aParent)
{

  StructTreeView *tTreeView = new StructTreeView(aParent);
  setTreeViewStruct(tTreeView,_StructName);
  tTreeView->header()->resizeSection(DataStructModel::eColFieldName,225);
  tTreeView->header()->resizeSection(DataStructModel::eColTestRegex,225);
  tTreeView->header()->resizeSection(DataStructModel::eColTestScope,175);
  tTreeView->header()->resizeSection(DataStructModel::eColFormat,100);
  tTreeView->header()->resizeSection(DataStructModel::eColPostfix,75);

  tTreeView->setEditTriggers(QAbstractItemView::AllEditTriggers);

  TestRegexDelegate *tTestRegexDelegate =
      new TestRegexDelegate (_DataStructModel,this);
  tTreeView->setItemDelegateForColumn(
      DataStructModel::eColTestRegex,tTestRegexDelegate);

  ComboBoxDelegate *tTestScopeDelegate =
      new ComboBoxDelegate(_DataStructModel->getTestNodes(),this);
  tTreeView->setItemDelegateForColumn(
      DataStructModel::eColTestScope,tTestScopeDelegate);

  ComboBoxDelegate *tFormatDelegate =
      new ComboBoxDelegate(_DataStructModel->getFormats(),this);
  tTreeView->setItemDelegateForColumn(
      DataStructModel::eColFormat,tFormatDelegate);

  ComboBoxDelegate *tPostfixDelegate =
      new ComboBoxDelegate(_DataStructModel->getPostfixes(),this);
  tTreeView->setItemDelegateForColumn(
      DataStructModel::eColPostfix,tPostfixDelegate);

// TODO works form 4.8 on
#ifdef EXPAND_ALL
  tTreeView->expandAll();
#else
  tTreeView->expand(tTreeView->model()->index(0, 0, QModelIndex()));
#endif

  return tTreeView;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QComboBox *MainWindow::createStructComboBox(QWidget *aParent)
{
  std::vector<std::string> tStructNames = _StructorBuilder->getStructNames();

  QComboBox *tComboBox = new QComboBox(aParent);
  tComboBox->addItems(convertToQStringList(tStructNames));

  QString tSelectionStr(_StructName.c_str());
  int tSelection = tComboBox->findText(tSelectionStr);
  tComboBox->setCurrentIndex(tSelection);

  connect(tComboBox, SIGNAL(activated(int)), this,
      SLOT(onStructComboBoxActivated(int)));

  return tComboBox;
}

//-----------------------------------------------------------------------------
// Creates the options widget.
//-----------------------------------------------------------------------------
QWidget *MainWindow::createConfigureWidget(QWidget *aParent)
{
    QWidget *tConfigure = new QWidget(aParent);

    QGroupBox *tCustomFormatGroup = createCustomFormatGroup(tConfigure);
    tCustomFormatGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QHBoxLayout *tLayout = new QHBoxLayout;
    tLayout->addWidget(tCustomFormatGroup);
    tLayout->setAlignment(tCustomFormatGroup,Qt::AlignTop);

    tConfigure->setLayout(tLayout);

    return tConfigure;
}

//-------------------------------------------------------------------------------
// Note that tree view must be created before this is called. Otherwise,
// _DataStructMode will be null, and so the connections won't be made.
//-------------------------------------------------------------------------------
QGroupBox *MainWindow::createCustomFormatGroup(QWidget *aParent)
{
    QGroupBox *tGroup = new QGroupBox("Custom Format Utilities",aParent);
    tGroup->setFlat(true);

    QPushButton *tAsIsButton =
        new QPushButton("Apply \"As-is\" settings",tGroup);
    QPushButton *tLongnameButton =
        new QPushButton("Apply \"Longname\" settings",tGroup);
    QPushButton *tTableButton =
        new QPushButton("Apply \"Table\" settings",tGroup);

    _AsIsCheckBox = new QCheckBox("Checked fields only");
    _LongnameCheckBox = new QCheckBox("Checked fields only");
    _TableCheckBox = new QCheckBox("Checked fields only");

    connect(tAsIsButton,SIGNAL(clicked(bool)),
      this,SLOT(onAsIsPushbuttonClicked(bool)));
    connect(tLongnameButton,SIGNAL(clicked(bool)),
      this,SLOT(onLongnamePushbuttonClicked(bool)));
    connect(tTableButton,SIGNAL(clicked(bool)),
      this,SLOT(onTablePushbuttonClicked(bool)));

    connect(this,SIGNAL(applyFormatMode(int,bool)),
      _DataStructModel,SLOT(applyFormatMode(int,bool)));

    QGridLayout *tGrid = new QGridLayout(aParent);
    tGrid->addWidget(tAsIsButton,0,0);
    tGrid->addWidget(tLongnameButton,1,0);
    tGrid->addWidget(tTableButton,2,0);
    tGrid->addWidget(_AsIsCheckBox,0,1);
    tGrid->addWidget(_LongnameCheckBox,1,1);
    tGrid->addWidget(_TableCheckBox,2,1);

    tGroup->setLayout(tGrid);

    return tGroup;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onAsIsPushbuttonClicked(bool)
{
  int tAsIsInt = (int)RecordProcessor::eAsIs;
  bool tChecked = (_AsIsCheckBox->isChecked() ? true: false);
  emit applyFormatMode(tAsIsInt,tChecked);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onLongnamePushbuttonClicked(bool)
{
  int tLongnameInt = (int)RecordProcessor::eLongname;
  bool tChecked = (_LongnameCheckBox->isChecked() ? true: false);
  emit applyFormatMode(tLongnameInt,tChecked);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onTablePushbuttonClicked(bool)
{
  int tTableInt = (int)RecordProcessor::eTable;
  bool tChecked = (_TableCheckBox->isChecked() ? true: false);
  emit applyFormatMode(tTableInt,tChecked);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onCustomFormatToolAction()
{
#if 1
  /*
   * Dock.
   */
    _ConfigureWidget = createConfigureWidget(0);

//   QDockWidget *shapesDockWidget = new QDockWidget(QString("Shapes"));
   _CustomFormatToolsWidget = new QDockWidget(0);
   _CustomFormatToolsWidget->setObjectName("shapesDockWidget");
   _CustomFormatToolsWidget->setWidget(_ConfigureWidget);
   _CustomFormatToolsWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
#endif

  addDockWidget(Qt::TopDockWidgetArea, _CustomFormatToolsWidget);
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
// Slot used in stream reader mode to trigger the main display.
// The stream reader calls this once it has determined what data structure is
// being it is going to process.
//-------------------------------------------------------------------------------
void MainWindow::onStructNameAvailable(QString aStructName)
{
  /*
   * Need to set the structure name before setting up the view and launching
   * the window.
   */
  setInitialStructName(aStructName.toStdString());
  setupView(_StructName);

  /*
   * Give the stream reader the data struct model so that it can start
   * processing incoming lines.
   * TODO maybe can get rid of the getFirstField function to eliminate this
   * step? Need to check if that first field is needed anymore now that
   * RecordProcessor has been implemented.
   */
  emit dataStructModelAvailable(static_cast<void *>(_DataStructModel));

  /*
   * Everything ready to go, so show the main window.
   */
  show();
}

//-------------------------------------------------------------------------------
// Load the newly user-selected structure into the tree view.
//-------------------------------------------------------------------------------
void MainWindow::onStructComboBoxActivated(int index)
{
  QString tString = _StructComboBox->itemText(index);
  setTreeViewStruct(tString.toStdString());
}
