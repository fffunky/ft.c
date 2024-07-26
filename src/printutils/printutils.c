#include "printutils.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * takes 1 foreground and 1 backgorund color code and combines them
 * to be applied together. i.e., red_fg = "\x1b[31m", blue_bg = "\x1b[44m"
 * => color_pair = "\x1b[31;44m"
 */
char* new_color_pair(const char* txt_color, const char* bg_color)
{
    char txt_code[8] = "";
    char bg_code[9] = "";

    int i, j;
    // get txt color code
    for (i = 0; txt_color[i] != '\0'; i++) {
        if (is_digit(txt_color[i])) {
            strncat(txt_code, &txt_color[i], 1);
        }
    }
    txt_code[7] = '\0';

    // get bg color code
    for (j = 0; bg_color[j] != '\0'; j++) {
        if (is_digit(bg_color[j])) {
            strncat(bg_code, &bg_color[j], 1);
        }
    }
    bg_code[8] = '\0';

    size_t pair_size = sizeof("\x1b[m;") + sizeof(txt_code) + sizeof(bg_code);
    char*  color_pair = malloc(pair_size);
    sprintf(color_pair, "\x1b[%s;%sm", txt_code, bg_code);

    return color_pair;
}

void color_print(const char* color_code, char* msg, ...)
{
    va_list args;
    va_start(args, msg);

    printf("%s", color_code);
    vprintf(msg, args);
    printf("%s", ANSI_RESET_COLOR);

    va_end(args);
}

void bgcolor_print(const char* txt_color, const char* bg_color, char* msg, ...)
{
    va_list args;
    va_start(args, msg);

    char* color_pair = new_color_pair(txt_color, bg_color);

    printf("%s", color_pair);
    vprintf(msg, args);
    printf("%s", ANSI_RESET_COLOR);

    va_end(args);
    free(color_pair);
}

bool is_digit(char c)
{
    return '0' <= c && c <= '9';
}
