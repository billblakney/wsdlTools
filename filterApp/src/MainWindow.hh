#ifndef MainWindow_hh
#define MainWindow_hh

#include <string>
#include <QActionGroup>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QStringList>
#include <QWidget>
#include <QMainWindow>
#include "AppConfigReader.hh"
#include "DataStructModel.hh"
#include "DelimitActionGroup.hh"
#include "FilterReader.hh"
#include "OperateActionGroup.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "StructorBuilder.hh"
#include "StructTreeView.hh"

class MainWindow: public QMainWindow
{
  Q_OBJECT
public:

  static const QString kAppName;
  static const QString kTitle;

  static StructorBuilder *parseHeaderFile(QString aHeaderFile);

  // constructor for browse mode
  MainWindow(QApplication &aApp,QWidget *aParent,AppConfig aAppConfig,
      MessageSpecMap aMessageSpecMap,StructorBuilder *_StructorBuilder);

  // constructor for filter mode //TODO remove 0 default values
  MainWindow(QApplication &aApp,QWidget *aParent,AppConfig aAppConfig,
      MessageSpecMap aMessageSpecMap,StreamReader *aStreamReader = 0,
      RecordProcessor *aRecordProcessor = 0);

  virtual ~MainWindow();

	void setupView(std::string aStructName);

	void setInitialStructName(std::string aStructName); //TODO

	void setupDataStructModel(std::string aStructName); //TODO

	DataStructModel *getDataStructModel(){return _DataStructModel;}//TODO

signals:

  void dataStructModelAvailable(void *aModel);

  void applyFormatMode(
      int aFormatMode,bool aSelectedOnly,QModelIndexList aSelectList);

  void applyTestScope(QString aTestScope,bool aCheckedOnly);

  void togglePropagateChecks(bool);

public slots:

  // handle menu/toolbar actions
  void onOpenFilterAction();
  void onOpenCustomFilterAction();
  void onSaveCustomFilterAction();
  void onOperateModeAction(QAction *aAction);
  void onCustomFormatToolAction();
  void onPropagateCheckAction();
  void onCollapseAllAction();
  void onExpandToCheckedAction();
  void onExpandAllAction();
  void onTestScopeToolAction();

  void onViewFileToolbarAction();
  void onViewOperateToolbarAction();
  void onViewDelimitToolbarAction();
  void onViewFormatToolbarAction();
  void onViewTreeToolbarAction();
  void onViewToolToolbarAction();

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
  AppConfig           _AppConfig;
  MessageSpecMap      _MessageSpecMap;

  QToolBar           *_FileToolBar;
  QToolBar           *_OperateToolBar;
  QToolBar           *_DelimitToolBar;
  QToolBar           *_FormatToolBar;
  QToolBar           *_TreeToolBar;
  QToolBar           *_ToolToolBar;

  OperateActionGroup *_OperateActionGroup;
  DelimitActionGroup *_DelimitActionGroup;

  QAction             *_ViewFileToolbarAction;
  QAction             *_ViewOperateToolbarAction;
  QAction             *_ViewDelimitToolbarAction;
  QAction             *_ViewFormatToolbarAction;
  QAction             *_ViewTreeToolbarAction;
  QAction             *_ViewToolToolbarAction;

  QAction             *_PropagateCheckAction;

  QWidget            *_CentralWidget;
  QLabel             *_StatusLabel;

	StructorBuilder    *_StructorBuilder;
  bool                _IsFilterMode;
  StreamReader       *_StreamReader;
  RecordProcessor    *_RecordProcessor;
	std::string         _StructName;
	QString             _MessageId;
	DataStructModel    *_DataStructModel;
	QComboBox          *_StructComboBox;
	QComboBox          *_TestScopeComboBox;
	QWidget            *_CustomFormatToolWidget;
	QWidget            *_TestScopeToolWidget;
	QWidget            *_OperateWidget;
	StructTreeView     *_StructTree;
	QDockWidget        *_CustomFormatToolDock;
	QDockWidget        *_TestScopeToolDock;

  QCheckBox *_AsIsCheckBox;
  QCheckBox *_LongnameCheckBox;
  QCheckBox *_TableCheckBox;

  FilterReader *_FilterReader;

  void init();

  void setupMenuAndToolbar();

  void setupFileActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupOperateActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupDelimitActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupFormatActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupTreeActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupToolActions(QMenu *aMenu,QToolBar *aToolBar);

  void setupToolBarMenu(QMenu *aMenu);

  void setDelimitMode(StreamReader::DelimitMode aMode);

  void setStatusLabel(QString aStatus,QPalette aPalette);

  QComboBox *createStructComboBox(QWidget *aParent);

  QWidget *createTestScopeToolWidget(QWidget *aParent);
  QGroupBox *createTestScopeToolGroup(QWidget *aParent);

  QWidget *createCustomFormatToolWidget(QWidget *aParent);
  QGroupBox *createCustomFormatToolGroup(QWidget *aParent);

	QStringList convertToQStringList(std::vector<std::string> aStrings);

	std::string getMatchString();

	std::string getHeaderPath(MessageSpec *aMessageSpec);

	void updateWindowTitle();
	void updateWindowTitle(QString aMsgId,QString aStructName);
	void updatePermanentWidget(QString aMsgId,QString aStructName);
};

#endif /* MainWindow_hh */
