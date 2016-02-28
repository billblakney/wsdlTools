#ifndef StructTreeView_hh
#define StructTreeView_hh

#include <QTreeView>

#include "ComboBoxDelegate.hh"

#include "DataStructModel.hh"

class StructTreeView: public QTreeView
{
  Q_OBJECT
public:

  StructTreeView(QWidget *aParent,DataStructModel *aDataStructModel);

  virtual ~StructTreeView();

  void setDataStructModel(DataStructModel * aDataStructModel);

protected:
  ComboBoxDelegate *_TestScopeDelegate;

  void setupView(DataStructModel *aDataStructModel);
};

#endif /* StructTreeView_hh */
