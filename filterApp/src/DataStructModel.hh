#ifndef DataStructModel_hh
#define DataStructModel_hh

#include <vector>
#include <QAbstractItemModel>
#include <QFont>
#include <QModelIndex>
#include <QVariant>

#include "FieldItem.hh"
#include "Logger.hh"
#include "RecordProcessor.hh"
#include "Structure.hh"
#include "StructorBuilder.hh"

class DataStructModel : public QAbstractItemModel
{
  Q_OBJECT;

public:

  enum ColumnId {
    eColFieldName = 0,
    eColTestRegex,
    eColTestScope,
    eColFormat,
    eColPostfix,
    eColFieldKey,
    eColFieldType,
    eColMatchRegex,
    eNumColumns
  };

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  DataStructModel(Structure *aStructure);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  virtual ~DataStructModel();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  DataStructModel(
      Structure *aStructure,StructorBuilder *aStructBuilder);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  int childCount(FieldItem *aNode,FieldItemData::NodeType aType);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  FieldItem *getTopNode();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  FieldItem *getNode(std::string aKey);

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::vector<FieldItem *> &getTreeItems();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::vector<std::string> getTestNodes();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::vector<std::string> getPostfixes();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  Enum *getEnum(const QModelIndex &aIndex) const;

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  void printTestNodes();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::string getFirstFieldMatch();

  bool processStructLines(std::vector<std::string> &aLinesIn,
                          std::vector<std::string> &aLinesOut);

  void printInLines();

  std::string getMatchString(FieldItem *aFieldItem);

  std::string getMatchString();

  std::vector<FieldItem *> getCheckedFields();

  void addCheckedFields(
      FieldItem *aNode,std::vector<FieldItem *> &aFieldItems);

  QVariant data(const QModelIndex &index,int role) const;

  bool setData(const QModelIndex &index,const QVariant &value,
      int role = Qt::EditRole);

  Qt::ItemFlags flags(const QModelIndex &index) const;

  QVariant headerData(int section,Qt::Orientation orientation,
      int role = Qt::DisplayRole) const;

  QModelIndex index(int row,int col,
      const QModelIndex &parent = QModelIndex()) const;

  QModelIndex parent(const QModelIndex &index) const;

  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  int columnCount(const QModelIndex &parent = QModelIndex()) const;

signals:

  void modelUpdated();

public slots:

  void applyFormatMode(
      int aFormatMode,bool aSelectedOnly,QModelIndexList aSelectList);

  void applyTestScope(QString aTestScope,bool aCheckedOnly);

  void onPropogateToggled(bool aPropogateToChildren);

protected:

  static ccl::Logger sLogger;

  static QFont kArrayFont;

  StructorBuilder *_StructBuilder;

  // root node
  FieldItem *_RootItem;

  // top node
  FieldItem *_TopNodeItem;

  // tree nodes
  std::vector<FieldItem *> _TreeItems;

  bool       _PropogateFieldChecks;

  // test nodes
  std::vector<std::string> _TestScopes;
  std::vector<std::string> _Postfixes;

  void buildTree(FieldItem *rootItem,Structure *aStructure,int aLevel = 0);

  void buildStructNode(
      Field &aField,FieldItem *aParentItem,int &aLevel);
  void buildStructArrayNode(
      Field &aField,FieldItem *aParentItem,int &aLevel);
  void buildStructArrayLengthNode(
      Field &aField,FieldItem *aParentItem,int &aLevel);
  void buildPrimitiveArrayNode(
      Field &aField,FieldItem *aParentItem,int &aLevel);
  void buildPrimitiveArrayLengthNode(
      Field &aField,FieldItem *aParentItem,int &aLevel);
  void buildPrimitiveNode(
      Field &aField,FieldItem *aParentItem,int &aLevel);

  std::string buildKey(Field &aField,FieldItem *aParentItem,
      bool aIsArrayType = false);
  std::string buildArrayLengthKey(FieldItem *aParentItem);

  std::string buildMatchForField(
      const Field &aField,int aIndentLevel);
  std::string buildMatchForArrayLengthField(
      const Field &aField,int aIndentLevel);
  std::string buildMatchForStructField(
      const Field &aField,int aIndentLevel);
  std::string buildMatchForStructArrayField(
      const Field &aField,int aIndentLevel);
  std::string buildMatchForStructArrayLengthField(
      const Field &aField,int aIndentLevel);
  std::string buildMatchForPrimitiveField(
      const Field &aField,int aIndentLevel);
  std::string buildMatchForPrimitiveArrayField(
      const Field &aField,int aIndentLevel);
  std::string buildMatchForPrimitiveArrayLengthField(
      const Field &aField,int aIndentLevel);

  void setFilterScopeForDesignator();
  FieldItem *findFirstAncestor(FieldItem *aNode,FieldItemData::NodeType aNodeType);
  void applyTestScope(FieldItem *aNode,std::string aTestScope);

  void appendToTreeItemVector(FieldItem *aNode);

  void transformTestRegex(std::string &aValue);

  uint getEnumIndex(Enum *aEnum,std::string aValue) const;

  uint getStringVectorIndex(const std::vector<std::string> &aStringVector,
      const std::string &aString) const;

  void setupFormats();
  void setupPostfixes();

  uint getPostfixIndex(std::string &aPostfix) const;
  uint getTestScopeIndex(std::string &aTestScope) const;

  void setupModelData() {}

  void applyTestScope(QVariant aTestScope,bool aCheckedOnly,
      FieldItem *aFieldItem);

  void applyFormatMode(QVariant aFormat,QVariant aPostfix,
      FieldItem *aFieldItem);

  void applyFormatMode(QVariant aFormat,QVariant aPostfix,
      QModelIndexList aSelectedList);

  void setChildrenCheckStates(
      const QModelIndex &aParentIndex,Qt::CheckState aCheckState);

  void updateParentCheckState(
      const QModelIndex &aChildIndex,Qt::CheckState aNewState);
};

#endif /* DataStructModel_hh */
