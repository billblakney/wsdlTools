#include "OperateActionGroup.hh"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
OperateActionGroup::OperateActionGroup(QWidget *aParent)
  : QActionGroup(aParent)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
OperateActionGroup::~OperateActionGroup()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QAction *OperateActionGroup::getAction(StreamReader::OperateMode aOperateMode)
{
  QAction *tAction = NULL;

  QList<QAction *> tActions = actions();
  for (int tIdx = 0; tIdx < tActions.size(); tIdx++)
  {
      if (aOperateMode ==
          static_cast<StreamReader::OperateMode>(
              tActions[tIdx]->data().toInt()))
      {
        tAction = tActions[tIdx];
        break;
      }
  }
  return tAction;
}
