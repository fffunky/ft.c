#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void log_format(const char* tag, const char* msg, va_list args)
{
    time_t now;
    time(&now);

    char* date = ctime(&now);
    date[strlen(date) - 1] = '\0'; // replace \n with terminator
    printf("%s [%s]: ", date, tag);
    vprintf(msg, args);
    printf("\n");
}

void log_error(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    log_format("error", msg, args);
    va_end(args);
}

void log_fatal(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    log_format("fatal error", msg, args);
    va_end(args);
    exit(1);
}

void log_info(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    log_format("info", msg, args);
    va_end(args);
}

void log_debug(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);
    log_format("debug", msg, args);
    va_end(args);
}
