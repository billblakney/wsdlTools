#include <iostream>
#include <QDir>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "PickFilterDialog.hh"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PickFilterDialog::PickFilterDialog(QWidget *aParent,std::string aStructName,
    AppConfig aAppConfig,FilterReader *aFilterReader)
  : QDialog(aParent),
    _StructName(aStructName)
{
  setWindowTitle(QString("Select Filter"));

  /*
   * Create table.
   */
  _Table = new QTableWidget(this);
  _Table->setSelectionMode(QAbstractItemView::SingleSelection);
  _Table->setSelectionBehavior(QAbstractItemView::SelectRows);
  _Table->setColumnCount(2);

  /*
   * Setup table columns.
   */
  _Table->horizontalHeader()->setStretchLastSection(true);

  QTableWidgetItem *tFilterHeaderItem = new QTableWidgetItem(QString("Filters"));
  _Table->setHorizontalHeaderItem(0, tFilterHeaderItem);
  tFilterHeaderItem->setTextAlignment(Qt::AlignVCenter);

  QTableWidgetItem *tDescriptionHeaderItem = new QTableWidgetItem(QString("Descriptions"));
  _Table->setHorizontalHeaderItem(1, tDescriptionHeaderItem);
  tDescriptionHeaderItem->setTextAlignment(Qt::AlignVCenter);

  /*
   * Get list of filters.
   */
  QDir tDir;

  QStringList tFilters;
  tFilters.push_back(QString(_StructName.c_str()) + ".*");

  tDir.setCurrent(aAppConfig.getDefaultFiltersDir());
  QStringList tList = tDir.entryList(tFilters);

  /*
   * Create a row for each filter file.
   */
  _Table->setRowCount(tList.size());
  for (int tIdx = 0; tIdx < tList.size(); tIdx++)
  {
    QString tFile = tList[tIdx];

    /*
     * Strip the structure name prefix form the filename (to save display
     * space), and set that value to the first column item.
     */
    QTableWidgetItem *tFilterItem = new QTableWidgetItem(
        tFile.right(tFile.length()-_StructName.length()-1));
    _Table->setItem(tIdx,0,tFilterItem);

    /*
     * Read the filter file to get it's description, and use that value for
     * the second column item.
     */
    FilterSpec tFilter = aFilterReader->openFilter(
        aAppConfig.getDefaultFiltersDir(),tFile);

    QTableWidgetItem *tDescriptionItem = new QTableWidgetItem(tFilter._Description);
    _Table->setItem(tIdx,1,tDescriptionItem);
  }

  /*
   * Create buttons.
   */
  QWidget *tButtonWidget = new QWidget();
  QPushButton *tApplyButton =
      new QPushButton( QString("&Apply Filter"),tButtonWidget);
  QPushButton *tCloseButton =
      new QPushButton(QString("Close"),tButtonWidget);
  tApplyButton->setDefault(true);

  connect(tApplyButton, SIGNAL(clicked()),
      this, SLOT(accept()));
  connect(tCloseButton, SIGNAL(clicked()),
      this, SLOT(reject()));

  /*
   * Create layout for the buttons.
   */
  QHBoxLayout *tButtonLayout = new QHBoxLayout;
  tButtonLayout->addWidget(tApplyButton);
  tButtonLayout->addWidget(tCloseButton);
  tButtonWidget->setLayout(tButtonLayout);

  QVBoxLayout *tLayout = new QVBoxLayout;
  tLayout->addWidget(_Table);
  tLayout->addWidget(tButtonWidget);
  tLayout->addStretch();

  setLayout(tLayout);

  QItemSelectionModel *sm = _Table->selectionModel();
  connect(sm, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
              this, SLOT(onCurrentRowChanged(QModelIndex,QModelIndex)));

  _Table->resizeColumnsToContents();

  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  QSize tParentSize = parentWidget()->size();
  QSize tSize(0.8*tParentSize.width(),0.6*tParentSize.height());
  resize(tSize);
}

//------------------------------------------------------------------------------
// TODO used?
//------------------------------------------------------------------------------
QSize PickFilterDialog::sizeHint()
{
  QSize tParentSize = parentWidget()->size();
  QSize tSize(tParentSize.width(),tParentSize.height());
  return tSize;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
PickFilterDialog::~PickFilterDialog()
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
QString PickFilterDialog::getSelectedFilter()
{
  return _SelectedFiter;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void PickFilterDialog::onCurrentRowChanged(
    QModelIndex aCurrentIndex,QModelIndex aPreviousIndex)
{
  Q_UNUSED(aPreviousIndex);
  QString tSelection = _Table->item(aCurrentIndex.row(),0)->text();
  _SelectedFiter = QString(_StructName.c_str()) + QString(".") + tSelection;
}

