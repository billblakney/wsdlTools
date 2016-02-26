#include "FilterReader.hh"
#include "FilterSpec.hh"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterSpec::FilterSpec()
{
  _HasErrors = false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterSpec::~FilterSpec()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterSpec::apply(DataStructModel *aDataStructModel)
{
  std::vector<FieldSpec>::iterator tIter;

  for (tIter = _Fields.begin(); tIter != _Fields.end(); tIter++ )
  {
    FieldItem *tFieldItem = aDataStructModel->getNode(tIter->key.toStdString());

    if (tFieldItem)
    {
      updateMessageIsChecked(tFieldItem,tIter->isChecked);
      updateMessageTestScope(tFieldItem,tIter->testScope);
      updateMessageFormat(tFieldItem,tIter->format);
      updateMessagePostfix(tFieldItem,tIter->postfix);
    }
    else
    {
      std::cout << "WARNING: couldn't find tree node for key "
          << qPrintable(tIter->key) << std::endl;
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterSpec::updateMessageIsChecked(FieldItem *aItem,
    QString aIsChecked)
{
  bool tNewIsCheckedValue =
      (aIsChecked.toStdString().compare(FilterReader::kValueChecked)?false:true);
  bool tOldIsCheckedValue = aItem->getData().isChecked();

  if (tNewIsCheckedValue != tOldIsCheckedValue)
  {
    if (tNewIsCheckedValue)
    {
      aItem->setCheckState(Qt::Checked);
    }
    else
    {
      aItem->setCheckState(Qt::Unchecked);
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterSpec::updateMessageTestScope(FieldItem *aItem,
      QString aNewTestScope)
{
  QString tOldTestScope(aItem->getData().getTestScope().c_str());

  if (tOldTestScope != aNewTestScope)
  {
    aItem->setTestScope(aNewTestScope.toStdString().c_str());
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterSpec::updateMessageFormat(FieldItem *aItem,
    QString aNewFormat)
{
  FieldItemData::Format tOldFormatValue = aItem->getData().getFormat();
  QString tOldFormat = aItem->getData().getFormatString(tOldFormatValue);
  if (tOldFormat != aNewFormat)
  {
    FieldItemData::Format tNewFormatValue = FieldItemData::getFormat(aNewFormat);
    aItem->setFieldFormat(tNewFormatValue);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterSpec::updateMessagePostfix(FieldItem *aItem,
    QString aNewPostfix)
{
  QString tOldPostfix(aItem->getData().getPostfix().c_str());

  if (tOldPostfix != aNewPostfix)
  {
    aItem->setFieldPostfix(aNewPostfix.toStdString().c_str());
  }
}

