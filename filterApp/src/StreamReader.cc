#include <iostream>
#include <string>
#include <QAction>
#include "StreamReader.hh"
#include "SimpleLineMatcher.hh"

ccl::Logger StreamReader::sLogger("StreamReader");

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::StreamReader(RecordProcessor *aRecordProcessor)
  : _NumRecordsTotal(0),
    _NumRecordsOutput(0),
    _RecordProcessor(aRecordProcessor),
    _DataStructModel(0),
    _DelimitMode(eOutputRecords),
    _OutputMode(eNormal),
    _InDelimitRecordsMode(true)
{
}

#if 0 //TODO use in future maybe
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::StreamReader(DataStructModel *aModel,RecordWriter *aWriter)
  : _DataStructModel(aModel),
    _RecordProcessor(0),
    _InBypassMode(false),
    _InDelimitRecordsMode(true)
{
  _Mutex.lock();
  setRecordWriter(aWriter);
  _Mutex.unlock();
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StreamReader::~StreamReader()
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RecordProcessor *StreamReader::getRecordProcessor()
{
  return _RecordProcessor;
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
StreamReader::OutputMode StreamReader::getOutputMode()
{
  OutputMode tOutputMode = eNormal;

  _Mutex.lock();
  tOutputMode = _OutputMode;
  _Mutex.unlock();

  return tOutputMode;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::setOutputMode(OutputMode aOutputMode)
{
  if (aOutputMode == eNormal)
  {
    std::cout << "Output mode set to NORMAL" << std::endl;
  }
  else if (aOutputMode == eBypass)
  {
    std::cout << "Output mode set to BYPASS" << std::endl;
  }
  else if (aOutputMode == eFreezeDrop)
  {
    std::cout << "Output mode set to FREEZE" << std::endl;
  }

  _Mutex.lock();
  _OutputMode = aOutputMode;
  _Mutex.unlock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::onOutputModeAction(QAction* aAction)
{
  OutputMode tMode = static_cast<OutputMode>(aAction->data().toInt());
  setOutputMode(tMode);
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
void StreamReader::onDelimitModeAction(QAction* aAction)
{
  DelimitMode tDelimitMode = static_cast<DelimitMode>(aAction->data().toInt());

  if (tDelimitMode == eAllRecords)
  {
    std::cout << "Delimiting all records." << std::endl;
  }
  else if (tDelimitMode == eOutputRecords)
  {
    std::cout << "Delimiting output records only." << std::endl;
  }
  else if (tDelimitMode == eNoRecords)
  {
    std::cout << "Disabling record delimiting." << std::endl;
  }

  _Mutex.lock();
  _DelimitMode = tDelimitMode;
  _Mutex.unlock();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StreamReader::run()
{
  if (_RecordProcessor == NULL)
  {
    std::cerr << "ERROR: no record processor was set" << std::endl;
    exit(0); //TODO change when have proper way to exit
  }

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
sleep(1);
  emit structNameAvailable(QString(tMsgId.c_str()),QString(tStructName.c_str()));

sleep(1);
  /*
   * Wait until the main window has set the data struct model before
   * continuing.
   */
  waitUntilDataModelAvailable();
  std::cout << "continuing with reading the input..." << std::endl;
sleep(1);

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
// This method processes the incoming stream of records. Each record has the
// following format:
//
// <start-of-record delimiter line>
// <header lines (only for some structure types)>
// <first structure line>
// <subsequent structure lines>
// <end-of-record delimiter line>
//-----------------------------------------------------------------------------
void StreamReader::readAndProcessStructLines()
{
  static std::string _prefix = "";

  SimpleLineMatcher tBeginMessageMatcher(".*===RECEIVED MESSAGE===.*");
  SimpleLineMatcher tEndMessageMatcher(".*===END MESSAGE===.*");

  _NumRecordsTotal = 0;
  _NumRecordsOutput = 0;

  static const char *kStartHeader =
      "==================================================";
  static const char *kEndDelimiter   = "---------%d/%d----------";

  std::string tFirstFieldMatch = _DataStructModel->getFirstFieldMatch();
  DEBUG(sLogger,"match to start record: " << tFirstFieldMatch);

  SimpleLineMatcher tFirstFieldMatcher(tFirstFieldMatch);

  _RecordProcessor->configure(_DataStructModel->getTopNode());

  bool tFoundStart = false;
  bool tFoundFirstField = false;

  std::vector<std::string> tStructLines;
  std::string tLineBuffer;

  std::cout << kStartHeader << std::endl;

  while (std::getline(std::cin,tLineBuffer))
  {
    /*
     * If in bypass mode, just echo lines.
     */
    if (getOutputMode() == eBypass)
    {
      std::cout << tLineBuffer << std::endl;
      continue;
    }
    /*
     * If in freeze-drop mode, just drop lines.
     */
    else if (getOutputMode() == eFreezeDrop)
    {
      continue;
    }

    /*
     * Not in bypass or freeze-drop mode, so do normal processing.
     *
     * At this point, we'll be in one of two modes: looking for the
     * start-of-record delimiter, or processing the other types of lines.
     *
     * If we are looking for the start-of-record delimiter, the just check the
     * line against the start-of-record delimiter. If it matches, then set the
     * corresponding flag. Otherwise, do nothing with this line. In either case
     * that's it for this line.
     */
    if (!tFoundStart)
    {
      if (tBeginMessageMatcher.match(tLineBuffer) )
      {
        DEBUG(sLogger,"found start match");
        tFoundStart = true;
      }
      else
      {
        DEBUG(sLogger,"didn't find start match");
      }
    }
    /*
     * Else we are processing record lines, which could be any of the following
     * types:
     *   <header lines (only for some structure types)>
     *   <first structure line>
     *   <subsequent structure lines>
     *   <end-of-record delimiter line>
     */
    else
    {
      /*
       * If the line is an end-of-record delimiter line, process the record.
       */
      if (tEndMessageMatcher.match(tLineBuffer) )
      {
        DEBUG(sLogger,"reached end of structure");

        bool tWasOutput = false;
        std::vector<std::string> tOutLines;

        if (_RecordProcessor->process(&tStructLines))
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
            tWasOutput = true;
          }
          else
          {
            DEBUG(sLogger,"record failed filter tests");
          }
        }
        else
        {
          std::cout << "ERROR: _RecordProcessor->process() returned false"
              << std::endl;
        }
        tStructLines.clear();

        if (_DelimitMode == eAllRecords ||
            (_DelimitMode == eOutputRecords && tWasOutput))
        {
          _NumRecordsOutput++;

          char tBuff[81];
          sprintf(tBuff,kEndDelimiter,_NumRecordsOutput,_NumRecordsTotal);
          std::cout << tBuff << std::endl;
        }
        tFoundStart = false;
        tFoundFirstField = false;
      }
      /*
       * Else we are still processing lines in a record.
       */
      else
      {
        /*
         * If we haven't found the first structure line, the test against the
         * start-of-structure pattern.
         */
        if (!tFoundFirstField)
        {
          if (tFirstFieldMatcher.match(tLineBuffer))
          {
            tFoundFirstField = true;
            _NumRecordsTotal++;
          }
          else
          {
            DEBUG(sLogger,"discarding: " << tLineBuffer);
          }
        }
        /*
         * If we've already found the start-of-structure line, then the current
         * line is a structure line. Save it off to the list of structure lines.
         */
        if (tFoundFirstField)
        {
          DEBUG(sLogger,"pushing back struct line");
          tStructLines.push_back(tLineBuffer);
        }
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
