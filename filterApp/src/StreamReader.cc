#include <iostream>
#include <string>
#include "StreamReader.hh"
#include "SimpleLineMatcher.hh"

ccl::Logger StreamReader::sLogger("StreamReader");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::StreamReader()
  : _DataStructModel(0),
    _RecordProcessor(0),
    _InBypassMode(false),
    _InDelimitRecordsMode(true)
{
  _RecordProcessor = new RecordProcessor(_StructLines);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::StreamReader(DataStructModel *aModel,RecordWriter *aWriter)
  : _DataStructModel(aModel),
    _RecordProcessor(0),
    _InBypassMode(false),
    _InDelimitRecordsMode(true)
{
  _RecordProcessor = new RecordProcessor(_StructLines);

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
void StreamReader::onDataStructModelAvailable(void * aDataStructModel)
{
  setDataStructModel(static_cast<DataStructModel *>(aDataStructModel));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool StreamReader::inBypassMode()
{
  bool tInBypassMode;

  _Mutex.lock();
  tInBypassMode = _InBypassMode;
  _Mutex.unlock();

  return tInBypassMode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::setInBypassMode(bool aInBypassMode)
{
  if (aInBypassMode)
  {
    std::cout << "Entering BYPASS mode." << std::endl;
  }
  else
  {
    std::cout << "Leaving bypass mode." << std::endl;
  }

  _Mutex.lock();
  _InBypassMode = aInBypassMode;
  _Mutex.unlock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::onBypassToggle(bool aIsChecked)
{
  setInBypassMode(aIsChecked);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool StreamReader::inDelimitRecordsMode()
{
  bool tDelimitRecordsMode;

  _Mutex.lock();
  tDelimitRecordsMode = _InDelimitRecordsMode;
  _Mutex.unlock();

  return tDelimitRecordsMode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::setDelimitRecordsMode(bool aDelimitRecordsMode)
{
  if (aDelimitRecordsMode)
  {
    std::cout << "Enabling record delimiters." << std::endl;
  }
  else
  {
    std::cout << "Disabling record delimiters." << std::endl;
  }

  _Mutex.lock();
  _InDelimitRecordsMode = aDelimitRecordsMode;
  _Mutex.unlock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::onDelimitRecordsToggle(bool aIsChecked)
{
  setDelimitRecordsMode(aIsChecked);
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

  /*
   * Wait until the main window has set the data struct model before
   * continuing.
   */
  waitUntilDataModelAvailable();
  std::cout << "continuing with reading the input..." << std::endl;

  /*
   * Read and process the incoming struct lines.
   */
  readAndProcessStructLines();
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
// Waits for the data model from the main window to be available.
//-----------------------------------------------------------------------------
void StreamReader::waitUntilDataModelAvailable()
{
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
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::readAndProcessStructLines()
{
  static std::string _prefix = "";

  SimpleLineMatcher tBeginMessageMatcher(".*===RECEIVED MESSAGE===.*");
  SimpleLineMatcher tEndMessageMatcher(".*===END MESSAGE===.*");

  std::string tFirstFieldMatch = _DataStructModel->getFirstFieldMatch();

  SimpleLineMatcher tFirstFieldMatcher(tFirstFieldMatch);

  _RecordProcessor->configure(_DataStructModel->getTopNode());

  bool tFoundStart = false;
  bool tFoundFirstField = false;

  std::string tLineBuffer;

  while (std::getline(std::cin,tLineBuffer))
  {
    /*
     * If in bypass mode, just echo lines.
     */
    if (inBypassMode() == true)
    {
      std::cout << tLineBuffer << std::endl;
      continue;
    }

    /*
     * Not in bypass mode, so do normal processing.
     */
    if (!tFoundStart)
    {
      if (tBeginMessageMatcher.match(tLineBuffer) )
      {
        DEBUG(sLogger,"found start match");
        if (inDelimitRecordsMode())
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
        _StructLines.push_back(tLineBuffer);
      }
      else
      {
        DEBUG(sLogger,"reached end of structure");

        std::vector<std::string> tOutLines;

        if (_RecordProcessor->process())
        {
          if (_RecordProcessor->passedFilterTests())
          {
            DEBUG(sLogger,"record passed filter tests");
            std::vector<std::string> &tOutLines = _RecordProcessor->getOutLines();
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
          std::cout << "ERROR: _RecordProcessor->process() returned false" << std::endl;
        }
        _StructLines.clear();
        if (inDelimitRecordsMode())
        {
          std::cout << "=== end ===" << std::endl;
        }
        tFoundStart = false;
        tFoundFirstField = false;
      }
    }
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::setDataStructModel(DataStructModel *aModel)
{
  _Mutex.lock();
  _DataStructModel = aModel;
  _Mutex.unlock();
}
