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
#include "DataStructModel.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "StructorBuilder.hh"
#include "StructTreeView.hh"

class MainWindow: public QMainWindow
{
  Q_OBJECT
public:

  MainWindow(QApplication &aApp,QWidget *aParent,
      StructorBuilder *_StructorBuilder,bool aIsFilterMode = false,
      StreamReader *aStreamReader = 0,RecordProcessor *aRecordProcessor = 0);

  virtual ~MainWindow();

	void setupView(std::string aStructName);

	void setInitialStructName(std::string aStructName); //TODO

	void setTreeViewStruct(std::string aStructName); //TODO

	DataStructModel *getDataStructModel(){return _DataStructModel;}//TODO

signals:

  void dataStructModelAvailable(void *aModel);

  void applyFormatMode(int aFormatMode,bool aCheckedOnly);

public slots:

  // handle menu/toolbar actions
  void onFileOpenAction();
  void onFileSaveAction();
  void onOutputModeAction(QAction *aAction);
  void onCustomFormatToolAction();

  // handle custom format tool pushbutton
  void onAsIsPushbuttonClicked(bool);
  void onLongnamePushbuttonClicked(bool);
  void onTablePushbuttonClicked(bool);

  // start filter mode once input stream struct name is available
  void onStructNameAvailable(QString aStructName);

  // handle user selection of struct in browse mode
  void onStructComboBoxActivated(int index);

  // handle changes to data model to repaint struct tree
  void onModelUpdate();

protected:

  QWidget            *_CentralWidget;
  QLabel             *_StatusLabel;

	StructorBuilder    *_StructorBuilder;
  bool                _IsFilterMode;
  StreamReader       *_StreamReader;
  RecordProcessor    *_RecordProcessor;
	std::string         _StructName;
	DataStructModel    *_DataStructModel;
	QComboBox          *_StructComboBox;
	QWidget            *_ConfigureWidget;
	QWidget            *_OperateWidget;
	StructTreeView     *_StructTree;
	QCheckBox          *_PropagateCheckBox;
	QDockWidget        *_CustomFormatToolsWidget;

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

  QWidget *createConfigureWidget(QWidget *aParent);

  QGroupBox *createCustomFormatGroup(QWidget *aParent);

	QStringList convertToQStringList(std::vector<std::string> aStrings);

	std::string getMatchString();

	void setTreeViewStruct(
	    StructTreeView *aStructTreeView,std::string aStructName);
};

#endif /* MAINWINDOW_HH_ */
