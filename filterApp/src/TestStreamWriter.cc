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
TestStreamWriter::TestStreamWriter()
{
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void TestStreamWriter::run()
{
  while (true)
  {
    std::cout << "writing to _OutStream..." << std::endl;
//    _TestStream << tMsg << std::endl;
    _TestStream << tMsg << std::endl;
    _TestStream << tMsg << std::endl;
    _TestStream << tMsg << std::endl;
    sleep(2);
  }
}
