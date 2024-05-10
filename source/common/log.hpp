/**
 * \file
 * \brief Contains interface of log class
*/


#ifndef _LOG_H_
#define _LOG_H_


#include <cstdio>
#include "common/assert.hpp"


/// Singleton that stores current log file
class LogFile {
public:
    /**
     * \brief Rewrites log file or creates it
     * \return True on success
    */
    bool createLogFile(const char *filename);

    /**
     * \brief Closes log file
     * \return True on success
    */
    bool closeLogFile();

    /**
     * \brief Returns file stream
    */
    FILE *getLogFile();

    /**
     * \brief Returns instance of log file
    */
    static LogFile &instance();

    /**
     * \brief Prints current date to log file if it's open
    */
    void printDate();

    LogFile(const LogFile&) = delete;
    LogFile &operator = (const LogFile&) = delete;

    /**
     * \brief Closes file if its open
    */
    ~LogFile();

private:
    /**
     * \brief Creates register
    */
    LogFile();

    FILE *log_file;
};


#define ENABLE_LOG


#ifdef ENABLE_LOG


/**
 * \brief Prints message to log file without name specification 
*/
#define PRINT_ANON_LOG(...)                                 \
do {                                                        \
    fprintf(LogFile::instance().getLogFile(), __VA_ARGS__); \
} while(0)


/**
 * \brief Prints message to log file with object name and pointer
*/
#define PRINT_LOG(NAME, PTR, ...)                                       \
do {                                                                    \
    fprintf(LogFile::instance().getLogFile(), "%s [%p]: ", NAME, PTR);  \
    fprintf(LogFile::instance().getLogFile(), __VA_ARGS__);             \
} while(0)


#else


/**
 * \brief Logs disabled so does nothing
*/
#define PRINT_ANON_LOG(...)


/**
 * \brief Logs disabled so does nothing
*/
#define PRINT_LOG(NAME, PTR, ...)


#endif 


#endif
