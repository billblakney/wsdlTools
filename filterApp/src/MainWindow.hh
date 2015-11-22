#ifndef MAINWINDOW_HH_
#define MAINWINDOW_HH_

#include <string>
#include <QComboBox>
#include <QStringList>
#include <QWidget>
#include "StructorBuilder.hh"
#include "DataStructModel.hh"
#include "SimpleRecordWriter.hh"
#include "StreamReader.hh" //TODO
#include "StructTreeView.hh"

class MainWindow: public QWidget
{
  Q_OBJECT
public:

  MainWindow(int argc,char *argv[],QApplication &aApp,QWidget *aParent,
      StreamReader *aStreamReader = 0);

  virtual ~MainWindow();

  void parseHeaderFile();

	void setupView();

	void setInitialStructName(std::string aStructName); //TODO

	void setTreeViewStruct(std::string aStructName); //TODO

	DataStructModel *getDataStructModel(){return _DataStructModel;}//TODO

signals:

  void dataStructModelAvailable(void *aModel);

public slots:

  void onStructNameAvailable(QString aStructName);

  void onStructComboBoxActivated(int index);

  void onSetFilterClicked(bool);

protected:

	std::string         _HeaderFile;
	std::string         _InitialStruct;
	StructorBuilder    *_StructorBuilder;
	DataStructModel    *_DataStructModel;
	QComboBox          *_StructComboBox;
	StructTreeView     *_StructTree;
  SimpleRecordWriter *_Writer;
  StreamReader       *_StreamReader;
  bool               _HeaderFileWasParsed;

  void readEnvironmentVariables();

  void processCommandLine(int argc,char *argv[]);

	QStringList convertToQStringList(std::vector<std::string> aStrings);

	std::string getMatchString();
};

#endif /* MAINWINDOW_HH_ */
