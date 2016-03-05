#include <QHeaderView>

#include "TestRegexDelegate.hh"

#include "StructTreeView.hh"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StructTreeView::StructTreeView(QWidget *aParent,DataStructModel *aDataStructModel)
  : QTreeView(aParent),
    _DataStructModel(aDataStructModel)
{
  setModel(_DataStructModel);
  setupView();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StructTreeView::~StructTreeView()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructTreeView::setDataStructModel(DataStructModel * aDataStructModel)
{
  _DataStructModel = aDataStructModel;

  /*
   * Set the new model to the tree.
   */
  setModel(_DataStructModel);

  /*
   * Update the test scope combo box. Create and install the new one, and
   * cleanup the old one.
   */
  ComboBoxDelegate *tNewComboBox =
      new ComboBoxDelegate(_DataStructModel->getTestNodes(),this);

  setItemDelegateForColumn(
      DataStructModel::eColTestScope,tNewComboBox);

  delete _TestScopeDelegate;

  _TestScopeDelegate = tNewComboBox;

#if 0 //  seems to have no effect
  resizeColumnToContents(0);
  resizeColumnToContents(1);
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructTreeView::expandToChecked(bool aCollapseFirst)
{
  if (aCollapseFirst)
  {
    collapseAll();
  }

  std::vector<FieldItem *> tFieldItems = _DataStructModel->getTreeItems();
  std::vector<FieldItem *>::iterator tIter;
  for (tIter = tFieldItems.begin(); tIter != tFieldItems.end(); tIter++)
  {
    FieldItem *aNode = *tIter;

    if (aNode->getData().isChecked())
    {
      FieldItem *tParent;
      while ((tParent = aNode->parentItem()) != NULL)
      {
        QModelIndex tModelIndex = _DataStructModel->index(tParent,0);
        expand(tModelIndex);
//        if (tParent->getData().getNodeType() == aNodeType)
//        {
//          tReturn = tParent;
//          break;
//        }
        aNode = tParent; // advance to next ancestor
      }


    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructTreeView::setupView()
{
//  tTreeView->setSelectionMode(QAbstractItemView::MultiSelection);
  setSelectionMode(QAbstractItemView::ContiguousSelection);

  header()->resizeSection(DataStructModel::eColFieldName,225);
  header()->resizeSection(DataStructModel::eColTestRegex,225);
  header()->resizeSection(DataStructModel::eColTestScope,175);
  header()->resizeSection(DataStructModel::eColFormat,100);
  header()->resizeSection(DataStructModel::eColPostfix,75);

  setEditTriggers(QAbstractItemView::AllEditTriggers);

  TestRegexDelegate *tTestRegexDelegate =
      new TestRegexDelegate (_DataStructModel,this);
  setItemDelegateForColumn(
      DataStructModel::eColTestRegex,tTestRegexDelegate);

  _TestScopeDelegate =
      new ComboBoxDelegate(_DataStructModel->getTestNodes(),this);
  setItemDelegateForColumn(
      DataStructModel::eColTestScope,_TestScopeDelegate);

  ComboBoxDelegate *tFormatDelegate =
      new ComboBoxDelegate(FieldItemData::getFormatStringList(),this);
  setItemDelegateForColumn(
      DataStructModel::eColFormat,tFormatDelegate);

  ComboBoxDelegate *tPostfixDelegate =
      new ComboBoxDelegate(_DataStructModel->getPostfixes(),this);
  setItemDelegateForColumn(
      DataStructModel::eColPostfix,tPostfixDelegate);

// TODO works form 4.8 on
#ifdef EXPAND_ALL
  expandAll();
#else
  expand(model()->index(0, 0, QModelIndex()));
#endif
}
