#include "FileManager.hh"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include "DataStructModel.hh"

const char *FileManager::kWsdlfilterTag = "wsdlfilter";
const char *FileManager::kConfigTag = "config";
const char *FileManager::kOperateModeTag = "operate_mode";
const char *FileManager::kDelimitModeTag = "delimit_mode";
const char *FileManager::kFieldsTag = "fields";
const char *FileManager::kFieldTag = "field";
const char *FileManager::kAttrKeyTag = "key";
const char *FileManager::kAttrCheckedTag = "checked";
const char *FileManager::kValueChecked = "1";
const char *FileManager::kValueNotChecked = "0";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FileManager::FileManager(DataStructModel *aModel)
{
  _DataStructModel = aModel;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FileManager::~FileManager()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FileManager::openConfiguration()
{
  QString fileName = QFileDialog::getOpenFileName(0,
      QString("Open WSDL Config File"), "/tmp",
      QString("WSDL Config Files (*.wcf *.wpf)"));

  QFile file(fileName);
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    std::cout << "Error: Cannot read file " << qPrintable(fileName)
                        << ": " << qPrintable(file.errorString())
                        << std::endl;
    return;
  }
  reader.setDevice(&file);

  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kWsdlfilterTag)
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
void FileManager::readWsdlfilterElement()
{
  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kConfigTag)
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
void FileManager::readConfigElements()
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
void FileManager::readFieldElements()
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
void FileManager::skipUnknownElement()
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
void FileManager::saveConfiguration()
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
void FileManager::writeWsdlfilterDocument(QXmlStreamWriter &aWriter)
{
  // Writes a document start with the XML version number.
  aWriter.writeStartDocument();

  aWriter.writeStartElement(kWsdlfilterTag);

  writeConfigElements(aWriter);
  writeFieldElements(aWriter);

  // end tag wsdlfilter
  aWriter.writeEndElement(); //wsdlfilter

  // end document
  aWriter.writeEndDocument();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FileManager::writeConfigElements(QXmlStreamWriter &aWriter)
{
  aWriter.writeStartElement(kConfigTag);

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
void FileManager::writeFieldElements(QXmlStreamWriter &aWriter)
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
