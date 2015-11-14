#ifndef STRUCTTREEVIEW_HH_
#define STRUCTTREEVIEW_HH_

#include <QTreeView>

class StructTreeView: public QTreeView
{
  Q_OBJECT
public:
  StructTreeView(QWidget *aParent);
  virtual ~StructTreeView();
};

#endif /* STRUCTTREEVIEW_HH_ */
