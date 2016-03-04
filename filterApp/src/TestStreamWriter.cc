#include <iostream>
#include <string>
#include "TestStreamWriter.hh"

const char *tMsg =
"==============RECEIVED MESSAGE=========\n"
"some junk here\n"
"some junk ssifg_STUDENT_MSG_ID more stuff STUDENT_T\n"
"some junk there\n"
"some junk here\n"
"some junk there\n"
"structsdfd\n"
"firstName:	Benjamin\n"
"lastName:	AE_MODE_FM\n"
"address:\n"
"	city:	Reston\n"
"	state:	VA\n"
"	zipcode:	20171\n"
"grades:\n"
"	array of len: 2\n"
"	subject:	math\n"
"	grade:	A\n"
"	course:\n"
"		code:	550\n"
"		level:	grad\n"
"	scores:\n"
"		array of len: 3\n"
"		[0] =	89\n"
"		[1] =	95\n"
"		[2] =	100\n"
"	subject:	french\n"
"	grade:	B\n"
"	course:\n"
"		code:	415\n"
"		level:	bach\n"
"	scores:\n"
"		array of len: 2\n"
"		[0] =	89\n"
"		[1] =	82\n"
"school:	Oakton\n"
"==============END MESSAGE=========\n";

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TestStreamWriter::TestStreamWriter(std::string tRecord)
{
//    std::cout << "start writing to _OutStream..." << "tellp,tellg: " << _TestStream.tellp() << "," << _TestStream.tellg() << std::endl;
//    _TestStream << tMsg;
    _TestStream << tRecord;
//    std::cout << "done writing to _OutStream..." << "tellp,tellg: " << _TestStream.tellp() << "," << _TestStream.tellg() << std::endl;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TestStreamWriter::~TestStreamWriter()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
std::stringstream &TestStreamWriter::getTestStream()
{
  return _TestStream;
}
