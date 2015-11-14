/** Copyright (c) 2012 In-Depth Engineering. All rights reserved.
 **------------------------ UNCLASSIFIED --------------------------------------
 ** 
 ** @file   Logger.cc
 ** 
 ** @brief  Logger class which wraps a third-party logging system.
 ** 
 ** @author Andrew Glasscock, In-Depth Engineering
 ** 
 **----------------------------------------------------------------------------
 */

#include "Logger.hh"

#ifdef USE_LOG4CXX
#include "log4cxx/propertyconfigurator.h"
#endif

#include <cstdlib> // getenv()

ccl::Logger::
Logger(std::string aLoggerName)
#ifdef USE_LOG4CXX
   :_logger(log4cxx::Logger::getLogger(aLoggerName))
#endif
{}

ccl::Logger::LOG_LEVEL ccl::Logger::
getLevel()
{
   ccl::Logger::LOG_LEVEL tReturn;

#ifdef USE_LOG4CXX
   if(log4cxx::Level::getDebug() == _logger->getLevel())
   {
      tReturn = ccl::Logger::eDEBUG;
   }
   else if(log4cxx::Level::getInfo() == _logger->getLevel())
   {
      tReturn = ccl::Logger::eINFO;
   }
   else if(log4cxx::Level::getWarn() == _logger->getLevel())
   {
      tReturn = ccl::Logger::eWARN;
   }
   else if(log4cxx::Level::getError() == _logger->getLevel())
   {
      tReturn = ccl::Logger::eERROR;
   }
   else //if(log4cxx::Level::getFatal() == _logger->getLevel())
   {
      tReturn = ccl::Logger::eFATAL;
   }
#else
   tReturn = kDefaultLogLevel;
#endif

   return tReturn;
}

bool ccl::Logger::
isEnabledFor(ccl::Logger::LOG_LEVEL aLogLevel)
{
   bool tReturn;

#ifdef USE_LOG4CXX
   switch(aLogLevel)
   {
      case ccl::Logger::eDEBUG:
      {
         tReturn = _logger->isEnabledFor(log4cxx::Level::getDebug());
         break;
      }
      case ccl::Logger::eINFO:
      {
         tReturn = _logger->isEnabledFor(log4cxx::Level::getInfo());
         break;
      }
      case ccl::Logger::eWARN:
      {
         tReturn = _logger->isEnabledFor(log4cxx::Level::getWarn());
         break;
      }
      case ccl::Logger::eERROR:
      {
         tReturn = _logger->isEnabledFor(log4cxx::Level::getError());
         break;
      }
      case ccl::Logger::eFATAL:
      {
         tReturn = _logger->isEnabledFor(log4cxx::Level::getFatal());
         break;
      }
   }
#else
   tReturn = (aLogLevel >= kDefaultLogLevel);
#endif

   return tReturn;
}

void ccl::Logger::
log(ccl::Logger::LOG_LEVEL aLogLevel,
    const std::string aMessage)
{
   this->log(aLogLevel, aMessage.c_str());
}

void ccl::Logger::
log(ccl::Logger::LOG_LEVEL aLogLevel,
    const char * aMessage)
{
#ifdef USE_LOG4CXX
   switch(aLogLevel)
   {
      case ccl::Logger::eDEBUG:
      {
         LOG4CXX_DEBUG(_logger, aMessage);
         break;
      }
      case ccl::Logger::eINFO:
      {
         LOG4CXX_INFO(_logger, aMessage);
         break;
      }
      case ccl::Logger::eWARN:
      {
         LOG4CXX_WARN(_logger, aMessage);
         break;
      }
      case ccl::Logger::eERROR:
      {
         LOG4CXX_ERROR(_logger, aMessage);
         break;
      }
      case ccl::Logger::eFATAL:
      {
         LOG4CXX_FATAL(_logger, aMessage);
         break;
      }
   }
#else
   if(aLogLevel >= kDefaultLogLevel)
   {
      std::cerr << aMessage << std::endl; 
   }
#endif
}

void ccl::Logger::
setLevel(ccl::Logger::LOG_LEVEL aLogLevel)
{
#ifdef USE_LOG4CXX
   switch(aLogLevel)
   {
      case ccl::Logger::eDEBUG:
      {
         _logger->setLevel(log4cxx::Level::getDebug());
         break;
      }
      case ccl::Logger::eINFO:
      {
         _logger->setLevel(log4cxx::Level::getInfo());
         break;
      }
      case ccl::Logger::eWARN:
      {
         _logger->setLevel(log4cxx::Level::getWarn());
         break;
      }
      case ccl::Logger::eERROR:
      {
         _logger->setLevel(log4cxx::Level::getError());
         break;
      }
      case ccl::Logger::eFATAL:
      {
         _logger->setLevel(log4cxx::Level::getFatal());
         break;
      }
   }
#endif
}

void ccl::Logger::
initialize()
{
   static bool tInitialized = false;
   if(!tInitialized)
   {
      tInitialized = true;

#ifdef USE_LOG4CXX
      std::string tConfigFile;
      if(getenv("LOGGER_CONFIG_FILE"))
      {
         tConfigFile = getenv("LOGGER_CONFIG_FILE");

         std::cerr << "using LOGGER_CONFIG_FILE at "
            << tConfigFile << std::endl;

         log4cxx::PropertyConfigurator::configureAndWatch(tConfigFile);
      }
      else
      {
         std::cerr << "ERROR: LOGGER_CONFIG_FILE env var is not set!\n";
      }
#endif
   }
}

//------------------------ UNCLASSIFIED --------------------------------------
