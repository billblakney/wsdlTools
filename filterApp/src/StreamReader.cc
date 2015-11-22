#include <iostream>
#include <string>
#include "RecordProcessor.hh"
#include "StreamReader.hh"
#include "SimpleLineMatcher.hh"

ccl::Logger StreamReader::sLogger("StreamReader");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::StreamReader(void (*aSetStructCallback)(std::string aStructName))
  : _DataStructModel(0)
{
  _SetStructCallback =aSetStructCallback;
}

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
//-----------------------------------------------------------------------------
void StreamReader::setDataStructModel(DataStructModel *aModel)
{
  _Mutex.lock();
  _DataStructModel = aModel;
  _Mutex.unlock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::onDataStructModelAvailable(void * aDataStructModel)
{
  std::cout << "XXXXXXXXXXXXXXXXXXXXX " << aDataStructModel <<std::endl;
  _DataStructModel = static_cast<DataStructModel *>(aDataStructModel);
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
// Processes input to get structure name.
//-----------------------------------------------------------------------------
void StreamReader::readForStructName(
    std::string &aMsgId,std::string &aStructName)
{
  std::string tStructNameLineMatch(".*(ssifg_[^\\s]+)\\s+.*?([\\w]+)$");
  SimpleLineMatcher tMatcher(tStructNameLineMatch);

  std::string tLineBuffer;
  while (std::getline(std::cin,tLineBuffer))
  {
      if (tMatcher.match(tLineBuffer) )
      {
        aMsgId = tMatcher.getWhat(1);
        aStructName = tMatcher.getWhat(2);
        return;
      }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::run()
{
  /*
   * Read the input to find the structure name that is being used.
   */
  std::string tMsgId;
  std::string tStructName;

  readForStructName(tMsgId,tStructName);
  std::cout << "processing for " << tMsgId << " " << tStructName << std::endl;

  /*
   * Invoke the callback to let the main window (via main stuff) know the
   * structure type that will be processed.
   * TODO signal to main window directly possible/better?
   */
  emit structNameAvailable(QString(tStructName.c_str()));
//  _SetStructCallback(tStructName); //TODO cleanup

  bool tDataStructModelReady = false;
  while(!tDataStructModelReady)
  {
    _Mutex.lock();
    if (_DataStructModel != 0)
    {
      tDataStructModelReady = true;
    }
    _Mutex.unlock();
  }
  std::cout << "continuing with reading the input..." << std::endl;

  /*
   *
   */

  static std::string _prefix = "";
  static bool _printStartAndEnd = true;

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
        if (_printStartAndEnd)
        {
          std::cout << "=== start ===" << std::endl;
        }
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
              std::cout << _prefix << *tIter; //TODO
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
        if (_printStartAndEnd)
        {
          std::cout << "=== end ===" << std::endl;
        }
        tFoundStart = false;
        tFoundFirstField = false;
      }
    }
  }
}
