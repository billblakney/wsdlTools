#include <cstdio>
#include <iostream>
#include <vector>
#include <QAbstractItemModel>
#include <QButtonGroup>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "ComboBoxDelegate.hh"
#include "TestRegexDelegate.hh"
#include "MainWindow.hh"

using namespace std;

extern StructorBuilder *lex_main(char *aHeaderFile);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    int argc, char *argv[],QApplication &aApp,QWidget *aParent,
    bool aIsFilterMode,StreamReader *aStreamReader,
    RecordProcessor *aRecordProcessor)
  : QWidget(aParent),
    _IsFilterMode(aIsFilterMode),
    _StreamReader(aStreamReader),
    _RecordProcessor(aRecordProcessor),
    _StructorBuilder(0),
    _DataStructModel(0),
    _StructComboBox(0),
    _StructTree(0),
    _HeaderFileWasParsed(false),
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
   * Read the environment variables. Currently only one: CLIRCAR_H.
   * It may optionally be set by command-line instead.
   */
  readEnvironmentVariables();

  /*
   * Process command line arguments.
   */
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
    std::cout << "parsing header file " << _HeaderFile << std::endl;
    _StructorBuilder = lex_main((char *) _HeaderFile.c_str());
    //   _StructorBuilder->printSummary();
    //   _StructorBuilder->postProcess();
    _HeaderFileWasParsed = true;
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
//-----------------------------------------------------------------------------
void MainWindow::setupView()
{
  /*
   * Parse the header file.
   */
  parseHeaderFile(); // ok if already called before

  if (!_IsFilterMode && _InitialStruct.length() == 0)
  {
    _InitialStruct = _StructorBuilder->getStructNames().at(0);
  }

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
  _StructTree->header()->resizeSection(DataStructModel::eColFieldName,225);
  _StructTree->header()->resizeSection(DataStructModel::eColTestRegex,225);
  _StructTree->header()->resizeSection(DataStructModel::eColTestScope,175);
  _StructTree->header()->resizeSection(DataStructModel::eColFormat,100);
  _StructTree->header()->resizeSection(DataStructModel::eColPostfix,75);

  _StructTree->setEditTriggers(QAbstractItemView::AllEditTriggers);

  TestRegexDelegate *tTestRegexDelegate =
      new TestRegexDelegate (_DataStructModel,this);
  _StructTree->setItemDelegateForColumn(
      DataStructModel::eColTestRegex,tTestRegexDelegate);

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

#if USING_BOTTOM_PUSHBUTTON // no longer using, may use later
  /*
   * Pushbutton for setting filter.
   */
  QPushButton *tButton = new QPushButton("Set Filter", this);

  connect(tButton, SIGNAL(clicked(bool)), this, SLOT(onSetFilterClicked(bool)));
#endif

  /*
   * The options will only be populated when in filter mode.
   */
  QWidget *tOptions = new QWidget(this);

  if (_IsFilterMode)
  {

    /*
     * Create widget to hold miscellaneous options.
     */
//    QWidget *tMainOptions = new QWidget(tOptions);

    QGroupBox *tFormatModeWidget = createFormatModeGroup(tOptions);
    tFormatModeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QGroupBox *tCustomFormatGroup = createCustomFormatGroup(tOptions);
    tCustomFormatGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QGroupBox *tOutputModeGroup = createOutputModeGroup(tOptions);
    tOutputModeGroup->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

//    QVBoxLayout *tMainOptionsLayout = new QVBoxLayout;
//    tMainOptionsLayout->addWidget(tOutputModeGroup);
//    tMainOptions->setLayout(tMainOptionsLayout);


    QHBoxLayout *tOptionsLayout = new QHBoxLayout;
    tOptionsLayout->addWidget(tFormatModeWidget);
    tOptionsLayout->addWidget(tCustomFormatGroup);
    tOptionsLayout->addWidget(tOutputModeGroup);
    tOptionsLayout->setAlignment(tFormatModeWidget,Qt::AlignTop);
    tOptionsLayout->setAlignment(tCustomFormatGroup,Qt::AlignTop);
    tOptionsLayout->setAlignment(tOutputModeGroup,Qt::AlignTop);

    tOptions->setLayout(tOptionsLayout);
  }

  /*
   * Create bypass checkbox and connect it to the stream reader.
   */
  QCheckBox *tPropagateCheckBox = new QCheckBox("Propagate check on field name node to children",this);

  connect(tPropagateCheckBox,SIGNAL(toggled(bool)),
      _DataStructModel,SLOT(onPropogateToggled(bool)));

  /*
   * Put widgets in the dialog using box layout.
   */
  QVBoxLayout *tWindowLayout = new QVBoxLayout;
  tWindowLayout->addWidget(_StructComboBox);
  if (tOptions)
  {
    tWindowLayout->addWidget(tOptions);
    tWindowLayout->addWidget(tPropagateCheckBox);
  }
  tWindowLayout->addWidget(_StructTree);
#if USING_BOTTOM_PUSHBUTTON // no longer using, may use later
  tWindowLayout->addWidget(tButton);
#endif

  setLayout(tWindowLayout);

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

    _FormatAsIsButton->setChecked(true);

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

    /*
     * Create delimit records checkbox and connect it to the stream reader.
     */
    QCheckBox *tDelimitRecordsCheckBox =
        new QCheckBox("Delimit records",tGroup);
    tDelimitRecordsCheckBox->setCheckState(Qt::Checked);

    connect(tDelimitRecordsCheckBox,SIGNAL(toggled(bool)),
        _StreamReader,SLOT(onDelimitRecordsToggle(bool)));

    QVBoxLayout *tGroupLayout = new QVBoxLayout;
    tGroupLayout->addWidget(_FormatAsIsButton);
    tGroupLayout->addWidget(_FormatLongnameButton);
    tGroupLayout->addWidget(_FormatTableButton);
    tGroupLayout->addWidget(_FormatCustomButton);
    tGroupLayout->addWidget(tDelimitRecordsCheckBox);

    tGroup->setLayout(tGroupLayout);
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
//-------------------------------------------------------------------------------
void MainWindow::onStructNameAvailable(QString aStructName)
{
  /*
   * Need to set the structure name before setting up the view and launching
   * the window.
   */
  setInitialStructName(aStructName.toStdString());
  setupView();

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
