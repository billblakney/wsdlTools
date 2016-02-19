#include <iostream>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include "AppConfigFile.hh"

const char *AppConfigFile::kWsdlFilterConfigTag = "wsdlfilterconfig";
const char *AppConfigFile::kDefaultsTag = "defaults";
const char *AppConfigFile::kDefaultOperateModeTag = "default_operate_mode";
const char *AppConfigFile::kDefaultDelimitModeTag = "default_delimit_mode";
const char *AppConfigFile::kMessagesTag = "messages";
const char *AppConfigFile::kMessageTag = "message";
const char *AppConfigFile::kAttrIdTag = "id";
const char *AppConfigFile::kAttrStructTag = "struct";
const char *AppConfigFile::kAttrHeaderTag = "header";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AppConfigFile::AppConfigFile(QString aConfigFilename)
{
  _ConfigFilename = aConfigFilename;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AppConfigFile::~AppConfigFile()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigFile::openConfiguration()
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

//  std::cout << "done===============" << std::endl;
  file.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigFile::readWsdlFilterConfigElement()
{
  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kDefaultsTag)
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
//-----------------------------------------------------------------------------
void AppConfigFile::readDefaultsElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kDefaultOperateModeTag)
      {
        QString tStr = reader.readElementText();
        std::cout << "default_operate_mode: " << qPrintable(tStr) << std::endl;
      }
      else if (reader.name() == kDefaultDelimitModeTag)
      {
        QString tStr = reader.readElementText();
        std::cout << "default_delimit_mode: " << qPrintable(tStr) << std::endl;
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
void AppConfigFile::readMessagesElements()
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

        MessageSpec tSpec(tId,tStruct,tHeader);
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigFile::skipUnknownElement()
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
std::map<QString,MessageSpec> &AppConfigFile::messageMap()
{
  return _MessageMap;
}

#if 0
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void AppConfigFile::saveConfiguration()
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
void AppConfigFile::writeWsdlfilterDocument(QXmlStreamWriter &aWriter)
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
void AppConfigFile::writeConfigElements(QXmlStreamWriter &aWriter)
{
  aWriter.writeStartElement(kDefaultsTag);

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
void AppConfigFile::writeMessageElements(QXmlStreamWriter &aWriter)
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
