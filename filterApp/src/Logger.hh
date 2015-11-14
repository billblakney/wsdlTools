/** Copyright (c) 2012 In-Depth Engineering. All rights reserved.
 **------------------------ UNCLASSIFIED --------------------------------------
 **
 ** @file   Logger.hh
 **
 ** @brief  Logger class which wraps a third-party logging system.
 **
 ** @author Andrew Glasscock, In-Depth Engineering
 **
 **----------------------------------------------------------------------------
 */

#ifndef CCL_LOGGER_HH 
#define CCL_LOGGER_HH

#ifdef USE_LOG4CXX
#include "log4cxx/logger.h"
#endif

#include <iostream>
#include <sstream>

/**
 * These macros provide users with a more concise format for writing logging
 * statements. They support the use of the stream operator (<<) within log
 * messages. Each macro will automatically check if the corresponding log
 * level is enabled (and avoid building the log message if not enabled).
 *
 *    Ex:   DEBUG( myLogger, "the value is " << 42 );
 */
#define DEBUG(aLogger, aMessage) LOG(aLogger, ccl::Logger::eDEBUG, aMessage)
#define INFO(aLogger, aMessage)  LOG(aLogger, ccl::Logger::eINFO,  aMessage)
#define WARN(aLogger, aMessage)  LOG(aLogger, ccl::Logger::eWARN,  aMessage)
#define ERROR(aLogger, aMessage) LOG(aLogger, ccl::Logger::eERROR, aMessage)
#define FATAL(aLogger, aMessage) LOG(aLogger, ccl::Logger::eFATAL, aMessage)

/**
 * This is a helper macro which is called by the other macros.
 */
#define LOG(aLogger, aLogLevel, aMessage) {\
   if(aLogger.isEnabledFor(aLogLevel)) {\
      std::stringstream tLogMessageStream;\
      tLogMessageStream << aMessage;\
      aLogger.log(aLogLevel, tLogMessageStream.str()); }}\

namespace ccl
{
   class Logger
   {
   public:
     
      /**
       * Definition of log levels
       */
      typedef enum LOG_LEVEL
      { 
         /**
          * Lowest possible logging level. Use the DEBUG logging level for 
          * events which are highly repetitive, or require a large amount
          * of data to be logged.
          */
         eDEBUG,

         /**
          * Use the INFO logging level to report events which happen
          * sporadically (such as by user interaction), or to indicate 
          * the status of the application. 
          */
         eINFO,

         /**
          * Use the WARN logging level to report events which are undesirable,
          * but may still allow the application to function normally.
          */
         eWARN,

         /**
          * Use the ERROR logging level when reporting events which likely 
          * prevent the application from performing its basic functions.
          */
         eERROR,

         /**
          * Highest possible logging level. Use the FATAL logging level to
          * report events which likely cause the application to terminate. 
          */
         eFATAL,
      } LOG_LEVEL;

      /**
       * Constructor
       *
       * @param aLoggerName - The name of a specific logger defined
       *                      in the log4cxx.properties file. Typically,
       *                      this should be the fully qualified name
       *                      of the class. Ex: "ccl.Logger"
       */
      Logger(std::string aLoggerName);

      /**
       * Returns the current logging level of this logger.
       */
      LOG_LEVEL getLevel();

      /**
       * Initializes the logging system. This should be called only once
       * for the entire application/process. You should typically call this
       * at the beginning of the program's main() method.
       */
      static void initialize();

      /**
       * Check whether this logger is enabled for the given log level.
       * 
       * It's a good idea to check the log level before doing a lot of
       * processing to build large debug strings.
       *
       * @param aLogLevel - the log level of interest
       * @return true if this logger is enabled for the given log level
       */
      bool isEnabledFor(LOG_LEVEL aLogLevel);

      /**
       * Attempts to log the given message at the given log level.
       *
       * If the log level specified in the log4cxx.properties file is
       * greater than the given log level, then the message will not
       * be logged.
       *
       * @param aLogLevel - log level associated with this message
       * @param aMessage - the message to be logged
       */
      void log(LOG_LEVEL aLogLevel, 
               const std::string aMessage);

      /**
       * Attempts to log the given message at the given log level.
       *
       * If the log level specified in the log4cxx.properties file is
       * greater than the given log level, then the message will not
       * be logged.
       *
       * @param aLogLevel - log level associated with this message
       * @param aMessage - the message to be logged
       */
      void log(LOG_LEVEL aLogLevel,
               const char * aMessage);

      /**
       * Sets the logging level of this logger.
       *
       * @param aLogLevel - log level to be set
       */
      void setLevel(LOG_LEVEL aLogLevel);

   private:
      
      /** The default log level to be used when USE_LOG4CXX is not set */
      static const LOG_LEVEL kDefaultLogLevel = eWARN;

#ifdef USE_LOG4CXX
      log4cxx::LoggerPtr _logger;
#endif

   };
 
} //end namespace ccl

#endif

//------------------------ UNCLASSIFIED --------------------------------------

