#include "FilterReader.hh"
#include "FilterSpec.hh"

#include "MainWindow.hh" //TODO remove when...

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
//TODO _OperateMode _DelimitMode
//-----------------------------------------------------------------------------
void FilterSpec::apply(DataStructModel *aDataStructModel,MainWindow *aMainWindow,
    StreamReader *aStreamReader,RecordProcessor *aRecordProcessor)
{

//  StreamReader::OperateMode aMode = StreamReader::eGo;
//  QList<QAction *> tActions = aOperateGroup->actions();
//  QAction *tOperateAction = NULL;
//  for (int tIdx = 0; tIdx < tActions.size(); tIdx++)
//  {
//    aMode =
//      static_cast<StreamReader::OperateMode>(tActions[tIdx]->data().toInt());
//      if (aMode == StreamReader::getOperateMode(_OperateMode))
//      {
//        tOperateAction = tActions[tIdx];
//        break;
//      }
//  }

  StreamReader::OperateMode tOperateMode = StreamReader::getOperateMode(_OperateMode);
  QAction *tOperateAction = aMainWindow->getOperateAction(tOperateMode);
  tOperateAction->activate(QAction::Trigger);



//  aStreamReader->setOperateMode(StreamReader::getOperateMode(_OperateMode));
  aStreamReader->setDelimitMode(StreamReader::getDelimitMode(_DelimitMode));

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

