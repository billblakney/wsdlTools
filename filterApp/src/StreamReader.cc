#include <iostream>
#include <string>
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "SimpleLineMatcher.hh"

ccl::Logger StreamReader::sLogger("StreamReader");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::StreamReader(DataStructModel *aModel,RecordWriter *aWriter)
  : _DataStructModel(aModel)
{
  _Mutex.lock();
  setRecordWriter(aWriter);
  _Mutex.unlock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::~StreamReader()
{
}

//-----------------------------------------------------------------------------
// TODO mem leak
//-----------------------------------------------------------------------------
void StreamReader::setRecordWriter(RecordWriter *aWriter)
{
  if (_Writers.size() == 0)
  {
    _Writers.push_back(aWriter);
  }
  else
  {
    _Writers[0] = aWriter;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::run()
{

  SimpleLineMatcher tBeginMessageMatcher(".*===RECEIVED MESSAGE===.*");
  SimpleLineMatcher tEndMessageMatcher(".*===END MESSAGE===.*");

  std::string tFirstFieldMatch = _DataStructModel->getFirstFieldMatch();

  SimpleLineMatcher tFirstFieldMatcher(tFirstFieldMatch);

  vector<std::string> tStructLines;

RecordProcessor *tRecordProcessor = new RecordProcessor( //TODO
    _DataStructModel->getTopNode(),tStructLines);

  bool tFoundStart = false;
  bool tFoundFirstField = false;

  std::string tLineBuffer;

  while (std::getline(std::cin,tLineBuffer))
  {
    if (!tFoundStart)
    {
      if (tBeginMessageMatcher.match(tLineBuffer) )
      {
        DEBUG(sLogger,"found start match");
        std::cout << "=== start ===" << std::endl;
        tFoundStart = true;
      }
      else
      {
        DEBUG(sLogger,"didn't find start match");
      }
    }
    else
    {
      if (!tEndMessageMatcher.match(tLineBuffer) )
      {
        if (!tFoundFirstField)
        {
          if (tFirstFieldMatcher.match(tLineBuffer))
          {
            tFoundFirstField = true;
          }
          else
          {
            DEBUG(sLogger,"discarding: " << tLineBuffer);
            continue;
          }
        }
        DEBUG(sLogger,"pushing back struct line");
        tStructLines.push_back(tLineBuffer);
      }
      else
      {
        DEBUG(sLogger,"reached end of structure");

        std::vector<std::string> tOutLines;

#define USE_NEW
#ifdef USE_NEW
        if (tRecordProcessor->process())
        {
          if (tRecordProcessor->passedFilterTests())
          {
            DEBUG(sLogger,"record passed filter tests");
            std::vector<std::string> &tOutLines = tRecordProcessor->getOutLines();
            std::vector<std::string>::iterator tIter;
            for (tIter = tOutLines.begin(); tIter != tOutLines.end(); tIter++)
            {
//              std::cout << "%" << *tIter << std::endl; //TODO
              std::cout << "%" << *tIter; //TODO
            }
          }
          else
          {
            DEBUG(sLogger,"record failed filter tests");
          }
        }
        else
        {
          std::cout << "ERROR: tRecordProcessor->process() returned false" << std::endl;
        }
#else
        std::vector<RecordWriter *>::iterator tIt;
        for (tIt = _Writers.begin(); tIt != _Writers.end(); tIt++)
        {
          (*tIt)->process(tStructLines);
        }
#endif
        tStructLines.clear();
        std::cout << "=== end ===" << std::endl;
        tFoundStart = false;
        tFoundFirstField = false;
      }
    }
  }
}
