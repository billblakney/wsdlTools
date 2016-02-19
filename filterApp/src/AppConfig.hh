#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

class AppConfig
{
public:
  AppConfig();
  AppConfig(QString aOpenFilterDir,QString aSaveFilterDir,QString aDefaultOperateMode);
  virtual ~AppConfig();

  const QString& getOpenFilterDir() const;

  void SetOpenFilterDir(const QString& id);

  const QString& getSaveFilterDir() const;

  void SetSaveFilterDir(const QString& structName);

  const QString& getDefaultOperateMode() const;

  void SetDefaultOperateMode(const QString& header);

  const QString& getDefaultDelimitMode() const;

  void SetDefaultDelimitMode(const QString& header);

  QString toQString();

protected:
  QString _OpenFilterDir;
  QString _SaveFilterDir;
  QString _DefaultOperateMode;
  QString _DefaultDelimitMode;
};

#endif /* APPCONFIG_H */
