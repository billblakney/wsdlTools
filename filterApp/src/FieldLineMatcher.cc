#include <boost/regex.hpp>
#include "FieldLineMatcher.hh"

/*
 * Looks for matches to strings with either of the following two formats:
 * "field: value" or "field:"
 * Input with leading or trailing space will match. But there is one subtelty
 * in this regard: "field:" the match will not contain any leading or trailing
 * white space, but, "value" may contain trailing white space. Ideally we'd
 * like to capture a value without any trailing whitespace (even though we
 * want to allow white space within the value). Maybe with some extra thought
 * the regular expression used could be modified to do that. (But we want
 * to do that with only one match test.)
 */

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FieldLineMatcher::FieldLineMatcher()
  : _FieldName(""),
    _FieldValue(""),
   _FieldRegex("\\s*(\\w+):([\\s]*)(.*)")
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FieldLineMatcher::~FieldLineMatcher()
{
}

//-----------------------------------------------------------------------------
// Check if a string matches a field line.
// Returns true on a match, false otherwise.
//-----------------------------------------------------------------------------
bool FieldLineMatcher::match(std::string &aStr)
{
  bool tReturn = false;

  _FieldName = "";
  _FieldValue = "";

  boost::match_results<std::string::const_iterator> what;

  if (boost::regex_match(aStr,what,_FieldRegex))
  {
    int tWhatSize = what.size();
    int tFieldSize = what[1].length();
    int tSpaceSize = what[2].length();
    int tValueSize = what[3].length();
    if (tFieldSize && !tSpaceSize && !tValueSize)
    {
      _FieldName = what[1];
      tReturn = true;
    }
    else if (tFieldSize && tSpaceSize && tValueSize)
    {
      _FieldName = what[1];
      _FieldValue = what[3];
      tReturn = true;
    }
    else
    {
      tReturn = false;
    }
  }
  return tReturn;
}

//-----------------------------------------------------------------------------
// Check if a call to getFieldName() would return an empty string.
//-----------------------------------------------------------------------------
bool FieldLineMatcher::hasFieldName()
{
  return ((_FieldName.length() > 0) ? true : false);
}

//-----------------------------------------------------------------------------
// Check if a call to getFieldValue() would return an empty string.
//-----------------------------------------------------------------------------
bool FieldLineMatcher::hasFieldValue()
{
  return ((_FieldValue.length() > 0) ? true : false);
  (_FieldValue.length() > 0) ? true : false;
}

//-----------------------------------------------------------------------------
// Get the field value from the last call to match(). Will be an empty string
// if last call to match returned false. Otherwise, should contain the parsed
// field name.
//-----------------------------------------------------------------------------
std::string FieldLineMatcher::getFieldName()
{
  return _FieldName;
}

//-----------------------------------------------------------------------------
// Get the field value from the last call to match(). Will be an empty string
// if last call to match returned false, or the last call to match was on a
// line that didn't have a field value.
//-----------------------------------------------------------------------------
std::string FieldLineMatcher::getFieldValue()
{
  return _FieldValue;
}
