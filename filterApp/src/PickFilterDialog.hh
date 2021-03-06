#ifndef PickFilterDialog_hh
#define PickFilterDialog_hh

#include <QDialog>
#include <QPushButton>
#include <QTableWidget>

#include "AppConfig.hh"
#include "FilterReader.hh"

class PickFilterDialog: public QDialog
{
  Q_OBJECT;
public:
  PickFilterDialog(QWidget *aParent,std::string aStructName,
      AppConfig aAppConfig,FilterReader *aFilterReader);
  virtual ~PickFilterDialog();
  QString getSelectedFilter();

signals:

public slots:
  void onCurrentRowChanged(QModelIndex,QModelIndex);

protected:
  QTableWidget   *_Table;
  QString         _SelectedFiter;
  std::string     _StructName;

  void setupView(AppConfig aAppConfig,FilterReader *aFilterReader);
};

#endif /* PickFilterDialog_hh */
