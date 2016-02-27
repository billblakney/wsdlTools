#ifndef OperateActionGroup_hh
#define OperateActionGroup_hh

#include <QActionGroup>

#include "StreamReader.hh"

class OperateActionGroup: public QActionGroup
{
  Q_OBJECT
public:
  OperateActionGroup(QWidget *aParent);
  virtual ~OperateActionGroup();
  QAction *getAction(StreamReader::OperateMode aOperateMode);
};

#endif /* OperateActionGroup_hh */
