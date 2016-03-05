#ifndef FilterSpec_hh
#define FilterSpec_hh

#include <vector>
#include <QActionGroup>

#include "DataStructModel.hh"
#include "DelimitActionGroup.hh"
#include "FormatActionGroup.hh"
#include "OperateActionGroup.hh"
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "FieldSpec.hh"

class FilterSpec
{
public:
  FilterSpec();
  virtual ~FilterSpec();
  void apply(DataStructModel *aDataStructModel,
      OperateActionGroup *aOperateActionGroup,
      DelimitActionGroup *aDelimitActionGroup,
      FormatActionGroup *aFormatActionGroup);

  bool _HasErrors;
  QString _Description;
  QString _OperateMode;
  QString _DelimitMode;
  QString _FormatMode;
  std::vector<FieldSpec> _Fields;
protected:
  void updateMessageIsChecked(FieldItem *aItem,QString aIsChecked);
  void updateMessageTestScope(FieldItem *aItem,QString aTestScope);
  void updateMessageFormat(FieldItem *aItem,QString aFormat);
  void updateMessagePostfix(FieldItem *aItem,QString aPostfix);
};

#endif /* FilterSpec_hh */
