#include <cstdlib>
#include <iostream>
#include <string>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>

#include "AppConfigReader.hh"

const char *AppConfigReader::kWsdlFilterConfigTag = "wsdlfilterconfig";
const char *AppConfigReader::kConfigTag = "config";
const char *AppConfigReader::kHeadersDirTag = "headers_dir";
const char *AppConfigReader::kDefaultHeaderTag = "default_header";
const char *AppConfigReader::kDefaultFiltersDirTag = "default_filters_dir";
const char *AppConfigReader::kCustomFiltersDirTag = "custom_filters_dir";
const char *AppConfigReader::kDefaultOperateModeTag = "default_operate_mode";
const char *AppConfigReader::kDefaultDelimitModeTag = "default_delimit_mode";
const char *AppConfigReader::kMessagesTag = "messages";
const char *AppConfigReader::kMessageTag = "message";
const char *AppConfigReader::kAttrIdTag = "id";
const char *AppConfigReader::kAttrStructTag = "struct";
const char *AppConfigReader::kAttrHeaderTag = "header";
const char *AppConfigReader::kAttrDefaultFilterTag = "default_filter";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AppConfigReader::AppConfigReader(QString aConfigFilename)
{
  _ConfigFilename = aConfigFilename;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AppConfigReader::~AppConfigReader()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QString AppConfigReader::expandEnvVars(QString aStr)
{
  std::string tStr = aStr.toStdString();
  std::string tNewStr = expandEnvVars(tStr);
  return tNewStr.c_str();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::string AppConfigReader::expandEnvVars(std::string aStr)
{
  if( aStr.find( "${" ) == std::string::npos ) return aStr;

  std::string pre  = aStr.substr( 0, aStr.find( "${" ) );
  std::string post = aStr.substr( aStr.find( "${" ) + 2 );

  if( post.find( '}' ) == std::string::npos ) return aStr;

  std::string variable = post.substr( 0, post.find( '}' ) );
  std::string value    = "";

  post = post.substr( post.find( '}' ) + 1 );

  if( getenv( variable.c_str() ) != NULL ) value = std::string( getenv( variable.c_str() ) );

  return expandEnvVars(pre + value + post);
}

//-----------------------------------------------------------------------------
// Open an application configuration file and process its contents.
//-----------------------------------------------------------------------------
void AppConfigReader::openConfiguration()
{
  QFile file(_ConfigFilename);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    std::cout << "Error: Cannot read file " << qPrintable(_ConfigFilename)
              << ": " << qPrintable(file.errorString()) << std::endl;
    return;
  }

  reader.setDevice(&file);
  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kWsdlFilterConfigTag)
      {
        readWsdlFilterConfigElement();
      }
      else
      {
        reader.raiseError(QObject::tr("Not a WSDL filter app config file"));
      }
    }
    else
    {
      reader.readNext();
    }
  }

  file.close();
}

//-----------------------------------------------------------------------------
// Looks for the top-level elements of the app config file.
//-----------------------------------------------------------------------------
void AppConfigReader::readWsdlFilterConfigElement()
{
  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kConfigTag)
      {
        readDefaultsElements();
      }
      else if (reader.name() == kMessagesTag)
      {
        readMessagesElements();
      }
      else
      {
        skipUnknownElement();
      }
    }
    reader.readNext();
  }
}

//-----------------------------------------------------------------------------
// Reads the defaults section of the app config file.
//-----------------------------------------------------------------------------
void AppConfigReader::readDefaultsElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kHeadersDirTag)
      {
        QString tStr = expandEnvVars(reader.readElementText());
        _AppConfig.SetHeadersDir(tStr);
      }
      else if (reader.name() == kDefaultHeaderTag)
      {
        QString tStr = expandEnvVars(reader.readElementText());
        _AppConfig.SetDefaultHeader(tStr);
      }
      else if (reader.name() == kDefaultFiltersDirTag)
      {
        QString tStr = expandEnvVars(reader.readElementText());
        _AppConfig.SetDefaultFiltersDir(tStr);
      }
      else if (reader.name() == kCustomFiltersDirTag)
      {
        QString tStr = expandEnvVars(reader.readElementText());
        _AppConfig.SetCustomFiltersDir(tStr);
      }
      else if (reader.name() == kDefaultOperateModeTag)
      {
        QString tStr = reader.readElementText();
        _AppConfig.SetDefaultOperateMode(tStr);
      }
      else if (reader.name() == kDefaultDelimitModeTag)
      {
        QString tStr = reader.readElementText();
        _AppConfig.SetDefaultDelimitMode(tStr);
      }
      else
      {
        skipUnknownElement();
      }
    }
    reader.readNext();
  }
}

//-----------------------------------------------------------------------------
// Reads the messages section of the app config file.
//-----------------------------------------------------------------------------
void AppConfigReader::readMessagesElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kMessageTag)
      {
        MessageSpec tSpec;
        readMessageElements(tSpec);
        if (tSpec.getId().length()!=0)
        {
          _MessageMap[tSpec.getId()] = tSpec;
        }
        else
        {
          std::cout << "WARNING: app config file specifies message without id"
              << std::endl;
        }

        reader.readElementText();
      }
      else
      {
        skipUnknownElement();
      }
    }
    reader.readNext();
  }
}
#if 0 // old format using attributes instead of sub-elements
void AppConfigReader::readMessagesElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kMessageTag)
      {
        QString tId = reader.attributes().value(kAttrIdTag).toString();
        QString tStruct = reader.attributes().value(kAttrStructTag).toString();
        QString tHeader = reader.attributes().value(kAttrHeaderTag).toString();
        QString tFilter = reader.attributes().value(kAttrDefaultFilterTag).toString();

        MessageSpec tSpec(tId,tStruct,tHeader,tFilter);
        _MessageMap[tId] = tSpec;

        reader.readElementText();
      }
      else
      {
        skipUnknownElement();
      }
    }
    reader.readNext();
  }
}
#endif

//-----------------------------------------------------------------------------
// Reads the message section of the app config file.
//-----------------------------------------------------------------------------
void AppConfigReader::readMessageElements(MessageSpec &aMessageSpec)
{
  reader.readNext();

  /*
   * Set the header of the message spec to the default header value.
   * Then let any value read from the message spec supercede it, if there.
   */
  aMessageSpec.SetHeader(_AppConfig.getDefaultHeader());

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kAttrIdTag)
      {
        QString tStr = reader.readElementText();
        aMessageSpec.SetId(tStr);
      }
      else if (reader.name() == kAttrStructTag)
      {
        QString tStr = reader.readElementText();
        aMessageSpec.SetStructName(tStr);
      }
      else if (reader.name() == kAttrHeaderTag)
      {
        QString tStr = reader.readElementText();
        aMessageSpec.SetHeader(tStr);
      }
      else if (reader.name() == kAttrDefaultFilterTag)
      {
        QString tStr = reader.readElementText();
        aMessageSpec.SetFilter(tStr);
      }
      else if (reader.isComment())
      {
        // do nothing
      }
      else
      {
        skipUnknownElement();
      }
    }
    reader.readNext();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigReader::skipUnknownElement()
{
  std::cout << "WARN: skipping unknown xml element" << std::endl;

  reader.readNext();

  while (!reader.atEnd())
  {
    reader.readNext();

    if (reader.isEndElement())
    {
      return;
    }

    if (reader.isStartElement())
    {
      skipUnknownElement();
    }

    reader.readNext();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::map<QString,MessageSpec> &AppConfigReader::messageMap()
{
  return _MessageMap;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AppConfig &AppConfigReader::appConfig()
{
  return _AppConfig;
}

#if 0
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigReader::saveConfiguration()
{

  QString tFileName = QFileDialog::getOpenFileName(0,
      QString("Open WSDL Config File"), "/tmp", QString("WSDL Config Files (*.wcf *.wpf)"));

//  std::cout << "selected " << tFileName.toStdString() << std::endl;


  QXmlStreamWriter xmlWriter;
  xmlWriter.setAutoFormatting(true);
  QFile file(tFileName);

  if (!file.open(QIODevice::WriteOnly))
  {
    QMessageBox::warning(0, "Error!", "Error opening file");
    return;
  }

  xmlWriter.setDevice(&file);

  writeWsdlfilterDocument(xmlWriter);

  file.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigReader::writeWsdlfilterDocument(QXmlStreamWriter &aWriter)
{
  // Writes a document start with the XML version number.
  aWriter.writeStartDocument();

  aWriter.writeStartElement(kWsdlFilterConfigTag);

  writeConfigElements(aWriter);
  writeMessageElements(aWriter);

  // end tag wsdlfilter
  aWriter.writeEndElement(); //wsdlfilter

  // end document
  aWriter.writeEndDocument();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigReader::writeConfigElements(QXmlStreamWriter &aWriter)
{
  aWriter.writeStartElement(kConfigTag);

  aWriter.writeStartElement(kDefaultOperateModeTag);
  aWriter.writeCharacters("go");
  aWriter.writeEndElement();

  aWriter.writeStartElement(kDefaultDelimitModeTag);
  aWriter.writeCharacters("on_out");
  aWriter.writeEndElement();

  aWriter.writeEndElement(); // config
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigReader::writeMessageElements(QXmlStreamWriter &aWriter)
{
  std::vector<MessageItem *> &tTreeItems = _DataStructModel->getTreeItems();

  aWriter.writeStartElement(kMessagesTag);

  std::vector<MessageItem *>::iterator tIter;
  for (tIter = tTreeItems.begin(); tIter != tTreeItems.end(); tIter++)
  {
    MessageItemData &tData = (*tIter)->getData();

    // open message tag
    aWriter.writeStartElement(kMessageTag);

    // write attributes
    aWriter.writeAttribute(kAttrIdTag,tData.getId().c_str());
    aWriter.writeAttribute(kAttrStructTag, (tData.getCheckState()?kValueStruct:kValueNotStruct));

    // close tag message
    aWriter.writeEndElement();
  }
  // end tag messages
  aWriter.writeEndElement(); //messages
}
#endif
