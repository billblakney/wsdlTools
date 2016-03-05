#include "FormatActionGroup.hh"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FormatActionGroup::FormatActionGroup(QWidget *aParent)
  : QActionGroup(aParent)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FormatActionGroup::~FormatActionGroup()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QAction *FormatActionGroup::getAction(RecordProcessor::FormatMode aFormatMode)
{
  QAction *tAction = NULL;

  QList<QAction *> tActions = actions();
  for (int tIdx = 0; tIdx < tActions.size(); tIdx++)
  {
      if (aFormatMode ==
          static_cast<RecordProcessor::FormatMode>(
              tActions[tIdx]->data().toInt()))
      {
        tAction = tActions[tIdx];
        break;
      }
  }
  return tAction;
}
