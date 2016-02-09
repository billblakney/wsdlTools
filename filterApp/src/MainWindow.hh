#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_

#include <string>
#include <QComboBox>
#include <QGroupBox>
#include <QRadioButton>
#include <QStringList>
#include <QWidget>
#include "DataStructModel.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "StructorBuilder.hh"
#include "StructTreeView.hh"

class MainWindow: public QWidget
{
  Q_OBJECT
public:

  MainWindow(int argc,char *argv[],QApplication &aApp,QWidget *aParent,
      bool aIsFilterMode = false,StreamReader *aStreamReader = 0,
      RecordProcessor *aRecordProcessor = 0);

  virtual ~MainWindow();

  void parseHeaderFile();

	void setupView();

	void setInitialStructName(std::string aStructName); //TODO

	void setTreeViewStruct(std::string aStructName); //TODO

	DataStructModel *getDataStructModel(){return _DataStructModel;}//TODO

signals:

  void dataStructModelAvailable(void *aModel);

  void formatOptionSelected(int aFormatOption);

  void outputModeSelected(int aOutputMode);

  void applyFormatMode(int aFormatMode,bool aCheckedOnly);

public slots:

  void onStructNameAvailable(QString aStructName);

  void onModelUpdate();

  void onAsIsPushbuttonClicked(bool);
  void onLongnamePushbuttonClicked(bool);
  void onTablePushbuttonClicked(bool);

  void onStructComboBoxActivated(int index);

  void onSetFilterClicked(bool);

  void onFormatOptionSelection(bool /*aIsChecked*/); //TODO make protected?

  void onOutputModeButtonClicked(QAbstractButton *aButton);

protected:

  bool                _IsFilterMode;
  StreamReader       *_StreamReader;
  RecordProcessor    *_RecordProcessor;
	std::string         _HeaderFile;
	std::string         _InitialStruct;
	StructorBuilder    *_StructorBuilder;
	DataStructModel    *_DataStructModel;
	QComboBox          *_StructComboBox;
	QWidget            *_OptionsWidget;
	StructTreeView     *_StructTree;
  bool               _HeaderFileWasParsed;

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

  void readEnvironmentVariables();

  void processCommandLine(int argc,char *argv[]);

  QComboBox *createStructComboBox(QWidget *aParent);

  QWidget *createOptionsWidget(QWidget *aParent);

  QGroupBox *createFormatModeGroup(QWidget *aParent);

  QGroupBox *createOutputModeGroup(QWidget *aParent);

  QGroupBox *createCustomFormatGroup(QWidget *aParent);

  StructTreeView *createTreeView(QWidget *aParent);

	QStringList convertToQStringList(std::vector<std::string> aStrings);

	std::string getMatchString();
};

#endif /* MAINWINDOW_HH_ */
