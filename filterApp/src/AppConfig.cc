#include "AppConfig.hh"
//TODO args aX

AppConfig::AppConfig()
{
}

AppConfig::AppConfig(QString aOpenFilterDir,QString aSaveFilterDir,QString aDefaultOperateMode)
{
  _OpenFilterDir = aOpenFilterDir;
  _SaveFilterDir = aSaveFilterDir;
  _DefaultOperateMode = aDefaultOperateMode;
}

AppConfig::~AppConfig()
{
}

const QString& AppConfig::getOpenFilterDir() const
{
  return _OpenFilterDir;
}

void AppConfig::SetOpenFilterDir(const QString& id)
{
  _OpenFilterDir = id;
}

const QString& AppConfig::getSaveFilterDir() const
{
  return _SaveFilterDir;
}

void AppConfig::SetSaveFilterDir(const QString& structName)
{
  _SaveFilterDir = structName;
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
  tStr += "open_filter_dir: " + _OpenFilterDir + "\n";
  tStr += "save_filter_dir: " + _SaveFilterDir + "\n";
  tStr += "default_operate_mode: " + _DefaultOperateMode + "\n";
  tStr += "default_delimit_mode: " + _DefaultDelimitMode;
  return tStr;
}

