#ifndef FormatActionGroup_hh
#define FormatActionGroup_hh

#include <QActionGroup>

#include "RecordProcessor.hh"

class FormatActionGroup: public QActionGroup
{
  Q_OBJECT
public:
  FormatActionGroup(QWidget *aParent);
  virtual ~FormatActionGroup();
  QAction *getAction(RecordProcessor::FormatMode aFormatMode);
};

#endif /* FormatActionGroup_hh */
