#include "MessageSpecReader.hh"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include "DataStructModel.hh"

const char *MessageSpecReader::kWsdlfilterconfigTag = "wsdlfilter";
const char *MessageSpecReader::kDefaultsTag = "config";
const char *MessageSpecReader::kOperateModeTag = "operate_mode";
const char *MessageSpecReader::kDelimitModeTag = "delimit_mode";
const char *MessageSpecReader::kFieldsTag = "fields";
const char *MessageSpecReader::kFieldTag = "field";
const char *MessageSpecReader::kAttrKeyTag = "key";
const char *MessageSpecReader::kAttrCheckedTag = "checked";
const char *MessageSpecReader::kAttrFilterScopeTag = "filter_scope";
const char *MessageSpecReader::kAttrFormatTag = "format";
const char *MessageSpecReader::kAttrPostfixTag = "postfix";
const char *MessageSpecReader::kValueChecked = "1";
const char *MessageSpecReader::kValueNotChecked = "0";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MessageSpecReader::MessageSpecReader(DataStructModel *aModel)
{
  _DataStructModel = aModel;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MessageSpecReader::~MessageSpecReader()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageSpecReader::openConfiguration(QString aDir)
{
  QString tFilepath = QFileDialog::getOpenFileName(0,
      QString("Open WSDL Config File"), aDir,
      QString("WSDL Config Files (*.wcf *.wpf)"));

  openConfiguration(QString(""),tFilepath);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageSpecReader::openConfiguration(QString aDir,QString aFilename)
{
  QString aFilepath;
  if (aDir.length())
  {
    aFilepath = aDir + "/" + aFilename;
  }
  else
  {
    aFilepath = aFilename;
  }
  QFile file(aFilepath);
  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    std::cout << "Error: Cannot read file " << qPrintable(aFilepath)
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
void MessageSpecReader::readWsdlfilterElement()
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
void MessageSpecReader::readConfigElements()
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
void MessageSpecReader::readFieldElements()
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kFieldTag)
      {
        QString tKey = reader.attributes().value(kAttrKeyTag).toString();
        QString tIsChecked = reader.attributes().value(kAttrCheckedTag).toString();
        QString tTestScope = reader.attributes().value(kAttrFilterScopeTag).toString();
        QString tFormat = reader.attributes().value(kAttrFormatTag).toString();
        QString tPostfix = reader.attributes().value(kAttrPostfixTag).toString();

        FieldItem *tFieldItem = _DataStructModel->getNode(tKey.toStdString());

        if (tFieldItem)
        {
          updateMessageIsChecked(tFieldItem,tIsChecked);
          updateMessageTestScope(tFieldItem,tTestScope);
          updateMessageFormat(tFieldItem,tFormat);
          updateMessagePostfix(tFieldItem,tPostfix);
        }
        else
        {
          std::cout << "WARNING: couldn't find tree node for key "
              << qPrintable(tKey) << std::endl;
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
void MessageSpecReader::updateMessageIsChecked(FieldItem *aItem,
    QString aIsChecked)
{
  bool tNewIsCheckedValue =
      (aIsChecked.toStdString().compare(kValueChecked)?false:true);
  bool tOldIsCheckedValue = aItem->getData().isChecked();

  if (tNewIsCheckedValue != tOldIsCheckedValue)
  {
    if (tNewIsCheckedValue)
    {
      aItem->setCheckState(Qt::Checked);
    }
    else
    {
      aItem->setCheckState(Qt::Unchecked);
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageSpecReader::updateMessageTestScope(FieldItem *aItem,
      QString aNewTestScope)
{
  QString tOldTestScope(aItem->getData().getTestScope().c_str());

  if (tOldTestScope != aNewTestScope)
  {
    aItem->setTestScope(aNewTestScope.toStdString().c_str());
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageSpecReader::updateMessageFormat(FieldItem *aItem,
    QString aNewFormat)
{
  FieldItemData::Format tOldFormatValue = aItem->getData().getFormat();
  QString tOldFormat = aItem->getData().getFormatString(tOldFormatValue);
  if (tOldFormat != aNewFormat)
  {
    FieldItemData::Format tNewFormatValue = FieldItemData::getFormat(aNewFormat);
    aItem->setFieldFormat(tNewFormatValue);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageSpecReader::updateMessagePostfix(FieldItem *aItem,
    QString aNewPostfix)
{
  QString tOldPostfix(aItem->getData().getPostfix().c_str());

  if (tOldPostfix != aNewPostfix)
  {
    aItem->setFieldPostfix(aNewPostfix.toStdString().c_str());
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MessageSpecReader::skipUnknownElement()
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
void MessageSpecReader::saveConfiguration(QString aDir)
{

  QString tFileName = QFileDialog::getSaveFileName(0,
      QString("Open WSDL Config File"), aDir,
      QString("WSDL Config Files (*.wcf *.wpf)"));

  std::cout << "saving filter settings to " << tFileName.toStdString()
      << std::endl;


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
void MessageSpecReader::writeWsdlfilterDocument(QXmlStreamWriter &aWriter)
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
void MessageSpecReader::writeConfigElements(QXmlStreamWriter &aWriter)
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
void MessageSpecReader::writeFieldElements(QXmlStreamWriter &aWriter)
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
    QString tKey = tData.getKey().c_str();
    QString tChecked = (tData.getCheckState()?kValueChecked:kValueNotChecked);
    QString tTestScope = tData.getTestScope().c_str();
    QString tFormat = FieldItemData::getFormatString(tData.getFormat());
    QString tPostfix = tData.getPostfix().c_str();

    aWriter.writeAttribute(kAttrKeyTag,tKey);
    aWriter.writeAttribute(kAttrCheckedTag,tChecked);
    aWriter.writeAttribute(kAttrFilterScopeTag,tTestScope);
    aWriter.writeAttribute(kAttrFormatTag,tFormat);
    aWriter.writeAttribute(kAttrPostfixTag,tPostfix);

    // close tag field
    aWriter.writeEndElement();
  }
  // end tag fields
  aWriter.writeEndElement(); //fields
}
