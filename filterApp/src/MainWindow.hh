#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_

#include <string>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QStringList>
#include <QWidget>
#include <QMainWindow>
#include "AppConfigFile.hh"
#include "DataStructModel.hh"
#include "MessageConfigFile.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "StructorBuilder.hh"
#include "StructTreeView.hh"

class MainWindow: public QMainWindow
{
  Q_OBJECT
public:

  static StructorBuilder *parseHeaderFile(QString aHeaderFile);

  // constructor for browse mode
  MainWindow(QApplication &aApp,QWidget *aParent,AppConfigFile *aAppConfigFile,
      StructorBuilder *_StructorBuilder);

  // constructor for filter mode //TODO remove 0 default values
  MainWindow(QApplication &aApp,QWidget *aParent,AppConfigFile *aAppConfigFile,
//      StructorBuilder *_StructorBuilder,bool aIsFilterMode = false,TODO rm
      StreamReader *aStreamReader = 0,RecordProcessor *aRecordProcessor = 0);

  virtual ~MainWindow();

	void setupView(std::string aStructName);

	void setInitialStructName(std::string aStructName); //TODO

	void setTreeViewStruct(std::string aStructName); //TODO

	DataStructModel *getDataStructModel(){return _DataStructModel;}//TODO

signals:

  void dataStructModelAvailable(void *aModel);

  void applyFormatMode(
      int aFormatMode,bool aSelectedOnly,QModelIndexList aSelectList);

  void applyTestScope(QString aTestScope,bool aCheckedOnly);

public slots:

  // handle menu/toolbar actions
  void onFileOpenAction();
  void onFileSaveAction();
  void onOutputModeAction(QAction *aAction);
  void onCustomFormatToolAction();
  void onTestScopeToolAction();

  // handle custom format tool pushbutton
  void onAsIsPushbuttonClicked(bool);
  void onLongnamePushbuttonClicked(bool);
  void onTablePushbuttonClicked(bool);

  // handle test scope tool pushbutton
  void onApplyTestScopeClicked(bool);

  // start filter mode once input stream struct name is available
  void onStructNameAvailable(QString aMsgId,QString aStructName);

  // handle user selection of struct in browse mode
  void onStructComboBoxActivated(int index);

  // handle changes to data model to repaint struct tree
  void onModelUpdate();

protected:
  AppConfigFile      *_AppConfigFile;

  QWidget            *_CentralWidget;
  QLabel             *_StatusLabel;

	StructorBuilder    *_StructorBuilder;
  bool                _IsFilterMode;
  StreamReader       *_StreamReader;
  RecordProcessor    *_RecordProcessor;
	std::string         _StructName;
	DataStructModel    *_DataStructModel;
	QComboBox          *_StructComboBox;
	QComboBox          *_TestScopeComboBox;
	QWidget            *_CustomFormatToolWidget;
	QWidget            *_TestScopeToolWidget;
	QWidget            *_OperateWidget;
	StructTreeView     *_StructTree;
	QCheckBox          *_PropagateCheckBox;
	QDockWidget        *_CustomFormatToolDock;
	QDockWidget        *_TestScopeToolDock;

  QRadioButton       *_DelimitAllButton;
  QRadioButton       *_DelimitOutputButton;
  QRadioButton       *_DelimitNoneButton;

  QRadioButton       *_FormatAsIsButton;
  QRadioButton       *_FormatLongnameButton;
  QRadioButton       *_FormatTableButton;
  QRadioButton       *_FormatCustomButton;

  QButtonGroup       *_OutputModeButtonGroup;
  QRadioButton       *_OutputNormalButton;
  QRadioButton       *_OutputBypassButton;
  QRadioButton       *_OutputFreezeDropButton;
  QRadioButton       *_OutputFreezeQueueButton;

  QCheckBox *_AsIsCheckBox;
  QCheckBox *_LongnameCheckBox;
  QCheckBox *_TableCheckBox;

  MessageConfigFile *_MessageConfigFile;

  void setupMenuAndToolbar();

  void setupFileActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupOperateActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupDelimitActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupFormatActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupToolActions(QMenu *aMenu,QToolBar *aToolBar);

  void setDelimitMode(StreamReader::DelimitMode aMode);

  void setStatusLabel(QString aStatus,QPalette aPalette);

  StructTreeView *createTreeView(QWidget *aParent);

  QComboBox *createStructComboBox(QWidget *aParent);

  QWidget *createTestScopeToolWidget(QWidget *aParent);
  QGroupBox *createTestScopeToolGroup(QWidget *aParent);

  QWidget *createCustomFormatToolWidget(QWidget *aParent);
  QGroupBox *createCustomFormatToolGroup(QWidget *aParent);

	QStringList convertToQStringList(std::vector<std::string> aStrings);

	std::string getMatchString();

	void setTreeViewStruct(
	    StructTreeView *aStructTreeView,std::string aStructName);
};

#endif /* MAINWINDOW_HH_ */
