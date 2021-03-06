#include <cstdio>
#include <iostream>
#include <vector>
#include <QAbstractItemModel>
#include <QActionGroup>
#include <QButtonGroup>
#include <QDockWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

#include "HeaderUtil.hh"
#include "PickFilterDialog.hh"
#include "MainWindow.hh"

using namespace std;

const QString MainWindow::kAppName("app_iec_wsdlFilter");
const QString MainWindow::kTitle(kAppName);

//-----------------------------------------------------------------------------
// Constructor for browse mode.
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    QApplication &aApp,QWidget *aParent,AppConfig aAppConfig,
      MessageSpecMap aMessageSpecMap,StructorBuilder *aStructorBuilder)
  : QMainWindow(aParent),
    _AppConfig(aAppConfig),
    _MessageSpecMap(aMessageSpecMap),
    _StructorBuilder(aStructorBuilder),
    _IsFilterMode(false),
    _StreamReader(0),
    _RecordProcessor(0)
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

  init();
}

//-----------------------------------------------------------------------------
// Constructor for filter mode.
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    QApplication &aApp,QWidget *aParent,AppConfig aAppConfig,
      MessageSpecMap aMessageSpecMap,StreamReader *aStreamReader,
      RecordProcessor *aRecordProcessor)
  : QMainWindow(aParent),
    _AppConfig(aAppConfig),
    _MessageSpecMap(aMessageSpecMap),
    _StructorBuilder(0),
    _IsFilterMode(true),
    _StreamReader(aStreamReader),
    _RecordProcessor(aRecordProcessor)
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

  init();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::~MainWindow()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::init()
{
  _FileToolBar = 0;
  _OperateToolBar = 0;
  _DelimitToolBar = 0;
  _FormatToolBar = 0;
  _ToolToolBar = 0;
  _OperateActionGroup = 0;
  _DelimitActionGroup = 0;
  _FormatActionGroup = 0;
  _ViewFileToolbarAction = 0;
  _ViewOperateToolbarAction = 0;
  _ViewDelimitToolbarAction = 0;
  _ViewFormatToolbarAction = 0;
  _ViewToolToolbarAction = 0;
  _PropagateCheckAction = 0;
  _CentralWidget = 0;
  _DataStructModel = 0;
  _StructComboBox = 0;
  _CustomFormatToolWidget = 0;
  _StructTree = 0;
  _CustomFormatToolDock = 0;
  _AsIsCheckBox = 0;
  _LongnameCheckBox = 0;
  _TableCheckBox = 0;

  _FilterReader = new FilterReader();
}

//-------------------------------------------------------------------------------
// Create a struct data model for the specified struct name and load it in the
// tree.
//-------------------------------------------------------------------------------
void MainWindow::setupDataStructModel(std::string aStructName)
{
  Structure *tStructure = _StructorBuilder->getStructure(aStructName);
  if (!tStructure)
  {
    std::cerr << "ERROR: couldn't find struct " << aStructName << std::endl;
    exit(0);
  }

  /*
   * Create the data structure model for the specified data structure.
   */
  _DataStructModel = new DataStructModel(tStructure, _StructorBuilder);

  /*
   * Setup connection to monitor updates to the data struct model.
   */
  connect(_DataStructModel, SIGNAL(modelUpdated()),
      this,SLOT(onModelUpdate()));

  /*
   * Set connection to apply test scope settings to data struct model.
   */
  connect(this,SIGNAL(applyTestScope(QString,bool)),
      _DataStructModel,SLOT(onApplyTestScope(QString,bool)));

  /*
   * Set connection to apply format mode setting to data struct model.
   */
  connect(this,SIGNAL(applyFormatMode(int,bool,QModelIndexList)),
      _DataStructModel,SLOT(applyFormatMode(int,bool,QModelIndexList)));

  /*
   * Set connection to inform the data struct model of the "propagate
   * checkbox" status.
   */
  connect(this,SIGNAL(togglePropagateChecks(bool)),
      _DataStructModel,SLOT(onPropogateToggled(bool)));
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
void MainWindow::onOpenFilterAction()
{
  PickFilterDialog *tDialog = new PickFilterDialog(
      this,_StructName,_AppConfig,_FilterReader);

  /*
   * If user selected cancel, nothing to do.
   */
  if (tDialog->exec() == 0)
  {
    return;
  }

  /*
   * If user selected a filter, read it and apply it.
   */
  QString tFilter = tDialog->getSelectedFilter();
  if (tFilter.length())
  {
//    std::cout << "User selected filter " <<  qPrintable(tFilter) << std::endl;
    FilterSpec tFiter = _FilterReader->openFilter(
        _AppConfig.getDefaultFiltersDir(),tFilter);

    tFiter.apply(_DataStructModel,
        _OperateActionGroup,_DelimitActionGroup,_FormatActionGroup);

    _StructTree->expandToChecked(true);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onOpenCustomFilterAction()
{
  FilterSpec tFiter = _FilterReader->openFilter(
      _AppConfig.getCustomFiltersDir());

  tFiter.apply(_DataStructModel,
      _OperateActionGroup,_DelimitActionGroup,_FormatActionGroup);

    _StructTree->expandToChecked(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onSaveCustomFilterAction()
{
  _FilterReader->saveFilter(_AppConfig.getCustomFiltersDir(),_DataStructModel,
      _StreamReader,_RecordProcessor);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onOperateModeAction(QAction *aAction)
{
  StreamReader::OperateMode aMode =
      static_cast<StreamReader::OperateMode>(aAction->data().toInt());

  QPalette tPalette;

  switch (aMode) {
    case StreamReader::eGo:
      tPalette.setColor(QPalette::Window, Qt::white);
      tPalette.setColor(QPalette::WindowText, Qt::black);
      setStatusLabel("Go",tPalette);
      break;
    case StreamReader::eStop:
      tPalette.setColor(QPalette::Window, Qt::darkRed);
      tPalette.setColor(QPalette::WindowText, Qt::white);
      setStatusLabel("Stop",tPalette);
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

  setupDataStructModel(_StructName);

//  if (_IsFilterMode)
//  {
    setupMenuAndToolbar();
//  }

   _CentralWidget = new QWidget(this);
   setCentralWidget(_CentralWidget);

  /*
   * Create structure dropdown list.
   */
  if (!_IsFilterMode)
  {
    _StructComboBox = createStructComboBox(_CentralWidget);
  }

  /*
   * Create structure tree view.
   * Need to create the tree view here since it creates _DataStructModel, and
   * other methods used below rely on _DataStructModel being set before they
   * are called.
   */
  _StructTree = new StructTreeView(_CentralWidget,_DataStructModel);

  /*
   * Put widgets in the dialog using box layout.
   */
  QVBoxLayout *tWindowLayout = new QVBoxLayout;
  if (!_IsFilterMode)
  {
    tWindowLayout->addWidget(_StructComboBox);
  }
  tWindowLayout->addWidget(_StructTree);

  _CentralWidget->setLayout(tWindowLayout);

  // always hide these debug columns
  _StructTree->hideColumn(DataStructModel::eColFieldKey);
  _StructTree->hideColumn(DataStructModel::eColMatchRegex);

  if (!_IsFilterMode)
  {
//    _StructTree->hideColumn(DataStructModel::eColFormat);
//    _StructTree->hideColumn(DataStructModel::eColMatchRegex);
//    _StructTree->hideColumn(DataStructModel::eColPostfix);
//    _StructTree->hideColumn(DataStructModel::eColTestRegex);
//    _StructTree->hideColumn(DataStructModel::eColTestScope);//TODO restore
  }
  else
  {
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupFileActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixamps
  QPixmap tFileOpen(":/document_open.png");
  QPixmap tFileSave(":/document_save.png");

  // actions
  QAction *tOpenFilterAction =
      new QAction(QIcon(tFileOpen/*TODO icon*/),"Open filter", this);
  QAction *tOpenCustomFilterAction =
      new QAction(QIcon(tFileOpen),"Open custom filter", this);
  QAction *tSaveCustomFilterAction =
      new QAction(QIcon(tFileSave),"Save custom filter", this);

  // short-cuts
  tOpenFilterAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
  tOpenCustomFilterAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
  tSaveCustomFilterAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

  // check boxes
  tOpenFilterAction->setCheckable(false);
  tOpenCustomFilterAction->setCheckable(false);
  tSaveCustomFilterAction->setCheckable(false);

  // action signal-slot
  connect(tOpenFilterAction,SIGNAL(triggered()),
      this,SLOT(onOpenFilterAction()));
  connect(tOpenCustomFilterAction,SIGNAL(triggered()),
      this,SLOT(onOpenCustomFilterAction()));
  connect(tSaveCustomFilterAction,SIGNAL(triggered()),
      this,SLOT(onSaveCustomFilterAction()));

  // menu
  aMenu = menuBar()->addMenu("&File");
  aMenu->addAction(tOpenFilterAction);
  aMenu->addAction(tOpenCustomFilterAction);
  aMenu->addAction(tSaveCustomFilterAction);

  // toolbar
  aToolBar->addAction(tOpenFilterAction);
  aToolBar->addAction(tOpenCustomFilterAction);
  aToolBar->addAction(tSaveCustomFilterAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupOperateActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixmaps
  QPixmap tGreenLight(":/green_light.png");
  QPixmap tRedLight(":/red_light.png");
  QPixmap tBypass(":/bypass.png");
  QPixmap tEnter(":/enter.png");

  // actions
  QAction *tGoAction = new QAction(QIcon(tGreenLight),"&Go", this);
  QAction *tStopAction = new QAction(QIcon(tRedLight),"&Stop", this);
  QAction *tBypassAction = new QAction(QIcon(tBypass),"&Bypass", this);
  QAction *tEnterAction = new QAction(QIcon(tEnter),"&Enter Space", this);

  // short-cuts
  tGoAction->setShortcut(QKeySequence(Qt::Key_G));
  tStopAction->setShortcut(QKeySequence(Qt::Key_S));
  tBypassAction->setShortcut(QKeySequence(Qt::Key_B));
  tEnterAction->setShortcut(QKeySequence(Qt::Key_E));

  // check boxes
  tGoAction->setCheckable(true);
  tStopAction->setCheckable(true);
  tBypassAction->setCheckable(true);
  tEnterAction->setCheckable(false);

  // action data for action group trigger
  tGoAction->setData(QVariant(StreamReader::eGo));
  tStopAction->setData(QVariant(StreamReader::eStop));
  tBypassAction->setData(QVariant(StreamReader::eBypass));

  // action group
  _OperateActionGroup = new OperateActionGroup(this);
  _OperateActionGroup->addAction(tGoAction);
  _OperateActionGroup->addAction(tStopAction);
  _OperateActionGroup->addAction(tBypassAction);

  // action signal-slot
  connect(_OperateActionGroup, SIGNAL(triggered(QAction*)), _StreamReader,
      SLOT(onOperateModeAction(QAction*)));
  connect(_OperateActionGroup, SIGNAL(triggered(QAction*)), this,
      SLOT(onOperateModeAction(QAction*)));

  connect(tEnterAction,SIGNAL(triggered()),
      _StreamReader,SLOT(onEnterSpaceAction()));

  // menu
  aMenu = menuBar()->addMenu("&Operate");
  aMenu->addAction(tGoAction);
  aMenu->addAction(tStopAction);
  aMenu->addAction(tBypassAction);
  aMenu->addSeparator();
  aMenu->addAction(tEnterAction);

  // toolbar
  aToolBar->addAction(tGoAction);
  aToolBar->addAction(tStopAction);
  aToolBar->addAction(tBypassAction);
  aToolBar->addSeparator();
  aToolBar->addAction(tEnterAction);

  // activate the default action
  QAction *tDefaultAction = tGoAction;
  if (_StreamReader) // is available in filter mode
  {
    tDefaultAction = _OperateActionGroup->getAction(
        _StreamReader->getOperateMode());
  }
  tDefaultAction->setChecked(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupDelimitActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixamps
  QPixmap tDelimAll(":/delim_all.png");
  QPixmap tDelimOut(":/delim_out.png");
  QPixmap tDelimNone(":/delim_none.png");

  // actions
  QAction *tDelimitOutputAction = new QAction(QIcon(tDelimOut),
      "&Delimit Output Records", this);
  QAction *tDelimitNoneAction = new QAction(QIcon(tDelimNone),
      "&No Record Delimiters", this);
  QAction *tDelimitAllAction = new QAction(QIcon(tDelimAll),
      "&Delimit All Records", this);

  // check boxes
  tDelimitAllAction->setCheckable(true);
  tDelimitOutputAction->setCheckable(true);
  tDelimitNoneAction->setCheckable(true);

  // action data for action group trigger
  tDelimitAllAction->setData(QVariant(StreamReader::eAllRecords));
  tDelimitOutputAction->setData(QVariant(StreamReader::eOutputRecords));
  tDelimitNoneAction->setData(QVariant(StreamReader::eNoRecords));

  // action group
  _DelimitActionGroup = new DelimitActionGroup(this);
  _DelimitActionGroup->addAction(tDelimitOutputAction);
  _DelimitActionGroup->addAction(tDelimitNoneAction);
  _DelimitActionGroup->addAction(tDelimitAllAction);

  // action signal-slot
  connect(_DelimitActionGroup, SIGNAL(triggered(QAction*)), _StreamReader,
      SLOT(onDelimitModeAction(QAction*)));

  // menu
  aMenu = menuBar()->addMenu("&Delimit");
  aMenu->addAction(tDelimitOutputAction);
  aMenu->addAction(tDelimitNoneAction);
  aMenu->addAction(tDelimitAllAction);

  // toolbar
  aToolBar->addAction(tDelimitOutputAction);
  aToolBar->addAction(tDelimitNoneAction);
  aToolBar->addAction(tDelimitAllAction);

  // activate the default action
  QAction *tDefaultAction = tDelimitOutputAction;
  if (_StreamReader) // is available in filter mode
  {
    tDefaultAction = _DelimitActionGroup->getAction(
        _StreamReader->getDelimitMode());
  }
  tDefaultAction->setChecked(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupFormatActions(QMenu *aMenu,QToolBar *aToolBar)
{
  //pixmaps
  QPixmap tFormatAsIs(":/fmt_asis.png");
  QPixmap tFormatLongname(":/fmt_longname.png");
  QPixmap tFormatTable(":/fmt_table.png");
  QPixmap tFormatCustom(":/fmt_custom.png");
  QPixmap tCustomFormat(":/custom_fmt_tool.png");

  // actions
  QAction *tAsIsFormatAction = new QAction(QIcon(tFormatAsIs),
      "&As-is Format Mode", this);
  QAction *tLongnameFormatAction = new QAction(QIcon(tFormatLongname),
      "&Long-name Format Mode", this);
  QAction *tTableFormatAction = new QAction(QIcon(tFormatTable),
      "&Table Format Mode", this);
  QAction *tCustomFormatAction = new QAction(QIcon(tFormatCustom),
      "&Custom Format Mode", this);
  QAction *tCustomFormatToolAction = new QAction(QIcon(tCustomFormat),
      "Custom Format Tool...", this);

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
  tCustomFormatToolAction->setCheckable(false);

  tCustomFormatAction->setChecked(true);

  // action data for action group trigger
  tAsIsFormatAction->setData(QVariant(RecordProcessor::eAsIs));
  tLongnameFormatAction->setData(QVariant(RecordProcessor::eLongname));
  tTableFormatAction->setData(QVariant(RecordProcessor::eTable));
  tCustomFormatAction->setData(QVariant(RecordProcessor::eCustom));

  // action group
  _FormatActionGroup = new FormatActionGroup(this);
  _FormatActionGroup->addAction(tAsIsFormatAction);
  _FormatActionGroup->addAction(tLongnameFormatAction);
  _FormatActionGroup->addAction(tTableFormatAction);
  _FormatActionGroup->addAction(tCustomFormatAction);

  // action signal-slot
  connect(_FormatActionGroup, SIGNAL(triggered(QAction*)), _RecordProcessor,
      SLOT(onFormatModeAction(QAction*)));
  connect(tCustomFormatToolAction,SIGNAL(triggered()),
      this,SLOT(onCustomFormatToolAction()));

  // menu
  aMenu = menuBar()->addMenu("&Format");
  aMenu->addAction(tAsIsFormatAction);
  aMenu->addAction(tLongnameFormatAction);
  aMenu->addAction(tTableFormatAction);
  aMenu->addAction(tCustomFormatAction);
  aMenu->addSeparator();
  aMenu->addAction(tCustomFormatToolAction);

  // toolbar
  aToolBar->addAction(tAsIsFormatAction);
  aToolBar->addAction(tLongnameFormatAction);
  aToolBar->addAction(tTableFormatAction);
  aToolBar->addAction(tCustomFormatAction);
  aToolBar->addSeparator();
  aToolBar->addAction(tCustomFormatToolAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupTreeActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixamps
  QPixmap tClearAllPixmap(":/clear_all.png");
  QPixmap tPropagateCheckPixmap(":/propagate_check.png");
  QPixmap tExpandToChecked(":/expand_checks.png");
  QPixmap tCollapseAllPixmap(":/collapse_all.png");
  QPixmap tExpandAllPixmap(":/expand_all.png");

  // actions
  QAction *tClearAllAction = new QAction(QIcon(tClearAllPixmap),
          "Clear all tree settings", this);
  _PropagateCheckAction =
      new QAction(QIcon(tPropagateCheckPixmap),
          "Propagate check to descendants", this);
  QAction *tExpandToCheckedAction = new QAction(QIcon(tExpandToChecked),
          "Expand to checked fields", this);
  QAction *tCollapseAllAction = new QAction(QIcon(tCollapseAllPixmap),
          "Collapse all", this);
  QAction *tExpandAllAction = new QAction(QIcon(tExpandAllPixmap),
          "Expand all", this);

  // check boxes
  _PropagateCheckAction->setCheckable(true);

  // action signal-slot
  connect(tClearAllAction,SIGNAL(triggered()),
      this,SLOT(onClearAllAction()));
  connect(_PropagateCheckAction,SIGNAL(triggered()),
      this,SLOT(onPropagateCheckAction()));
  connect(tExpandToCheckedAction,SIGNAL(triggered()),
      this,SLOT(onExpandToCheckedAction()));
  connect(tCollapseAllAction,SIGNAL(triggered()),
      this,SLOT(onCollapseAllAction()));
  connect(tExpandAllAction,SIGNAL(triggered()),
      this,SLOT(onExpandAllAction()));

  // menu
  aMenu = menuBar()->addMenu("&Tree");
  aMenu->addAction(tClearAllAction);
  aMenu->addAction(_PropagateCheckAction);
  aMenu->addAction(tExpandToCheckedAction);
  aMenu->addAction(tCollapseAllAction);
  aMenu->addAction(tExpandAllAction);

  // toolbar
  aToolBar->addAction(tClearAllAction);
  aToolBar->addAction(_PropagateCheckAction);
  aToolBar->addAction(tExpandToCheckedAction);
  aToolBar->addAction(tCollapseAllAction);
  aToolBar->addAction(tExpandAllAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupToolActions(QMenu *aMenu,QToolBar *aToolBar)
{
  // pixamps
  QPixmap tTestScopeToolPixmap(":/filter_tool.png");

  // actions
  QAction *tTestScopeToolAction =
      new QAction(QIcon(tTestScopeToolPixmap),"Filter Scope Tool...", this);

  // check boxes
  tTestScopeToolAction->setCheckable(false);

  // action signal-slot
  connect(tTestScopeToolAction,SIGNAL(triggered()),
      this,SLOT(onTestScopeToolAction()));

  // menu
  aMenu = menuBar()->addMenu("&Tools");
  aMenu->addAction(tTestScopeToolAction);

  // toolbar
  aToolBar->addAction(tTestScopeToolAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupToolBarMenu(QMenu *aMenu)
{
  // actions
  _ViewFileToolbarAction = new QAction("File ToolBar",this);
  _ViewOperateToolbarAction = new QAction("Operate ToolBar",this);
  _ViewDelimitToolbarAction = new QAction("Delimit ToolBar",this);
  _ViewFormatToolbarAction = new QAction("Format ToolBar",this);
  _ViewTreeToolbarAction = new QAction("Tree ToolBar",this);
  _ViewToolToolbarAction = new QAction("Tool ToolBar",this);

  // check boxes
  _ViewFileToolbarAction->setCheckable(true);
  _ViewOperateToolbarAction->setCheckable(true);
  _ViewDelimitToolbarAction->setCheckable(true);
  _ViewFormatToolbarAction->setCheckable(true);
  _ViewTreeToolbarAction->setCheckable(true);
  _ViewToolToolbarAction->setCheckable(true);

  _ViewFileToolbarAction->setChecked(false);
  _ViewOperateToolbarAction->setChecked(true);
  _ViewDelimitToolbarAction->setChecked(true);
  _ViewFormatToolbarAction->setChecked(true);
  _ViewTreeToolbarAction->setChecked(true);
  _ViewToolToolbarAction->setChecked(true);

  // action signal-slot
  connect(_ViewFileToolbarAction,SIGNAL(triggered()),
      this,SLOT(onViewFileToolbarAction()));
  connect(_ViewOperateToolbarAction,SIGNAL(triggered()),
      this,SLOT(onViewOperateToolbarAction()));
  connect(_ViewDelimitToolbarAction,SIGNAL(triggered()),
      this,SLOT(onViewDelimitToolbarAction()));
  connect(_ViewFormatToolbarAction,SIGNAL(triggered()),
      this,SLOT(onViewFormatToolbarAction()));
  connect(_ViewFormatToolbarAction,SIGNAL(triggered()),
      this,SLOT(onViewTreeToolbarAction()));
  connect(_ViewToolToolbarAction,SIGNAL(triggered()),
      this,SLOT(onViewToolToolbarAction()));

  // menu
  aMenu = menuBar()->addMenu("&ToolBars");
  aMenu->addAction(_ViewFileToolbarAction);
  aMenu->addAction(_ViewOperateToolbarAction);
  aMenu->addAction(_ViewDelimitToolbarAction);
  aMenu->addAction(_ViewFormatToolbarAction);
  aMenu->addAction(_ViewTreeToolbarAction);
  aMenu->addAction(_ViewToolToolbarAction);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setupMenuAndToolbar()
{
  QMenu *tMenu = new QMenu();

  _StatusLabel = new QLabel();
  _StatusLabel->setAutoFillBackground(true);

  statusBar()->addWidget(_StatusLabel);

  _FileToolBar = addToolBar("File Toolbar");
  setupFileActions(tMenu,_FileToolBar);

  _OperateToolBar = addToolBar("Operate Toolbar");
  setupOperateActions(tMenu,_OperateToolBar);

  _DelimitToolBar = addToolBar("Delimit Toolbar");
  setupDelimitActions(tMenu,_DelimitToolBar);

  _FormatToolBar = addToolBar("Format Toolbar");
  setupFormatActions(tMenu,_FormatToolBar);

  _TreeToolBar = addToolBar("Tree Toolbar");
  setupTreeActions(tMenu,_TreeToolBar);

  _ToolToolBar = addToolBar("Tool Toolbar");
  setupToolActions(tMenu,_ToolToolBar);

  setupToolBarMenu(tMenu);

  _FileToolBar->hide();
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
QWidget *MainWindow::createTestScopeToolWidget(QWidget *aParent)
{
  QWidget *tWidget = new QWidget(aParent);

  QGroupBox *tGroup = createTestScopeToolGroup(tWidget);
  tGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

  QVBoxLayout *tLayout = new QVBoxLayout;
  tLayout->addWidget(tGroup);
  tLayout->setAlignment(tGroup,Qt::AlignTop);

  tWidget->setLayout(tLayout);

  return tWidget;
}

//-------------------------------------------------------------------------------
// Note that tree view must be created before this is called. Otherwise,
// _DataStructMode will be null, and so the connections won't be made.
//-------------------------------------------------------------------------------
QGroupBox *MainWindow::createTestScopeToolGroup(QWidget *aParent)
{
  QGroupBox *tGroup = new QGroupBox("Test Scope Utility",aParent);
  tGroup->setFlat(true);

  // widgets
  _TestScopeComboBox = new QComboBox(aParent);
  std::vector<std::string> tTestScopes = _DataStructModel->getTestScopes();
  _TestScopeComboBox->addItems(convertToQStringList(tTestScopes));

  QPushButton *tPushButton = new QPushButton("Apply test scope",aParent);

  // layout
  QVBoxLayout *tLayout = new QVBoxLayout;
  tLayout->addWidget(_TestScopeComboBox);
  tLayout->addWidget(tPushButton);

  tGroup->setLayout(tLayout);

  // connections
  connect(tPushButton,SIGNAL(clicked(bool)),
      this,SLOT(onApplyTestScopeClicked(bool)));

  return tGroup;
}

//-----------------------------------------------------------------------------
// Creates the options widget.
//-----------------------------------------------------------------------------
QWidget *MainWindow::createCustomFormatToolWidget(QWidget *aParent)
{
  QWidget *tWidget = new QWidget(aParent);

  QGroupBox *tGroup = createCustomFormatToolGroup(tWidget);
  tGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

  QHBoxLayout *tLayout = new QHBoxLayout;
  tLayout->addWidget(tGroup);
  tLayout->setAlignment(tGroup,Qt::AlignTop);

  tWidget->setLayout(tLayout);

  return tWidget;
}

//-------------------------------------------------------------------------------
// Note that tree view must be created before this is called. Otherwise,
// _DataStructMode will be null, and so the connections won't be made.
//-------------------------------------------------------------------------------
QGroupBox *MainWindow::createCustomFormatToolGroup(QWidget *aParent)
{
  QGroupBox *tGroup = new QGroupBox("Custom Format Utility",aParent);
  tGroup->setFlat(true);

  // widgets
  QPushButton *tAsIsButton =
      new QPushButton("Apply \"As-is\" settings",tGroup);
  QPushButton *tLongnameButton =
      new QPushButton("Apply \"Longname\" settings",tGroup);
  QPushButton *tTableButton =
      new QPushButton("Apply \"Table\" settings",tGroup);

  _AsIsCheckBox = new QCheckBox("Selected fields only");
  _LongnameCheckBox = new QCheckBox("Selected fields only");
  _TableCheckBox = new QCheckBox("Selected fields only");

  // layout
  QGridLayout *tLayout = new QGridLayout(aParent);
  tLayout->addWidget(tAsIsButton,0,0);
  tLayout->addWidget(tLongnameButton,1,0);
  tLayout->addWidget(tTableButton,2,0);
  tLayout->addWidget(_AsIsCheckBox,0,1);
  tLayout->addWidget(_LongnameCheckBox,1,1);
  tLayout->addWidget(_TableCheckBox,2,1);

  tGroup->setLayout(tLayout);

  // connections
  connect(tAsIsButton,SIGNAL(clicked(bool)),
      this,SLOT(onAsIsPushbuttonClicked(bool)));
  connect(tLongnameButton,SIGNAL(clicked(bool)),
      this,SLOT(onLongnamePushbuttonClicked(bool)));
  connect(tTableButton,SIGNAL(clicked(bool)),
      this,SLOT(onTablePushbuttonClicked(bool)));

  return tGroup;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onAsIsPushbuttonClicked(bool)
{
  QModelIndexList tSelectedRows = _StructTree->selectionModel()->selectedRows();
  int tAsIsInt = (int)RecordProcessor::eAsIs;
  bool tChecked = (_AsIsCheckBox->isChecked() ? true: false);
  emit applyFormatMode(tAsIsInt,tChecked,tSelectedRows);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onLongnamePushbuttonClicked(bool)
{
  QModelIndexList tSelectedRows = _StructTree->selectionModel()->selectedRows();
  int tLongnameInt = (int)RecordProcessor::eLongname;
  bool tChecked = (_LongnameCheckBox->isChecked() ? true: false);
  emit applyFormatMode(tLongnameInt,tChecked,tSelectedRows);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onTablePushbuttonClicked(bool)
{
  QModelIndexList tSelectedRows = _StructTree->selectionModel()->selectedRows();
  int tTableInt = (int)RecordProcessor::eTable;
  bool tChecked = (_TableCheckBox->isChecked() ? true: false);
  emit applyFormatMode(tTableInt,tChecked,tSelectedRows);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onApplyTestScopeClicked(bool)
{
  QString tTestScope = _TestScopeComboBox->currentText();
  emit applyTestScope(tTestScope,false);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onCustomFormatToolAction()
{
    _CustomFormatToolWidget = createCustomFormatToolWidget(0);

   _CustomFormatToolDock = new QDockWidget(0);
   _CustomFormatToolDock->setObjectName("customFormatToolDock");
   _CustomFormatToolDock->setWidget(_CustomFormatToolWidget);
   _CustomFormatToolDock->setAllowedAreas(
       Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

  addDockWidget(Qt::TopDockWidgetArea, _CustomFormatToolDock);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onClearAllAction()
{
  _DataStructModel->resetTreeItems();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onPropagateCheckAction()
{
  if (_PropagateCheckAction->isChecked())
  {
    emit togglePropagateChecks(true);
  }
  else
  {
    emit togglePropagateChecks(false);
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onExpandToCheckedAction()
{
  _StructTree->expandToChecked(false);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onCollapseAllAction()
{
  _StructTree->collapseAll();
  _StructTree->expandToDepth(0);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onExpandAllAction()
{
  _StructTree->expandAll();
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onTestScopeToolAction()
{
   _TestScopeToolWidget = createTestScopeToolWidget(0);

   _TestScopeToolDock = new QDockWidget(0);
   _TestScopeToolDock->setObjectName("testScopeToolDock");
   _TestScopeToolDock->setWidget(_TestScopeToolWidget);
   _TestScopeToolDock->setAllowedAreas(
       Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

  addDockWidget(Qt::TopDockWidgetArea, _TestScopeToolDock);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onViewFileToolbarAction()
{
  if (_ViewFileToolbarAction->isChecked())
  {
    _FileToolBar->show();
  }
  else
  {
    _FileToolBar->hide();
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onViewOperateToolbarAction()
{
  if (_ViewOperateToolbarAction->isChecked())
  {
    _OperateToolBar->show();
  }
  else
  {
    _OperateToolBar->hide();
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onViewDelimitToolbarAction()
{
  if (_ViewDelimitToolbarAction->isChecked())
  {
    _DelimitToolBar->show();
  }
  else
  {
    _DelimitToolBar->hide();
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onViewFormatToolbarAction()
{
  if (_ViewFormatToolbarAction->isChecked())
  {
    _FormatToolBar->show();
  }
  else
  {
    _FormatToolBar->hide();
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onViewTreeToolbarAction()
{
  if (_ViewTreeToolbarAction->isChecked())
  {
    _TreeToolBar->show();
  }
  else
  {
    _TreeToolBar->hide();
  }
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onViewToolToolbarAction()
{
  if (_ViewToolToolbarAction->isChecked())
  {
    _ToolToolBar->show();
  }
  else
  {
    _ToolToolBar->hide();
  }
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
void MainWindow::onStructNameAvailable(QString aMsgId,QString aStructName)
{
  _MessageId = aMsgId;

  /*
   * Update the window title and permanent widget.
   */
  updateWindowTitle(aMsgId,aStructName);
  updatePermanentWidget(aMsgId,aStructName);

  /*
   * Get the message spec for this message ID, if it exists.
   */
  MessageSpec *aMessageSpec = NULL;

  MessageSpecMap::iterator tIter = _MessageSpecMap.find(aMsgId);
  if (tIter != _MessageSpecMap.end())
  {
    aMessageSpec = new MessageSpec(tIter->second);
  }
  else
  {
    std::cout << "No message map for messageId "
              << qPrintable(aMsgId) << std::endl;
  }

  /*
   * Verify aStructName against the message map.
   */
  if (aMessageSpec && aMessageSpec->getStructName() != aStructName )
  {
    std::cout << "WARNING: unexpected message id/structure combination"
        << std::endl;
  }

  /*
   * Get the header file for the message ID.
   */
  std::string tHeaderPath = getHeaderPath(aMessageSpec);

  /*
   * Parse the header file to populate _StructorBuilder.
   */
  std::cout << "Parsing header file " << tHeaderPath
            << "..." << std::endl;

  _StructorBuilder = HeaderUtil::parseHeaderFile(tHeaderPath.c_str());
  if (_StructorBuilder == NULL)
  {
    std::cout << "ERROR: Failed to build structor builder for msgId "
              << qPrintable(aMsgId) << std::endl;
    exit(1);
  }

  /*
   * Need to set the structure name before setting up the view and launching
   * the window.
   */
  setInitialStructName(aStructName.toStdString());
  setupView(_StructName);

  /*
   * Apply default filter settings if found.
   */
  if (aMessageSpec)
  {
    QString tFilter = aMessageSpec->getFilter();
    if (tFilter.length() > 0)
    {
      QString tDir = _AppConfig.getDefaultFiltersDir();
      FilterSpec tFiter = _FilterReader->openFilter(tDir,tFilter);
      tFiter.apply(_DataStructModel,
          _OperateActionGroup,_DelimitActionGroup,_FormatActionGroup);
    _StructTree->expandToChecked(true);
    }
  }

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
  setupDataStructModel(tString.toStdString());
  _StructTree->setDataStructModel(_DataStructModel);
}

//-------------------------------------------------------------------------------
// Set the main window title.
//-------------------------------------------------------------------------------
void MainWindow::updateWindowTitle()
{
  QString tTitle(kAppName);
  setWindowTitle(tTitle);
}

//-------------------------------------------------------------------------------
// Set the main window title.
//-------------------------------------------------------------------------------
void MainWindow::updateWindowTitle(QString aMsgId,QString aStructName)
{
  QString tTitle(
      kAppName + "   [" + aMsgId + " : " + aStructName + "]");
  setWindowTitle(tTitle);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::updatePermanentWidget(QString aMsgId,QString aStructName)
{
  statusBar()->addPermanentWidget(
      new QLabel("<i>" + aMsgId + "</i> : " + aStructName));
}

//-------------------------------------------------------------------------------
// Get the full path to the header file corresponding to a message ID.
// If a header file is specified in the message spec, then that header will be
// used. Otherwise, the default header will be used.
// Note: This method assumes that the app config specifies the default header
// directory, and either a default header, or a message spec that specifies the
// header for the message ID. If not, an empty string will be returned.
//-------------------------------------------------------------------------------
std::string MainWindow::getHeaderPath(MessageSpec *aMessageSpec)
{
  QString tHeaderPath;

  /*
   * Get header filename.
   */
  QString tHeaderFilename;
  if (aMessageSpec)
  {
    tHeaderFilename = aMessageSpec->getHeader();
  }
  else
  {
    tHeaderFilename = _AppConfig.getDefaultHeader();
  }

  /*
   * If a header filename was found, get the headers directory, and construct
   * the full pathname to the header file.
   */
  if (tHeaderFilename.length() != 0)
  {
    QString tHeadersDir = _AppConfig.getHeadersDir();
    if (tHeadersDir.length() != 0)
    {
      tHeaderPath = tHeadersDir + "/" + tHeaderFilename;
    }
    else
    {
      std::cout << "ERROR: no headers directory specified" << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: no header file specified" << std::endl;
  }

  return tHeaderPath.toStdString();
}
