#include "AppConfig.hh"
//TODO args aX

AppConfig::AppConfig()
{
}

AppConfig::~AppConfig()
{
}

const QString& AppConfig::getHeadersDir() const
{
  return _HeadersDir;
}

void AppConfig::SetHeadersDir(const QString& id)
{
  _HeadersDir = id;
}

const QString& AppConfig::getDefaultHeader() const
{
  return _DefaultHeader;
}

void AppConfig::SetDefaultHeader(const QString& id)
{
  _DefaultHeader = id;
}

const QString& AppConfig::getDefaultFiltersDir() const
{
  return _DefaultFiltersDir;
}

void AppConfig::SetDefaultFiltersDir(const QString& id)
{
  _DefaultFiltersDir = id;
}

const QString& AppConfig::getCustomFiltersDir() const
{
  return _CustomFiltersDir;
}

void AppConfig::SetCustomFiltersDir(const QString& structName)
{
  _CustomFiltersDir = structName;
}

const QString& AppConfig::getDefaultOperateMode() const
{
  return _DefaultOperateMode;
}

void AppConfig::SetDefaultOperateMode(const QString& header)
{
  _DefaultOperateMode = header;
}

const QString& AppConfig::getDefaultDelimitMode() const
{
  return _DefaultDelimitMode;
}

void AppConfig::SetDefaultDelimitMode(const QString& header)
{
  _DefaultDelimitMode = header;
}

QString AppConfig::toQString()
{
  QString tStr;
  tStr += "default_filters_dir: " + _DefaultFiltersDir + "\n";
  tStr += "custom_filters_dir: " + _CustomFiltersDir + "\n";
  tStr += "default_operate_mode: " + _DefaultOperateMode + "\n";
  tStr += "default_delimit_mode: " + _DefaultDelimitMode;
  return tStr;
}

