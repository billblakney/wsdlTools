#ifndef MessageSpec_hh
#define MessageSpec_hh

#include <map>
#include <QString>

class MessageSpec
{
public:
  MessageSpec();
  MessageSpec(QString aId,QString aStructName,QString aHeader,QString aFilter);
  virtual ~MessageSpec();


  const QString& getHeader() const;

  void SetHeader(const QString& header);

  const QString& getId() const;

  void SetId(const QString& id);

  const QString& getStructName() const;

  void SetStructName(const QString& structName);

  const QString& getFilter() const;

  void SetFilter(const QString& filter);

  QString toQString();

protected:
  QString _Id;
  QString _StructName;
  QString _Header;
  QString _Filter;
};

typedef std::map<QString,MessageSpec> MessageSpecMap;

#endif /* MessageSpec_hh */
