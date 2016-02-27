#ifndef DelimitActionGroup_hh
#define DelimitActionGroup_hh

#include <QActionGroup>

#include "StreamReader.hh"

class DelimitActionGroup: public QActionGroup
{
  Q_OBJECT
public:
  DelimitActionGroup(QWidget *aParent);
  virtual ~DelimitActionGroup();
  QAction *getAction(StreamReader::DelimitMode aDelimitMode);
};

#endif /* DelimitActionGroup_hh */
