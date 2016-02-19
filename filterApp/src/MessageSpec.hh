#ifndef MESSAGESPEC_H
#define MESSAGESPEC_H

#include <QString>

class MessageSpec
{
public:
  MessageSpec();
  MessageSpec(QString aId,QString aStructName,QString aHeader);
  virtual ~MessageSpec();


  const QString& getHeader() const;

  void SetHeader(const QString& header);

  const QString& getId() const;

  void SetId(const QString& id);

  const QString& getStructName() const;

  void SetStructName(const QString& structName);

protected:
  QString _Id;
  QString _StructName;
  QString _Header;
};

#endif /* MESSAGESPEC_H */
