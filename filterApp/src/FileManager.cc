#include "FileManager.hh"
//#include <QtWidgets>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include "DataStructModel.hh"

QXmlStreamReader FileManager::reader;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FileManager::FileManager()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FileManager::~FileManager()
{
}

//-----------------------------------------------------------------------------
// TODO is untested and may not work
//-----------------------------------------------------------------------------
void FileManager::skipUnknownElement()
{
std::cout << "SKIPPINGGGGGGGGGGGGGGGGGGGGGGGGG" << std::endl;
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
void FileManager::readFieldElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement()) // TODO try make else if
    {
      if (reader.name() == "field")
      {
        QString tString = reader.attributes().value("key").toString();
        std::cout << "key: " << qPrintable(tString) << std::endl;
        QString tString2 = reader.attributes().value("checkstate").toString();
        std::cout << "checkstate: " << qPrintable(tString2) << std::endl;
        QString tStr = reader.readElementText(); //TODO not used
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
      if (reader.name() == "operate_mode")
      {
        QString tStr = reader.readElementText();
        std::cout << "operate_mode: " << qPrintable(tStr) << std::endl;
      }
      else if (reader.name() == "delimit_mode")
      {
        QString tStr = reader.readElementText();
        std::cout << "delimit_mode: " << qPrintable(tStr) << std::endl;
      }
      else // TODO check to see if it works
      {
        skipUnknownElement();
      }
    }
    reader.readNext();
  }
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
      if (reader.name() == "config")
      {
        readConfigElements();
      }
      else if (reader.name() == "fields")
      {
        readFieldElements();
      }
      else
      {
        reader.raiseError(QObject::tr("expected config or fields"));
      }
    }
    reader.readNext();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FileManager::openConfiguration(DataStructModel *aModel)
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
      if (reader.name() == "wsdlfilter")
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

  std::cout << "done===============" << std::endl;
  file.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FileManager::saveConfiguration(DataStructModel *aModel)
{

  QString tFileName = QFileDialog::getOpenFileName(0,
      QString("Open WSDL Config File"), "/tmp", QString("WSDL Config Files (*.wcf *.wpf)"));

  std::cout << "selected " << tFileName.toStdString() << std::endl;


  QXmlStreamWriter xmlWriter;
  xmlWriter.setAutoFormatting(true);
  QFile file(tFileName);

  if (!file.open(QIODevice::WriteOnly))
  {
    QMessageBox::warning(0, "Error!", "Error opening file");
    return;
  }

  std::vector<FieldItem *> &tTreeItems = aModel->getTreeItems();

  xmlWriter.setDevice(&file);

  // Writes a document start with the XML version number.
  xmlWriter.writeStartDocument();

  xmlWriter.writeStartElement("wsdlfilter");

  xmlWriter.writeStartElement("config");

  xmlWriter.writeStartElement("operate_mode");
  xmlWriter.writeCharacters("go");
  xmlWriter.writeEndElement();

  xmlWriter.writeStartElement("delimit_mode");
  xmlWriter.writeCharacters("on_out");
  xmlWriter.writeEndElement();

  xmlWriter.writeEndElement(); // config

  xmlWriter.writeStartElement("fields");

  std::vector<FieldItem *>::iterator tIter;
  for (tIter = tTreeItems.begin(); tIter != tTreeItems.end(); tIter++)
  {
    FieldItemData &tData = (*tIter)->getData();

    // open field tag
    xmlWriter.writeStartElement("field");

    // write attributes
    xmlWriter.writeAttribute("key",tData.getKey().c_str());
    xmlWriter.writeAttribute("checkstate", (tData.getCheckState()?"1":"0"));

    // close tag field
    xmlWriter.writeEndElement();
  }

  // end tag fields
  xmlWriter.writeEndElement(); //fields

  // end tag wsdlfilter
  xmlWriter.writeEndElement(); //wsdlfilter

  // end document
  xmlWriter.writeEndDocument();

  file.close();
}
