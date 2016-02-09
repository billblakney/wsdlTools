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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MainWindow::MainWindow(
    int argc, char *argv[],QApplication &aApp,QWidget *aParent,
    StructorBuilder *aStructorBuilder,bool aIsFilterMode,
    StreamReader *aStreamReader,RecordProcessor *aRecordProcessor)
  : QWidget(aParent),
    _StructorBuilder(aStructorBuilder),
    _IsFilterMode(aIsFilterMode),
    _StreamReader(aStreamReader),
    _RecordProcessor(aRecordProcessor),
    _DataStructModel(0),
    _StructComboBox(0),
    _OptionsWidget(0),
    _StructTree(0),
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
// Set up the main GUI window.
// This method ...
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

  /*
   * Create structure dropdown list.
   */
  _StructComboBox = createStructComboBox(this);

  /*
   * Create structure tree view.
   * Need to create the tree view here since it creates _DataStructModel, and
   * other methods used below rely on _DataStructModel being set before they
   * are called.
   */
  _StructTree = createTreeView(this);
//  _StructTree = new StructTreeView(this);
//  setTreeViewStruct(_StructName);
//  _StructTree->header()->resizeSection(DataStructModel::eColFieldName,225);
//  _StructTree->header()->resizeSection(DataStructModel::eColTestRegex,225);
//  _StructTree->header()->resizeSection(DataStructModel::eColTestScope,175);
//  _StructTree->header()->resizeSection(DataStructModel::eColFormat,100);
//  _StructTree->header()->resizeSection(DataStructModel::eColPostfix,75);
//
//  _StructTree->setEditTriggers(QAbstractItemView::AllEditTriggers);
//
//  TestRegexDelegate *tTestRegexDelegate =
//      new TestRegexDelegate (_DataStructModel,this);
//  _StructTree->setItemDelegateForColumn(
//      DataStructModel::eColTestRegex,tTestRegexDelegate);
//
//  ComboBoxDelegate *tTestScopeDelegate =
//      new ComboBoxDelegate(_DataStructModel->getTestNodes(),this);
//  _StructTree->setItemDelegateForColumn(
//      DataStructModel::eColTestScope,tTestScopeDelegate);
//
//  ComboBoxDelegate *tFormatDelegate =
//      new ComboBoxDelegate(_DataStructModel->getFormats(),this);
//  _StructTree->setItemDelegateForColumn(
//      DataStructModel::eColFormat,tFormatDelegate);
//
//  ComboBoxDelegate *tPostfixDelegate =
//      new ComboBoxDelegate(_DataStructModel->getPostfixes(),this);
//  _StructTree->setItemDelegateForColumn(
//      DataStructModel::eColPostfix,tPostfixDelegate);
//
//// TODO works form 4.8 on
//#ifdef EXPAND_ALL
//  _StructTree->expandAll();
//#else
//  _StructTree->expand(_StructTree->model()->index(0, 0, QModelIndex()));
//#endif
//
//  return tTreeView;

  /*
   * The options will only be populated when in filter mode.
   */
  if (_IsFilterMode)
  {
    _OptionsWidget = createOptionsWidget(this);
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
  if (_OptionsWidget)
  {
    tWindowLayout->addWidget(_OptionsWidget);
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

  QComboBox *tComboBox = new QComboBox(this);
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
QWidget *MainWindow::createOptionsWidget(QWidget *aParent)
{
    QWidget *tOptions = new QWidget(aParent);

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

    return tOptions;
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
