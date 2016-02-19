#include "MessageConfigFile.hh"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include "DataStructModel.hh"

const char *MessageConfigFile::kWsdlfilterconfigTag = "wsdlfilter";
const char *MessageConfigFile::kDefaultsTag = "config";
const char *MessageConfigFile::kOperateModeTag = "operate_mode";
const char *MessageConfigFile::kDelimitModeTag = "delimit_mode";
const char *MessageConfigFile::kFieldsTag = "fields";
const char *MessageConfigFile::kFieldTag = "field";
const char *MessageConfigFile::kAttrKeyTag = "key";
const char *MessageConfigFile::kAttrCheckedTag = "checked";
const char *MessageConfigFile::kValueChecked = "1";
const char *MessageConfigFile::kValueNotChecked = "0";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MessageConfigFile::MessageConfigFile(DataStructModel *aModel)
{
  _DataStructModel = aModel;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MessageConfigFile::~MessageConfigFile()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageConfigFile::openConfiguration(QString aDir)
{
  QString fileName = QFileDialog::getOpenFileName(0,
      QString("Open WSDL Config File"), aDir,
      QString("WSDL Config Files (*.wcf *.wpf)"));

  openConfiguration(aDir,fileName);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageConfigFile::openConfiguration(QString aDir,QString aFilename)
{
  QString aFilepath = aDir + "/" + aFilename;
  QFile file(aFilepath);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    std::cout << "Error: Cannot read file " << qPrintable(aFilename)
              << ": " << qPrintable(file.errorString()) << std::endl;
    return;
  }

  std::cout << "applying filter " << qPrintable(aFilepath) << std::endl;
  reader.setDevice(&file);

  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kWsdlfilterconfigTag)
      {
        readWsdlfilterElement();
      }
      else
      {
        reader.raiseError(QObject::tr("Not a WSDL filter config file"));
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
void MessageConfigFile::readWsdlfilterElement()
{
  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kDefaultsTag)
      {
        readConfigElements();
      }
      else if (reader.name() == kFieldsTag)
      {
        readFieldElements();
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
void MessageConfigFile::readConfigElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kOperateModeTag)
      {
        QString tStr = reader.readElementText();
//        std::cout << "operate_mode: " << qPrintable(tStr) << std::endl;
      }
      else if (reader.name() == kDelimitModeTag)
      {
        QString tStr = reader.readElementText();
//        std::cout << "delimit_mode: " << qPrintable(tStr) << std::endl;
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
void MessageConfigFile::readFieldElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kFieldTag)
      {
        QString tKey = reader.attributes().value(kAttrKeyTag).toString();
//        std::cout << "key: " << qPrintable(tKey) << std::endl;
        QString tCheckValue = reader.attributes().value(kAttrCheckedTag).toString();
//        std::cout << "checked: " << qPrintable(tCheckValue) << std::endl;

        bool tChecked = (tCheckValue.toStdString().compare(kValueChecked)?false:true);

        FieldItem *tFieldItem = _DataStructModel->getNode(tKey.toStdString());
        if (tFieldItem)
        {
          FieldItemData &tData = tFieldItem->getData();
          bool tCurrentState = tData.isChecked();
          if (tChecked != tCurrentState)
          {
            if (tChecked)
            {
              tData.setCheckState(Qt::Checked);
            }
            else
            {
              tData.setCheckState(Qt::Unchecked);
            }
          }
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageConfigFile::skipUnknownElement()
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
void MessageConfigFile::saveConfiguration(QString aDir)
{

  QString tFileName = QFileDialog::getOpenFileName(0,
      QString("Open WSDL Config File"), aDir, QString("WSDL Config Files (*.wcf *.wpf)"));

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
void MessageConfigFile::writeWsdlfilterDocument(QXmlStreamWriter &aWriter)
{
  // Writes a document start with the XML version number.
  aWriter.writeStartDocument();

  aWriter.writeStartElement(kWsdlfilterconfigTag);

  writeConfigElements(aWriter);
  writeFieldElements(aWriter);

  // end tag wsdlfilter
  aWriter.writeEndElement(); //wsdlfilter

  // end document
  aWriter.writeEndDocument();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageConfigFile::writeConfigElements(QXmlStreamWriter &aWriter)
{
  aWriter.writeStartElement(kDefaultsTag);

  aWriter.writeStartElement(kOperateModeTag);
  aWriter.writeCharacters("go");
  aWriter.writeEndElement();

  aWriter.writeStartElement(kDelimitModeTag);
  aWriter.writeCharacters("on_out");
  aWriter.writeEndElement();

  aWriter.writeEndElement(); // config
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageConfigFile::writeFieldElements(QXmlStreamWriter &aWriter)
{
  std::vector<FieldItem *> &tTreeItems = _DataStructModel->getTreeItems();

  aWriter.writeStartElement(kFieldsTag);

  std::vector<FieldItem *>::iterator tIter;
  for (tIter = tTreeItems.begin(); tIter != tTreeItems.end(); tIter++)
  {
    FieldItemData &tData = (*tIter)->getData();

    // open field tag
    aWriter.writeStartElement(kFieldTag);

    // write attributes
    aWriter.writeAttribute(kAttrKeyTag,tData.getKey().c_str());
    aWriter.writeAttribute(kAttrCheckedTag, (tData.getCheckState()?kValueChecked:kValueNotChecked));

    // close tag field
    aWriter.writeEndElement();
  }
  // end tag fields
  aWriter.writeEndElement(); //fields
}
