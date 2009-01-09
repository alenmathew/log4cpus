// Module:  Log4CPLUS
// File:    logger.h
// Created: 6/2001
// Author:  Tad E. Smith
//
//
// Copyright (C) Tad E. Smith  All rights reserved.
//
// This software is published under the terms of the Apache Software
// License version 1.1, a copy of which has been included with this
// distribution in the LICENSE.APL file.
//

/** @file 
 * This header defines the Logger class and the logging macros. */

#ifndef _LOG4CPLUS_LOGGERHEADER_
#define _LOG4CPLUS_LOGGERHEADER_

#include <log4cplus/config.hxx>
#include <log4cplus/loglevel.h>
#include <log4cplus/tstring.h>
#include <log4cplus/streams.h>
#include <log4cplus/helpers/pointer.h>
#include <log4cplus/spi/appenderattachable.h>
#include <log4cplus/spi/loggerfactory.h>
#include <log4cplus/spi/loggerimpl.h>

#include <memory>
#include <vector>

namespace log4cplus {
    // Forward declarations

    class Appender;
    class Hierarchy;
    class HierarchyLocker;
    class DefaultLoggerFactory;


    /** \typedef std::vector<Logger> LoggerList
     * This is a list of {@link Logger Loggers}. */
    typedef std::vector<Logger> LoggerList;


    /**
     * This is the central class in the log4cplus package. One of the
     * distintive features of log4cplus are hierarchical loggers and their
     * evaluation.
     * <p>
     * See the <a href="../../../../manual.html">user manual</a> for an
     * introduction on this class.
     */
    class LOG4CPLUS_EXPORT Logger
        : public log4cplus::spi::AppenderAttachable
    {
    public:
      // Static Methods
        /**
         * Returns <code>true </code>if the named logger exists 
         * (in the default hierarchy).
         *                
         * @param name The name of the logger to search for.
         */
        static bool exists(const log4cplus::tstring& name);

        /*
         * Returns all the currently defined loggers in the default
         * hierarchy.
         * <p>
         * The root logger is <em>not</em> included in the returned
         * list.      
        */
        static LoggerList getCurrentLoggers();
     
        /**
         * Return the default Hierarchy instance.
         */
        static Hierarchy& getDefaultHierarchy();

        /**
         * Retrieve a logger with name <code>name</code>.  If the named 
         * logger already exists, then the existing instance will be returned. 
         * Otherwise, a new instance is created. 
         * <p>
         * By default, loggers do not have a set LogLevel but inherit
         * it from the hierarchy. This is one of the central features of
         * log4cplus.
         * <p>
         * @param name The name of the logger to retrieve.  
         */
        static Logger getInstance(const log4cplus::tstring& name);

        /**
         * Like {@link #getInstance(log4cplus::tstring)} except that the type of logger
         * instantiated depends on the type returned by the {@link
         * spi::LoggerFactory#makeNewLoggerInstance} method of the
         * <code>factory</code> parameter.
         * <p>                         
         * This method is intended to be used by sub-classes.
         * <p>                                  
         * @param name The name of the logger to retrieve.
         * @param factory A {@link spi::LoggerFactory} implementation that will
         * actually create a new Instance.
         */
        static Logger getInstance(const log4cplus::tstring& name, spi::LoggerFactory& factory);

        /**
         * Return the root of the default logger hierrachy.
         * <p>
         * The root logger is always instantiated and available. It's
         * name is "root".
         * <p>
         * Nevertheless, calling {@link #getInstance
         * Logger.getInstance("root")} does not retrieve the root logger 
         * but a logger just under root named "root".
         */
        static Logger getRoot();

        /**
         * Calling this method will <em>safely</em> close and remove all
         * appenders in all the loggers including root contained in the
         * default hierachy.
         * <p>                    
         * Some appenders such as SocketAppender need to be closed before the
         * application exits. Otherwise, pending logging events might be
         * lost.
         * <p>
         * The <code>shutdown</code> method is careful to close nested
         * appenders before closing regular appenders. This is allows
         * configurations where a regular appender is attached to a logger
         * and again to a nested appender.  
         */
        static void shutdown();

      // Non-Static Methods
        /**
         * If <code>assertion</code> parameter is <code>false</code>, then
         * logs <code>msg</code> as an {@link #error(const log4cplus::tstring&) error} 
         * statement.
         *
         * @param assertion 
         * @param msg The message to print if <code>assertion</code> is
         * false.
         */
        void assertion(bool assertionVal, const log4cplus::tstring& msg) const
        {
            if(!assertionVal) {
                log(FATAL_LOG_LEVEL, msg);
            }
        }

        /**
         * Close all attached appenders implementing the AppenderAttachable
         * interface.  
         */
        void closeNestedAppenders() const
        {
            value->closeNestedAppenders();
        }

        /**
         * Check whether this logger is enabled for a given {@link
         * LogLevel} passed as parameter.
         *
         * @return boolean True if this logger is enabled for <code>ll</code>.
         */
        bool isEnabledFor(LogLevel ll) const
        {
            return value->isEnabledFor(ll);
        }

        /**
         * This generic form is intended to be used by wrappers. 
         */
        void log(LogLevel ll, const log4cplus::tstring& message,
                 const char* file=NULL, int line=-1) const
        {
            value->log(ll, message, file, line);
        }

        /**
         * This method creates a new logging event and logs the event
         * without further checks.  
         */
        void forcedLog(LogLevel ll, const log4cplus::tstring& message,
                       const char* file=NULL, int line=-1) const
        {
            value->forcedLog(ll, message, file, line);
        }

        /**
         * Call the appenders in the hierrachy starting at
         * <code>this</code>.  If no appenders could be found, emit a
         * warning.
         * <p>
         * This method calls all the appenders inherited from the
         * hierarchy circumventing any evaluation of whether to log or not
         * to log the particular log request.
         *
         * @param spi::InternalLoggingEvent the event to log.
         */
        void callAppenders(const spi::InternalLoggingEvent& event) const
        {
            value->callAppenders(event);
        }

        /**
         * Starting from this logger, search the logger hierarchy for a
         * "set" LogLevel and return it. Otherwise, return the LogLevel of the
         * root logger.
         * <p>
         * The Logger class is designed so that this method executes as
         * quickly as possible.
         */
        LogLevel getChainedLogLevel() const
        {
            return value->getChainedLogLevel();
        }

        /**
         * Returns the assigned {@link LogLevel}, if any, for this Logger.  
         *           
         * @return LogLevel - the assigned LogLevel, can be <code>NOT_SET_LOG_LEVEL</code>.
         */
        LogLevel getLogLevel() const
        {
            return value->getLogLevel();
        }

        /**
         * Set the LogLevel of this Logger.
         */
        void setLogLevel(LogLevel ll)
        {
            value->setLogLevel(ll);
        }


        /**
         * Return the the {@link Hierarchy} where this <code>Logger</code> instance is
         * attached.
         */
        Hierarchy& getHierarchy() const
        { 
            return value->getHierarchy();
        }


        /**
         * Return the logger name.  
         */
        log4cplus::tstring const & getName() const
        {
            return value->getName();
        }


        /**
         * Get the additivity flag for this Logger instance.  
         */
        bool getAdditivity() const
        {
            return value->getAdditivity();
        }

        /**
         * Set the additivity flag for this Logger instance.
         */
        void setAdditivity(bool additive)
        { 
            value->setAdditivity(additive);
        }


      // AppenderAttachable Methods
        virtual void addAppender(SharedAppenderPtr newAppender);

        virtual SharedAppenderPtrList getAllAppenders();

        virtual SharedAppenderPtr getAppender(const log4cplus::tstring& name);

        virtual void removeAllAppenders();

        virtual void removeAppender(SharedAppenderPtr appender);

        virtual void removeAppender(const log4cplus::tstring& name);

      // Copy Ctor
        Logger(const Logger& rhs);
        Logger& operator=(const Logger& rhs);

      // Dtor
        ~Logger();

        /**
         * Used to retrieve the parent of this Logger in the
         * Logger tree.
         */
        Logger getParent() const;

    protected:
      // Data
        /** This is a pointer to the implementation class. */
        spi::SharedLoggerImplPtr value;

    private:
      // Ctors
        /**
         * This constructor created a new <code>Logger</code> instance 
         * with a pointer to a Logger implementation.
         * <p>
         * You should not create loggers directly.
         *
         * @param ptr A pointer to the Logger implementation.  This value
         *            cannot be NULL.  
         */
        Logger(spi::LoggerImpl *ptr);
        Logger(const spi::SharedLoggerImplPtr& val);

      // Friends
        friend class log4cplus::spi::LoggerImpl;
        friend class log4cplus::Hierarchy;
        friend class log4cplus::HierarchyLocker;
        friend class log4cplus::DefaultLoggerFactory;
    };


    /**
     * This class is used to create the default implementation of
     * the Logger class
     */
    class LOG4CPLUS_EXPORT DefaultLoggerFactory : public spi::LoggerFactory {
    public:
        Logger makeNewLoggerInstance(const log4cplus::tstring& name, Hierarchy& h);
    };


} // end namespace log4cplus


#include <log4cplus/loggingmacros.h>
#include <log4cplus/tracelogger.h>


#endif // _LOG4CPLUS_LOGGERHEADER_
