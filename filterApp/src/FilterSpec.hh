#ifndef FilterSpec_hh
#define FilterSpec_hh

#include <vector>

#include "DataStructModel.hh"
#include "FieldSpec.hh"

class FilterSpec
{
public:
  FilterSpec();
  virtual ~FilterSpec();
  void apply(DataStructModel *aDataStructModel);

  bool _HasErrors;
  QString _OperateMode;
  QString _DelimitMode;
  std::vector<FieldSpec> _Fields;
protected:
  void updateMessageIsChecked(FieldItem *aItem,QString aIsChecked);
  void updateMessageTestScope(FieldItem *aItem,QString aTestScope);
  void updateMessageFormat(FieldItem *aItem,QString aFormat);
  void updateMessagePostfix(FieldItem *aItem,QString aPostfix);
};

#endif /* FilterSpec_hh */
