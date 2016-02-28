#include <QHeaderView>

#include "TestRegexDelegate.hh"

#include "StructTreeView.hh"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StructTreeView::StructTreeView(QWidget *aParent,DataStructModel *aDataStructModel)
  : QTreeView(aParent)
{
  setModel(aDataStructModel);
  setupView(aDataStructModel);
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
  /*
   * Set the new model to the tree.
   */
  setModel(aDataStructModel);

  /*
   * Update the test scope combo box. Create and install the new one, and
   * cleanup the old one.
   */
  ComboBoxDelegate *tNewComboBox =
      new ComboBoxDelegate(aDataStructModel->getTestNodes(),this);

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
void StructTreeView::setupView(DataStructModel *aDataStructModel)
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
      new TestRegexDelegate (aDataStructModel,this);
  setItemDelegateForColumn(
      DataStructModel::eColTestRegex,tTestRegexDelegate);

  _TestScopeDelegate =
      new ComboBoxDelegate(aDataStructModel->getTestNodes(),this);
  setItemDelegateForColumn(
      DataStructModel::eColTestScope,_TestScopeDelegate);

  ComboBoxDelegate *tFormatDelegate =
      new ComboBoxDelegate(FieldItemData::getFormatStringList(),this);
  setItemDelegateForColumn(
      DataStructModel::eColFormat,tFormatDelegate);

  ComboBoxDelegate *tPostfixDelegate =
      new ComboBoxDelegate(aDataStructModel->getPostfixes(),this);
  setItemDelegateForColumn(
      DataStructModel::eColPostfix,tPostfixDelegate);

// TODO works form 4.8 on
#ifdef EXPAND_ALL
  expandAll();
#else
  expand(model()->index(0, 0, QModelIndex()));
#endif
}
