#include "MessageSpec.hh"
//TODO args aX

MessageSpec::MessageSpec()
{
}

MessageSpec::MessageSpec(QString aId,QString aStructName,QString aHeader)
{
  _Id = aId;
  _StructName = aStructName;
  _Header = aHeader;
}

MessageSpec::~MessageSpec()
{
}

const QString& MessageSpec::getHeader() const
{
  return _Header;
}

void MessageSpec::SetHeader(const QString& header)
{
  _Header = header;
}

const QString& MessageSpec::getId() const
{
  return _Id;
}

void MessageSpec::SetId(const QString& id)
{
  _Id = id;
}

const QString& MessageSpec::getStructName() const
{
  return _StructName;
}

void MessageSpec::SetStructName(const QString& structName)
{
  _StructName = structName;
}

