#include "MessageSpec.hh"
//TODO args aX

MessageSpec::MessageSpec()
{
}

MessageSpec::MessageSpec(
    QString aId,QString aStructName,QString aHeader,QString aFilter)
{
  _Id = aId;
  _StructName = aStructName;
  _Header = aHeader;
  _Filter = aFilter;
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

const QString& MessageSpec::getFilter() const
{
  return _Filter;
}

void MessageSpec::SetFilter(const QString& filter)
{
  _Filter = filter;
}

QString MessageSpec::toQString()
{
  QString tStr;
  tStr += "id: " + _Id;
  tStr += ",struct: " + _StructName;
  tStr += ",header: " + _Header;
  tStr += ",filter: " + _Filter;
  return tStr;
}
