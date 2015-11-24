#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_

#include <string>
#include <QComboBox>
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

public slots:

  void onStructNameAvailable(QString aStructName);

  void onStructComboBoxActivated(int index);

  void onSetFilterClicked(bool);

  void onFormatOptionSelection(bool /*aIsChecked*/); //TODO make protected?

protected:

  bool                _IsFilterMode;
  StreamReader       *_StreamReader;
  RecordProcessor    *_RecordProcessor;
	std::string         _HeaderFile;
	std::string         _InitialStruct;
	StructorBuilder    *_StructorBuilder;
	DataStructModel    *_DataStructModel;
	QComboBox          *_StructComboBox;
	StructTreeView     *_StructTree;
  bool               _HeaderFileWasParsed;

  QRadioButton       *_FormatAsIsButton;
  QRadioButton       *_FormatLongnameButton;
  QRadioButton       *_FormatTableButton;
  QRadioButton       *_FormatCustomButton;

  void readEnvironmentVariables();

  void processCommandLine(int argc,char *argv[]);

	QStringList convertToQStringList(std::vector<std::string> aStrings);

	std::string getMatchString();
};

#endif /* MAINWINDOW_HH_ */
