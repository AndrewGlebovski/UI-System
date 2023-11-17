/**
 * \file
 * \brief Contains implementation of log file
*/


#include <time.h>
#include "common/log.hpp"


bool LogFile::createLogFile(const char *filename) {
    ASSERT(filename, "Invalid argument!\n");

    FILE *new_log_file = fopen(filename, "w");
    ASSERT(new_log_file, "Failed to open log file!\n");

    if (log_file) closeLogFile();

    log_file = new_log_file;
    
    fprintf(log_file, "Log file opened\n");
    printDate();

    return true;
}


bool LogFile::closeLogFile() {
    if (!log_file) return true;

    fprintf(log_file, "Log file closed\n");

    ASSERT(!fclose(log_file), "Failed to close log file!\n");

    log_file = nullptr;

    return true;
}


FILE *LogFile::getLogFile() {
    ASSERT(log_file, "Log file is not opened yet!\n");
    return log_file;
}


void LogFile::printDate() {
    if (!log_file) return;

    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fprintf(log_file, "Date: %s", asctime(timeinfo));
}


LogFile::~LogFile() {
    if (log_file) closeLogFile();
}


LogFile &LogFile::instance() {
    static LogFile instance_;
    return instance_;
}


LogFile::LogFile() :
    log_file(nullptr) {}
