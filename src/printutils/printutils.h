#ifndef PRINTUTILS_H
#define PRINTUTILS_H

#include <stdbool.h>

/*** color codes ***/

// text color codes
#define ANSI_BLACK   "\x1b[30m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_WHITE   "\x1b[37m"

// background/highlight color codes
#define ANSI_BG_BLACK   "\x1b[40m"
#define ANSI_BG_RED     "\x1b[41m"
#define ANSI_BG_GREEN   "\x1b[42m"
#define ANSI_BG_YELLOW  "\x1b[43m"
#define ANSI_BG_BLUE    "\x1b[44m"
#define ANSI_BG_MAGENTA "\x1b[45m"
#define ANSI_BG_CYAN    "\x1b[46m"
#define ANSI_BG_WHITE   "\x1b[47m"

// reset color
#define ANSI_RESET_COLOR "\x1b[0m"

/*** text formatting ***/
#define ANSI_BOLD            "\x1b[1m"
#define ANSI_RESET_BOLD      "\x1b[22m"
#define ANSI_ITALICS         "\x1b[3m"
#define ANSI_RESET_ITALICS   "\x1b[23m"
#define ANSI_UNDERLINE       "\x1b[4m"
#define ANSI_RESET_UNDERLINE "\x1b[24m"

/*** string functions ***/

/*** printing functions ***/

char* new_color_pair(const char* txt_color, const char* bg_color);
void  color_print(const char* color_code, char* msg, ...);
void bgcolor_print(const char* txt_color, const char* bg_color, char* msg, ...);
bool is_digit(char c);

#endif
