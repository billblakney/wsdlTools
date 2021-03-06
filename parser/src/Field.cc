#include "Field.hh"

#include <sstream>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Field::Field()
{
	_IsPointer = false;;
	_Type = "";
	_Name = "";
	_Level = eUnknown;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Field::Field(string aType, string aName, int aIsPointer)
{
	_Type = aType;
	_Name = aName;
	_IsPointer = aIsPointer ? true : false;
	_Level = eUnknown;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Field::~Field()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
string Field::toString()
{
	stringstream str;
	str << "type: " << _Type << ", "
		<< "name: " << _Name << ", "
		<< "isPointer: " << _IsPointer;
	return str.str();
}
