#include <iostream>
#include <iterator>
#include <sstream>
#include <boost/regex.hpp>
#include "Structure.hh"
#include "StructorBuilder.hh"

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
Structure::Structure()
{
}


//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
Structure::Structure(std::string aName)
{
  _Name = aName;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
Structure::~Structure()
{
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void Structure::addFields(vector<Field> aFields)
{
  _Fields = aFields;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
void Structure::addField(Field aField)
{
  _Fields.push_back(aField);
}

//TODO deep enough? replace generically?
static std::string blanks[] = {
    "",
    "  ",
    "    ",
    "      ",
    "        ",
    "          ",
    "            ",
    "              ",
    "                ",
    "                  ",
};

//-------------------------------------------------------------------------------
// Prints a description of the data structure in a hierarchical fashion.
//-------------------------------------------------------------------------------
void Structure::postProcess(StructorBuilder *aStructBuilder,int aLevel)
//void Structure::postProcess(std::map<std::string,Structure *> &aStructMap)
{
  boost::regex array_size_regex("^([a-zA-Z]+)_size$");

  // If level is zero, this is the root node.
  // Print the root node name at indent level, and increment the level to
  // indent child fields.
  if (aLevel == 0)
  {
    std::cout << "root node: " << _Name << std::endl;
    aLevel++;
  }

  // Print all of the fields. For fields that are structs themselves,
  // this method will be called recursively.
  vector<Field>::iterator tIter;
  for (tIter = _Fields.begin(); tIter != _Fields.end(); tIter++)
  {
    boost::match_results<std::string::const_iterator> what;

    // For array size line,
    // print the array type and name.
    if (boost::regex_match(tIter->_Name,what,array_size_regex))
    {
      std::string tName = what[1];
      std::cout << blanks[aLevel];
      std::cout << "array size entry: "
          << tIter->_Type << ":" << tIter->_Name << std::endl;
    }
    else if (tIter->_IsPointer)
    {
      // For primitive pointer,
      // print type and name.
      if (aStructBuilder->isPrimitive(tIter->_Type))
      {
        std::cout << blanks[aLevel];
        std::cout << "primitive array node: "
            << tIter->_Type << ":" << tIter->_Name << std::endl;
      }
      // For struct pointer,
      // print type and name, increment level, and call this routine
      // recursively to print the contents of the struct.
      else
      {
        std::cout << blanks[aLevel];
        std::cout << "struct array node: "
            << tIter->_Type << ":" << tIter->_Name << std::endl;
        Structure *tStruct = aStructBuilder->getStructure(tIter->_Type);
        if (tStruct != NULL)
        {
          tStruct->postProcess(aStructBuilder,++aLevel);
          --aLevel;
        }
      }
    }
    // For struct field,
    // print type and name, then call this routine recursively to print the
    // contents of the struct.
    else if (!aStructBuilder->isPrimitive(tIter->_Type))
    {
      std::cout << blanks[aLevel];
      std::cout << "struct node: "
          << tIter->_Type << ":" << tIter->_Name << std::endl;
      Structure *tStruct = aStructBuilder->getStructure(tIter->_Type);
      if (tStruct != NULL)
      {
        tStruct->postProcess(aStructBuilder,++aLevel);
        --aLevel;
      }
      else
      {
        std::cout << blanks[aLevel];
        std::cout << "ERROR: can't find struct " << tIter->_Name << std::endl;
      }
    }
    // For primitive type field,
    // print field type and name at current indent level.
    else
    {
      std::cout << blanks[aLevel];
      std::cout << "primitive node: "
          << tIter->_Type << ":" << tIter->_Name << std::endl;
    }
  }
#if 0
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
#endif
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
std::string Structure::toString()
{
  stringstream tStream;

  tStream << _Name << std::endl;
  vector<Field>::iterator it;
  for( it = _Fields.begin(); it != _Fields.end(); it++ )
  {
    tStream << it->toString() << std::endl;
  }

  return tStream.str();
}
