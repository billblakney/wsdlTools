#ifndef DATASTRUCTMODEL_HH_
#define DATASTRUCTMODEL_HH_

#include <QAbstractItemModel>
#include <QFont>
#include <QModelIndex>
#include <QVariant>
#include "FieldItem.hh"
#include "Logger.hh"
#include "Structure.hh"
#include "StructorBuilder.hh"

class DataStructModel : public QAbstractItemModel
{
public:

  enum ColumnId {
    eColFieldName = 0,
    eColTestRegex,
    eColTestKey,
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
  FieldItem *getTopNode();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::vector<std::string> getTestNodes();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::vector<std::string> getPostfixes();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  void printTestNodes();

  //---------------------------------------------------------------------------
  //---------------------------------------------------------------------------
  std::string getFirstFieldMatch();

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

  bool processStructLines(std::vector<std::string> &aLinesIn,
                          std::vector<std::string> &aLinesOut);

  void printInLines();

  std::string getDotString(StructorBuilder *aStructBuilder,
    std::string aName,std::string aPrefix);

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

protected:

  static ccl::Logger sLogger;

  static QFont kArrayFont;

  StructorBuilder *_StructBuilder;

  // root and top nodes
  FieldItem *_RootItem;
  FieldItem *_TopNodeItem;

  // test nodes
  std::vector<std::string> _TestScopes;

  std::vector<std::string> _Postfixes;

  uint getStringVectorIndex(const std::vector<std::string> &aStringVector,
      const std::string &aString) const;

  void setupPostfixes();

  uint getPostfixIndex(std::string &aPostfix) const;

  uint getTestScopeIndex(std::string &aTestScope) const;

  void setupModelData() {}

  void setChildrenCheckStates(
      const QModelIndex &aParentIndex,Qt::CheckState aCheckState);

  void updateParentCheckState(
      const QModelIndex &aChildIndex,Qt::CheckState aNewState);
};

#endif /* DATASTRUCTMODEL_HH_ */
