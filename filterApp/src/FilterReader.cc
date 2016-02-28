#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include "DataStructModel.hh"
#include "FilterReader.hh"

const char *FilterReader::kWsdlfilterconfigTag = "wsdlfilter";
const char *FilterReader::kDefaultsTag = "config";
const char *FilterReader::kOperateModeTag = "operate_mode";
const char *FilterReader::kDelimitModeTag = "delimit_mode";
const char *FilterReader::kFilterDescriptionTag = "description";
const char *FilterReader::kFieldsTag = "fields";
const char *FilterReader::kFieldTag = "field";
const char *FilterReader::kAttrKeyTag = "key";
const char *FilterReader::kAttrCheckedTag = "checked";
const char *FilterReader::kAttrFilterScopeTag = "filter_scope";
const char *FilterReader::kAttrFormatTag = "format";
const char *FilterReader::kAttrPostfixTag = "postfix";
const char *FilterReader::kValueChecked = "1";
const char *FilterReader::kValueNotChecked = "0";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterReader::FilterReader()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterReader::~FilterReader()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterSpec FilterReader::openFilter(QString aDir)
{
  FilterSpec tFilterSpec;

  QString tFilepath = QFileDialog::getOpenFileName(0,
      QString("Open WSDL Config File"), aDir,
      QString("WSDL Config Files (*.wcf *.wpf)"));

  tFilterSpec = openFilter(QString(""),tFilepath);
  return tFilterSpec;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FilterSpec FilterReader::openFilter(QString aDir,QString aFilename)
{
  FilterSpec tFilterSpec;

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
    tFilterSpec._HasErrors = true;
    return tFilterSpec;
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
        readWsdlfilterElement(tFilterSpec);
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

  return tFilterSpec;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterReader::readWsdlfilterElement(FilterSpec &aFilterSpec)
{
  reader.readNext();

  while (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kDefaultsTag)
      {
        readConfigElements(aFilterSpec);
      }
      else if (reader.name() == kFieldsTag)
      {
        readFieldElements(aFilterSpec);
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
void FilterReader::readConfigElements(FilterSpec &aFilterSpec)
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kOperateModeTag)
      {
        QString tStr = reader.readElementText();
        aFilterSpec._OperateMode = tStr;
      }
      else if (reader.name() == kDelimitModeTag)
      {
        QString tStr = reader.readElementText();
        aFilterSpec._DelimitMode = tStr;
      }
      else if (reader.name() == kFilterDescriptionTag)
      {
        QString tStr = reader.readElementText();
        aFilterSpec._Description = tStr;
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
void FilterReader::readFieldElements(FilterSpec &aFilterSpec)
{
  reader.readNext();

  while (!reader.atEnd() && !reader.isEndElement())
  {
    if (reader.isStartElement())
    {
      if (reader.name() == kFieldTag)
      {
        QString tKey = reader.attributes().
            value(kAttrKeyTag).toString();
        QString tIsChecked = reader.attributes().
            value(kAttrCheckedTag).toString();
        QString tTestScope = reader.attributes().
            value(kAttrFilterScopeTag).toString();
        QString tFormat = reader.attributes().
            value(kAttrFormatTag).toString();
        QString tPostfix = reader.attributes().
            value(kAttrPostfixTag).toString();

        FieldSpec tFieldSpec;
        tFieldSpec.key = tKey;
        tFieldSpec.isChecked = tIsChecked;
        tFieldSpec.testScope = tTestScope;
        tFieldSpec.format = tFormat;
        tFieldSpec.postfix = tPostfix;
        aFilterSpec._Fields.push_back(tFieldSpec);

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
void FilterReader::skipUnknownElement()
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
void FilterReader::saveFilter(QString aDir,DataStructModel *aModel,
    StreamReader *aStreamReader)
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

  writeWsdlfilterDocument(xmlWriter,aModel,aStreamReader);

  file.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterReader::writeWsdlfilterDocument(QXmlStreamWriter &aWriter,
    DataStructModel *aModel,StreamReader *aStreamReader)
{
  // Writes a document start with the XML version number.
  aWriter.writeStartDocument();

  aWriter.writeStartElement(kWsdlfilterconfigTag);

  writeConfigElements(aWriter,aStreamReader);
  writeFieldElements(aWriter,aModel);

  // end tag wsdlfilter
  aWriter.writeEndElement(); //wsdlfilter

  // end document
  aWriter.writeEndDocument();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterReader::writeConfigElements(QXmlStreamWriter &aWriter,
    StreamReader *aStreamReader)
{
  aWriter.writeStartElement(kDefaultsTag);

  aWriter.writeStartElement(kOperateModeTag);
  aWriter.writeCharacters(
      StreamReader::getOperateModeString(aStreamReader->getOperateMode()));
  aWriter.writeEndElement();

  aWriter.writeStartElement(kDelimitModeTag);
  aWriter.writeCharacters(
      StreamReader::getDelimitModeString(aStreamReader->getDelimitMode()));
  aWriter.writeEndElement();

  aWriter.writeEndElement(); // config
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void FilterReader::writeFieldElements(
    QXmlStreamWriter &aWriter,DataStructModel *aModel)
{
  std::vector<FieldItem *> &tTreeItems = aModel->getTreeItems();

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
