#include "StructorBuilder.hh"

#ifndef C11
#include <iterator>
#include <stdio.h>
#endif
#include <iostream>
#include <sstream>

StructorBuilder::StructorBuilder() {
}

StructorBuilder::~StructorBuilder() {
}

void StructorBuilder::pushField(Field field) {
	_Fields.push_back(field);
}

vector<Field> StructorBuilder::pullFields()
{
  return _Fields;
}

void StructorBuilder::clearFields() {
	_Fields.clear();
}

void StructorBuilder::printFields() {
	cout << "FIELDS:" << endl;
#ifdef C11
	for( auto field: _Fields ){
#else
	vector<Field>::iterator tIter;
	for( tIter = _Fields.begin(); tIter != _Fields.end(); tIter++ ){
		Field field = *tIter;
#endif
		cout << "field: " << field.toString() << endl;
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructorBuilder::onMatch(const char *aRuleName)
{
   static int _debug = 0;
   if( _debug )
      cout << "matched: " << aRuleName << endl;;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructorBuilder::onEnumValue(std::string *aValue)
{
//   static int _debug = 0;
std::cout << "enum value: " << *aValue << std::endl;
   _EnumValues.push_back(*aValue);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructorBuilder::onField(string *aName,string *aType,int aIsPointer)
{
//   static int _debug = 0;
   Field tField(*aName,*aType,aIsPointer);
   pushField(tField);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructorBuilder::onEnum(std::string *aName)
{
   static int _debug = 1;

   Enum *tEnum = new Enum(*aName);

   tEnum->addValues(_EnumValues);
   _EnumValues.clear();

   if( _debug )
      printf("enum: %s\n",tEnum->toString().c_str());

   _Enums[*aName] = tEnum;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructorBuilder::onStruct(std::string *aStructName)
{
   static int _debug = 0;

   Structure *tStructure = new Structure(*aStructName);

   tStructure->addFields(pullFields());
   clearFields();

   if( _debug )
      printf("struct: %s\n",tStructure->toString().c_str());

   _Structs[*aStructName] = tStructure;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool StructorBuilder::isPrimitive(std::string aType)
{
  map<std::string,Structure *>::iterator tStructIter;
  tStructIter = _Structs.find(aType);

  if( tStructIter == _Structs.end() )
  {
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::vector<std::string> StructorBuilder::getStructNames()
{
  std::vector<std::string> tNames;

  map<std::string,Structure *>::iterator tIt;
  for (tIt = _Structs.begin(); tIt != _Structs.end(); tIt++)
  {
    tNames.push_back(tIt->first);
  }

  return tNames;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Structure *StructorBuilder::getStructure(std::string aType)
{
  map<std::string,Structure *>::iterator tStructIter;
  tStructIter = _Structs.find(aType);

  if( tStructIter == _Structs.end() )
  {
    return NULL;
  }

  return tStructIter->second;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::string StructorBuilder::getDotString(std::string aName,std::string aPrefix)
{
  stringstream tReturn;

  map<std::string,Structure *>::iterator tStructIter;
  tStructIter = _Structs.find(aName);

  if( tStructIter != _Structs.end() )
  {
    Structure *tStructure = tStructIter->second;
    vector<Field>::iterator it;
    for( it = tStructure->_Fields.begin(); it != tStructure->_Fields.end(); it++)
    {
      std::string tName = it->_Name;
      std::string tType = it->_Type;
      std::string tFieldString = aPrefix + "." + tName;
      std::string tFieldDotString = getDotString(tType,tFieldString);
      if( tFieldDotString.length() )
      {
        tReturn << tFieldDotString;
      }
      else
      {
        tReturn << tFieldString << endl;
      }
    }
  }

  return tReturn.str();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructorBuilder::postProcess()
{
  std::string tCarReportType("CONTACT_ATTRIBUTES_REPORT_T");

  map<std::string,Structure *>::iterator it;
  it = _Structs.find(tCarReportType);
  if( it == _Structs.end() )
  {
    cout << "couldn't find " << tCarReportType << endl;
  }
  else
  {
    cout << "FOUND " << tCarReportType << endl;
    it->second->postProcess(this);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StructorBuilder::printSummary()
{
//   static int _debug = 1;
   map<std::string,Structure *>::iterator it;
#if 0
   for( it = _Structs.begin() ; it != _Structs.end(); it++ ){
   	  Structure *tStruct = it->second;
      printf("struct: %s\n",tStruct->toString().c_str());
   }
#endif
   for( it = _Structs.begin() ; it != _Structs.end(); it++ ){
   	  Structure *tStruct = it->second;
      cout << "struct: " << tStruct->_Name << endl;
      cout << getDotString(tStruct->_Name,"aStruct") << endl;
   }
}
