#ifndef AppConfig_hh
#define AppConfig_hh

#include <QString>

class AppConfig
{
public:
  AppConfig();
  virtual ~AppConfig();

  const QString& getHeadersDir() const;

  void SetHeadersDir(const QString& id);

  const QString& getDefaultHeader() const;

  void SetDefaultHeader(const QString& id);

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
  QString _DefaultHeader;
  QString _DefaultFiltersDir;
  QString _CustomFiltersDir;
  QString _DefaultOperateMode;
  QString _DefaultDelimitMode;
};

#endif /* AppConfig_hh */
