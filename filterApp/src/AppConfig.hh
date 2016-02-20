#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

class AppConfig
{
public:
  AppConfig();
  AppConfig(QString aOpenFilterDir,QString aSaveFilterDir,QString aDefaultOperateMode);
  virtual ~AppConfig();

  const QString& getHeadersDir() const;

  void SetHeadersDir(const QString& id);

  const QString& getDefaultFiltersDir() const;

  void SetDefaultFiltersDir(const QString& id);

  const QString& getCustomFiltersDir() const;

  void SetCustomFiltersDir(const QString& structName);

  const QString& getDefaultOperateMode() const;

  void SetDefaultOperateMode(const QString& header);

  const QString& getDefaultDelimitMode() const;

  void SetDefaultDelimitMode(const QString& header);

  QString toQString();

protected:
  QString _HeadersDir;
  QString _DefaultFiltersDir;
  QString _CustomFiltersDir;
  QString _DefaultOperateMode;
  QString _DefaultDelimitMode;
};

#endif /* APPCONFIG_H */
