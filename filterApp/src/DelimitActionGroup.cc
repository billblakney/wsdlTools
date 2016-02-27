#include "DelimitActionGroup.hh"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DelimitActionGroup::DelimitActionGroup(QWidget *aParent)
  : QActionGroup(aParent)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DelimitActionGroup::~DelimitActionGroup()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QAction *DelimitActionGroup::getAction(StreamReader::DelimitMode aDelimitMode)
{
  QAction *tAction = NULL;

  QList<QAction *> tActions = actions();
  for (int tIdx = 0; tIdx < tActions.size(); tIdx++)
  {
      if (aDelimitMode ==
          static_cast<StreamReader::DelimitMode>(
              tActions[tIdx]->data().toInt()))
      {
        tAction = tActions[tIdx];
        break;
      }
  }
  return tAction;
}
