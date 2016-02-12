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

using namespace std;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    int argc, char *argv[],QApplication &aApp,QWidget *aParent,
    StructorBuilder *aStructorBuilder,bool aIsFilterMode,
    StreamReader *aStreamReader,RecordProcessor *aRecordProcessor)
/*  : QWidget(aParent),TODO*/
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

  /*
   * Process command line arguments.
   */
  processCommandLine(argc,argv);
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

/*------------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
void MainWindow::processCommandLine(int argc,char *argv[])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);
#if 0
  for (int tIdx = 0; tIdx < argc; tIdx++)
  {
  }
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onDelimitOptionSelection(bool aIsChecked)
{
  if (!aIsChecked) // only need to process the toggle on, not the one off
    return;

  if (_DelimitAllButton->isChecked())
  {
    emit delimitOptionSelected((int)StreamReader::eAllRecords);
  }
  else if (_DelimitOutputButton->isChecked())
  {
    emit delimitOptionSelected((int)StreamReader::eOutputRecords);
  }
  else if (_DelimitNoneButton->isChecked())
  {
    emit delimitOptionSelected((int)StreamReader::eNoRecords);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onFormatOptionSelection(bool aIsChecked)
{
  if (!aIsChecked) // only need to process the toggle on, not the one off
    return;

  if (_FormatAsIsButton->isChecked())
  {
    emit formatOptionSelected((int)RecordProcessor::eAsIs);
  }
  else if (_FormatLongnameButton->isChecked())
  {
    emit formatOptionSelected((int)RecordProcessor::eLongname);
  }
  else if (_FormatTableButton->isChecked())
  {
    emit formatOptionSelected((int)RecordProcessor::eTable);
  }
  else if (_FormatCustomButton->isChecked())
  {
    emit formatOptionSelected((int)RecordProcessor::eCustom);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onBypass()
{
    setOutputMode(StreamReader::eBypass);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onGo()
{
    setOutputMode(StreamReader::eNormal);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onStop()
{
    setOutputMode(StreamReader::eFreezeDrop);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onDelimitOut()
{
  setDelimitMode(StreamReader::eOutputRecords);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onDelimitAll()
{
  setDelimitMode(StreamReader::eAllRecords);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onDelimitNone()
{
  setDelimitMode(StreamReader::eNoRecords);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onAsIsFormat()
{
  setFormatMode(RecordProcessor::eAsIs);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onLongnameFormat()
{
  setFormatMode(RecordProcessor::eLongname);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onTableFormat()
{
  setFormatMode(RecordProcessor::eTable);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onCustomFormat()
{
  setFormatMode(RecordProcessor::eCustom);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setDelimitMode(StreamReader::DelimitMode aMode)
{
  emit delimitOptionSelected((int)aMode);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setOutputMode(StreamReader::OutputMode aMode)
{
  emit outputModeSelected((int)aMode);

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
void MainWindow::setFormatMode(RecordProcessor::FormatMode aMode)
{
  emit formatOptionSelected((int)aMode);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::setStatusLabel(QString aStatus,QPalette aPalette)
{
  _StatusLabel->setPalette(aPalette);
  _StatusLabel->setText("Mode: " + aStatus);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MainWindow::onOutputModeButtonClicked(QAbstractButton *aButton)
{
  if (aButton == _OutputNormalButton)
  {
    std::cout << "Normal clicked" << std::endl;
    emit outputModeSelected((int)StreamReader::eNormal);
  }
  else if (aButton == _OutputBypassButton)
  {
    std::cout << "Bypass clicked" << std::endl;
    emit outputModeSelected((int)StreamReader::eBypass);
  }
  else if (aButton == _OutputFreezeDropButton)
  {
    std::cout << "Freeze clicked" << std::endl;
    emit outputModeSelected((int)StreamReader::eFreezeDrop);
  }
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
  setupMenuAndToolbar();

   _CentralWidget = new QWidget(this);
   setCentralWidget(_CentralWidget);

   QDockWidget *shapesDockWidget = new QDockWidget(QString("Shapes"));
   shapesDockWidget->setObjectName("shapesDockWidget");
   shapesDockWidget->setWidget(new QLabel("dock area"));
   shapesDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
   addDockWidget(Qt::TopDockWidgetArea, shapesDockWidget);

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

  QTabWidget *tTabWidget = NULL;
  /*
   * The options will only be populated when in filter mode.
   */
  if (_IsFilterMode)
  {
    tTabWidget = new QTabWidget(0);
    tTabWidget->setSizePolicy(
        QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);
    _ConfigureWidget = createConfigureWidget(0);
//    _ConfigureWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    _OperateWidget = createOperateWidget(0);
    tTabWidget->addTab(_ConfigureWidget,QString("Configure"));
    tTabWidget->addTab(_OperateWidget,QString("Operate"));

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
//TODO rm
//  if (_OptionsWidget)
//  {
//    tWindowLayout->addWidget(_OptionsWidget);
//    tWindowLayout->addWidget(_PropagateCheckBox);
//  }
  if (tTabWidget)
  {
    tWindowLayout->addWidget(tTabWidget);
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
void MainWindow::setupMenuAndToolbar()
{
   QPixmap tGreenLight("green.png");
   QPixmap tRedLight("red.png");
   QPixmap tBypass("bypass3.png");

   QPixmap tDelimAll("delim_all.png");
   QPixmap tDelimOut("delim_out.png");
   QPixmap tDelimNone("delim_none.png");
//   QPixmap quitpix("quit.png");

#if FILE_MENU
   /*
    * Quit action
    */
   QAction *quit = new QAction("&Quit", this);

   connect(quit, SIGNAL(triggered()), qApp, SLOT(quit()));
#endif

   /*
    * Create go/stop/bypass actions.
    */
   QAction *tGoAction = new QAction(QIcon(tGreenLight),"&Go", this);
   QAction *tStopAction = new QAction(QIcon(tRedLight),"&Stop", this);
   QAction *tBypassAction = new QAction(QIcon(tBypass),"&Bypass", this);

   tGoAction->setShortcut(QKeySequence(Qt::Key_G));
   tStopAction->setShortcut(QKeySequence(Qt::Key_S));
   tBypassAction->setShortcut(QKeySequence(Qt::Key_B));

   tGoAction->setCheckable(true);
   tStopAction->setCheckable(true);
   tBypassAction->setCheckable(true);

   tGoAction->setChecked(true);

   QActionGroup *tRunGroup = new QActionGroup(this);
   tRunGroup->addAction(tGoAction);
   tRunGroup->addAction(tStopAction);
   tRunGroup->addAction(tBypassAction);

   connect(tGoAction, SIGNAL(triggered()), this, SLOT(onGo()));
   connect(tStopAction, SIGNAL(triggered()), this, SLOT(onStop()));
   connect(tBypassAction, SIGNAL(triggered()), this, SLOT(onBypass()));

   /*
    * Create format mode actions.
    */
   QAction *tAsIsFormatAction = new QAction(QIcon(tGreenLight),"&As-is", this);
   QAction *tLongnameFormatAction = new QAction(QIcon(tRedLight),"&Long name", this);
   QAction *tTableFormatAction = new QAction(QIcon(tBypass),"&Table", this);
   QAction *tCustomFormatAction = new QAction(QIcon(tBypass),"&Custom", this);

   tAsIsFormatAction->setShortcut(QKeySequence(Qt::Key_A));
   tLongnameFormatAction->setShortcut(QKeySequence(Qt::Key_L));
   tTableFormatAction->setShortcut(QKeySequence(Qt::Key_T));
   tCustomFormatAction->setShortcut(QKeySequence(Qt::Key_C));

   tAsIsFormatAction->setCheckable(true);
   tLongnameFormatAction->setCheckable(true);
   tTableFormatAction->setCheckable(true);
   tCustomFormatAction->setCheckable(true);

   tAsIsFormatAction->setChecked(true);

   QActionGroup *tFormatGroup = new QActionGroup(this);
   tFormatGroup->addAction(tAsIsFormatAction);
   tFormatGroup->addAction(tLongnameFormatAction);
   tFormatGroup->addAction(tTableFormatAction);
   tFormatGroup->addAction(tCustomFormatAction);

   connect(tAsIsFormatAction, SIGNAL(triggered()), this, SLOT(onAsIsFormat()));
   connect(tLongnameFormatAction, SIGNAL(triggered()), this, SLOT(onLongnameFormat()));
   connect(tTableFormatAction, SIGNAL(triggered()), this, SLOT(onTableFormat()));
   connect(tCustomFormatAction, SIGNAL(triggered()), this, SLOT(onCustomFormat()));

   /*
    * Create record delimiter actions.
    */
   QAction *tDelimitOutputAction = new QAction(QIcon(tDelimOut),"&Delimit Output Records", this);
   QAction *tDelimitNoneAction = new QAction(QIcon(tDelimNone),"&No Record Delimiters", this);
   QAction *tDelimitAllAction = new QAction(QIcon(tDelimAll),"&Delimit All Records", this);

   tDelimitAllAction->setCheckable(true);
   tDelimitOutputAction->setCheckable(true);
   tDelimitNoneAction->setCheckable(true);

   tDelimitOutputAction->setChecked(true);

   QActionGroup *tDelimitGroup = new QActionGroup(this);
   tDelimitGroup->addAction(tDelimitOutputAction);
   tDelimitGroup->addAction(tDelimitNoneAction);
   tDelimitGroup->addAction(tDelimitAllAction);

   connect(tDelimitOutputAction, SIGNAL(triggered()), this, SLOT(onDelimitOut()));
   connect(tDelimitAllAction, SIGNAL(triggered()), this, SLOT(onDelimitAll()));
   connect(tDelimitNoneAction, SIGNAL(triggered()), this, SLOT(onDelimitNone()));

   /*
    * Create menu.
    */
   QMenu *tMenu;

#if FILE_MENU
   tMenu = menuBar()->addMenu("&File");
   tMenu->addAction(quit);
#endif

   tMenu = menuBar()->addMenu("&Operate");
   tMenu->addAction(tGoAction);
   tMenu->addAction(tStopAction);
   tMenu->addAction(tBypassAction);

   tMenu = menuBar()->addMenu("&Delimit Records");
   tMenu->addAction(tDelimitOutputAction);
   tMenu->addAction(tDelimitNoneAction);
   tMenu->addAction(tDelimitAllAction);

   tMenu = menuBar()->addMenu("&Format Mode");
   tMenu->addAction(tAsIsFormatAction);
   tMenu->addAction(tLongnameFormatAction);
   tMenu->addAction(tTableFormatAction);
   tMenu->addAction(tCustomFormatAction);

   /*
    * Create toolbar.
    */
   QToolBar *toolbar = addToolBar("Main Toolbar");
   toolbar->addAction(tGoAction);
   toolbar->addAction(tStopAction);
   toolbar->addAction(tBypassAction);
   toolbar->addSeparator();
   toolbar->addAction(tDelimitOutputAction);
   toolbar->addAction(tDelimitNoneAction);
   toolbar->addAction(tDelimitAllAction);
   toolbar->addSeparator();
   toolbar->addAction(tAsIsFormatAction);
   toolbar->addAction(tLongnameFormatAction);
   toolbar->addAction(tTableFormatAction);
   toolbar->addAction(tCustomFormatAction);


#if 0
   toolbar->addSeparator();

   QAction *quit2 = toolbar->addAction(QIcon(quitpix),
       "Quit Application");
   connect(quit2, SIGNAL(triggered()), qApp, SLOT(quit()));
#endif

   _StatusLabel = new QLabel();
   _StatusLabel->setAutoFillBackground(true);

   setOutputMode(StreamReader::eNormal);

   statusBar()->addWidget(_StatusLabel);
   statusBar()->addPermanentWidget(new QLabel("-counts-"));
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

  connect(tComboBox, SIGNAL(activated(int)), aParent,
      SLOT(onStructComboBoxActivated(int)));

  return tComboBox;
}

//-----------------------------------------------------------------------------
// Creates the options widget.
//-----------------------------------------------------------------------------
QWidget *MainWindow::createConfigureWidget(QWidget *aParent)
{
    QWidget *tConfigure = new QWidget(aParent);

    QGroupBox *tDelimitModeWidget = createDelimitModeGroup(tConfigure);
    tDelimitModeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QGroupBox *tFormatModeWidget = createFormatModeGroup(tConfigure);
    tFormatModeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QGroupBox *tCustomFormatGroup = createCustomFormatGroup(tConfigure);
    tCustomFormatGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

//    QVBoxLayout *tMainOptionsLayout = new QVBoxLayout;
//    tMainOptionsLayout->addWidget(tOutputModeGroup);
//    tMainOptions->setLayout(tMainOptionsLayout);


    QHBoxLayout *tLayout = new QHBoxLayout;
    tLayout->addWidget(tDelimitModeWidget);
    tLayout->addWidget(tFormatModeWidget);
    tLayout->addWidget(tCustomFormatGroup);
    tLayout->setAlignment(tDelimitModeWidget,Qt::AlignTop);
    tLayout->setAlignment(tFormatModeWidget,Qt::AlignTop);
    tLayout->setAlignment(tCustomFormatGroup,Qt::AlignTop);

    tConfigure->setLayout(tLayout);

    return tConfigure;
}

//-----------------------------------------------------------------------------
// Creates the options widget.
//-----------------------------------------------------------------------------
QWidget *MainWindow::createOperateWidget(QWidget *aParent)
{
    QWidget *tOperate = new QWidget(aParent);

    QGroupBox *tOutputModeGroup = createOutputModeGroup(tOperate);
    tOutputModeGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

//    QVBoxLayout *tMainOptionsLayout = new QVBoxLayout;
//    tMainOptionsLayout->addWidget(tOutputModeGroup);
//    tMainOptions->setLayout(tMainOptionsLayout);


    QHBoxLayout *tLayout = new QHBoxLayout;
    tLayout->addWidget(tOutputModeGroup);
    tLayout->setAlignment(tOutputModeGroup,Qt::AlignTop);

    tOperate->setLayout(tLayout);

    return tOperate;
}

//-----------------------------------------------------------------------------
// Creates the delimit mode widget.
//-----------------------------------------------------------------------------
QGroupBox *MainWindow::createDelimitModeGroup(QWidget *aParent)
{
    QGroupBox *tGroup = new QGroupBox("Delimit Records Mode",aParent);
    tGroup->setFlat(true);

    _DelimitAllButton =
        new QRadioButton("Always",tGroup);
    _DelimitOutputButton =
        new QRadioButton("On output",tGroup);
    _DelimitNoneButton =
        new QRadioButton("Never",tGroup);

   _DelimitOutputButton->setChecked(true);

    connect(_DelimitAllButton,SIGNAL(toggled(bool)),
        this,SLOT(onDelimitOptionSelection(bool)));
    connect(_DelimitOutputButton,SIGNAL(toggled(bool)),
        this,SLOT(onDelimitOptionSelection(bool)));
    connect(_DelimitNoneButton,SIGNAL(toggled(bool)),
        this,SLOT(onDelimitOptionSelection(bool)));

    connect(this,SIGNAL(delimitOptionSelected(int)),
        _StreamReader,SLOT(setDelimitMode(int)));

#if 0 //TODO rm
    /*
     * Create delimit records checkbox and connect it to the stream reader.
     */
    QCheckBox *tDelimitRecordsCheckBox =
        new QCheckBox("Delimit records",tGroup);
    tDelimitRecordsCheckBox->setCheckState(Qt::Checked);

    connect(tDelimitRecordsCheckBox,SIGNAL(toggled(bool)),
        _StreamReader,SLOT(onDelimitRecordsToggle(bool)));
#endif

    QVBoxLayout *tLayout = new QVBoxLayout;
    tLayout->addWidget(_DelimitAllButton);
    tLayout->addWidget(_DelimitOutputButton);
    tLayout->addWidget(_DelimitNoneButton);

    tGroup->setLayout(tLayout);
    return tGroup;
}

//-----------------------------------------------------------------------------
// Creates the format mode widget.
//-----------------------------------------------------------------------------
QGroupBox *MainWindow::createFormatModeGroup(QWidget *aParent)
{
    QGroupBox *tGroup = new QGroupBox("Format Mode",aParent);
    tGroup->setFlat(true);

    _FormatAsIsButton =
        new QRadioButton("As-is Formatting",tGroup);
    _FormatLongnameButton =
        new QRadioButton("Longname Formatting",tGroup);
    _FormatTableButton =
        new QRadioButton("Table Formatting",tGroup);
    _FormatCustomButton =
        new QRadioButton("Custom Formatting",tGroup);

    _FormatCustomButton->setChecked(true);

    connect(_FormatAsIsButton,SIGNAL(toggled(bool)),
        this,SLOT(onFormatOptionSelection(bool)));
    connect(_FormatLongnameButton,SIGNAL(toggled(bool)),
        this,SLOT(onFormatOptionSelection(bool)));
    connect(_FormatTableButton,SIGNAL(toggled(bool)),
        this,SLOT(onFormatOptionSelection(bool)));
    connect(_FormatCustomButton,SIGNAL(toggled(bool)),
        this,SLOT(onFormatOptionSelection(bool)));

    connect(this,SIGNAL(formatOptionSelected(int)),
        _RecordProcessor,SLOT(setFormatMode(int)));

#if 0 //TODO rm
    /*
     * Create delimit records checkbox and connect it to the stream reader.
     */
    QCheckBox *tDelimitRecordsCheckBox =
        new QCheckBox("Delimit records",tGroup);
    tDelimitRecordsCheckBox->setCheckState(Qt::Checked);

    connect(tDelimitRecordsCheckBox,SIGNAL(toggled(bool)),
        _StreamReader,SLOT(onDelimitRecordsToggle(bool)));
#endif

    QVBoxLayout *tLayout = new QVBoxLayout;
    tLayout->addWidget(_FormatAsIsButton);
    tLayout->addWidget(_FormatLongnameButton);
    tLayout->addWidget(_FormatTableButton);
    tLayout->addWidget(_FormatCustomButton);

    tGroup->setLayout(tLayout);
    return tGroup;
}

//-----------------------------------------------------------------------------
// Creates the output mode widget.
QGroupBox *MainWindow::createOutputModeGroup(QWidget *aParent)
{
    QGroupBox *tGroup = new QGroupBox("Output Mode",aParent);
    tGroup->setFlat(true);

    _OutputNormalButton =
        new QRadioButton("Normal",tGroup);
    _OutputFreezeDropButton =
        new QRadioButton("Freeze - drop incoming",tGroup);
    _OutputFreezeQueueButton =
        new QRadioButton("Freeze - queue incoming",tGroup);
    _OutputBypassButton =
        new QRadioButton("Bypass",tGroup);

    _OutputFreezeQueueButton->setEnabled(false);

    _OutputNormalButton->setChecked(true);

    _OutputModeButtonGroup = new QButtonGroup(this);

    _OutputModeButtonGroup->addButton(_OutputNormalButton);
    _OutputModeButtonGroup->addButton(_OutputFreezeDropButton);
    _OutputModeButtonGroup->addButton(_OutputFreezeQueueButton);
    _OutputModeButtonGroup->addButton(_OutputBypassButton);

    connect(_OutputModeButtonGroup,SIGNAL(buttonClicked(QAbstractButton*)),
        this,SLOT(onOutputModeButtonClicked(QAbstractButton*)));

    connect(this,SIGNAL(outputModeSelected(int)),
        _StreamReader,SLOT(onOutputModeSelected(int)));

#define USE_FREEZE_QUEUE
#ifdef USE_FREEZE_QUEUE
    QWidget *tFreezeQueuePanel = new QWidget(this);
    tFreezeQueuePanel->setContentsMargins(0,0,0,0);

    QPushButton *tNext = new QPushButton("Next",tFreezeQueuePanel);
    tNext->setMaximumSize(30,20);
    tNext->setEnabled(false);

    QLabel *tQueueCount = new QLabel("Queued: ",tFreezeQueuePanel);
    tQueueCount->setEnabled(false);

    QHBoxLayout *tFreezeQueueLayout = new QHBoxLayout;
    tFreezeQueueLayout->addWidget(tNext);
    tFreezeQueueLayout->addWidget(tQueueCount);
    tFreezeQueuePanel->setLayout(tFreezeQueueLayout);
#endif

    QVBoxLayout *tGroupLayout = new QVBoxLayout;
    tGroupLayout->addWidget(_OutputNormalButton);
    tGroupLayout->addWidget(_OutputBypassButton);
    tGroupLayout->addWidget(_OutputFreezeDropButton);
    tGroupLayout->addWidget(_OutputFreezeQueueButton);
#ifdef USE_FREEZE_QUEUE
    tGroupLayout->addWidget(tFreezeQueuePanel);
#endif

    tGroup->setLayout(tGroupLayout);
    return tGroup;
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

#if 0
    QVBoxLayout *tGroupLayout = new QVBoxLayout;
    tGroupLayout->addWidget(tAsIsButton);
    tGroupLayout->addWidget(tLongnameButton);
    tGroupLayout->addWidget(tTableButton);

    tGroup->setLayout(tGroupLayout);
#endif

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

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void MainWindow::onSetFilterClicked(bool)
{
  _DataStructModel->printTestNodes();
}
