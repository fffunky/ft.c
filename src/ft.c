/*** includes ***/

#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <unistd.h>

#include "log/log.h"
#include "printutils/printutils.h"

/*** files ***/

/**
 * get_working_directory implements the getcwd() function in order to get
 * the directory the user is currently in. If an error is encountered then
 * the program is exited.
 */
char* get_working_directory(void)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        char* cur_dir = (char*)malloc(PATH_MAX);
        strcpy(cur_dir, cwd);
        return cur_dir;
    } else {
        log_fatal("getcwd() couldn't get current working directory.");
    }
    return "";
}

/**
 * filter needed for the dirent.h scandir function. Specifies which files
 * not to scan within the directory. If 0 is returned the file will not be
 * included but if 1 is returned then it will be. filters for directories in
 * the passed dirent struct.
 */
int dir_filter(const struct dirent* d)
{
    if (d->d_type != DT_DIR) {
        return 0;
    }

    if (d->d_name[0] == '.') {
        return 0;
    }

    if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
        return 0;
    }

    return 1;
}

/**
 * filter needed for the dirent.h scandir function. Specifies which files
 * not to scan within the directory. If 0 is returned the file will not be
 * included but if 1 is returned then it will be. filters for regular files in
 * the passed dirent struct.
 */
int file_filter(const struct dirent* d)
{
    if (d->d_type != DT_REG) {
        return 0;
    }

    if (d->d_name[0] == '.') {
        return 0;
    }

    if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
        return 0;
    }

    return 1;
}

/**
 * function passed into dirent.h's scandir do filter for
 * only hidden files.
 */
int hidden_file_filter(const struct dirent* d)
{
    if (d->d_type != DT_REG) {
        return 0;
    }

    // dont accept if the first char isn't a dot.
    if (d->d_name[0] != '.') {
        return 0;
    }

    if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
        return 0;
    }

    return 1;
}

/**
 * function passed into dirent.h's scandir do filter for
 * only hidden folders.
 */
int hidden_dir_filter(const struct dirent* d)
{
    if (d->d_type != DT_DIR) {
        return 0;
    }

    // dont accept if the first char isn't a dot.
    if (d->d_name[0] != '.') {
        return 0;
    }

    if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
        return 0;
    }

    return 1;
}

/**
 * get_directory_contents takes a directory name and an int*. This function
 * attempts to open the directory and stores the size of its contents in
 * the int* n variable. The program crashes if we can't scan the directory.
 */
struct dirent** get_subdirectories(char* dirname, int* n)
{
    struct dirent** contents;

    *n = scandir(dirname, &contents, dir_filter, alphasort);
    if (*n == -1) {
        log_fatal("couldn't scan directory '%s'", dirname);
    }

    return contents;
}

struct dirent** get_files(char* dirname, int* n)
{
    struct dirent** contents;

    *n = scandir(dirname, &contents, file_filter, alphasort);
    if (*n == -1) {
        log_fatal("couldn't scan directory '%s'", dirname);
    }

    return contents;
}

struct dirent** get_hidden_files(char* dirname, int* n)
{
    struct dirent** contents;

    *n = scandir(dirname, &contents, hidden_file_filter, alphasort);
    if (*n == -1) {
        log_fatal("couldn't scan directory '%s'", dirname);
    }

    return contents;
}

struct dirent** get_hidden_dirs(char* dirname, int* n)
{
    struct dirent** contents;

    *n = scandir(dirname, &contents, hidden_dir_filter, alphasort);
    if (*n == -1) {
        log_fatal("couldn't scan directory '%s'", dirname);
    }

    return contents;
}

// get_filepath finds the fill path of a file when given a filename.
// If filename is not found an empty string is returned and an error
// is thrown. You should free() the result of the function once you are
// done with it.
char* get_filepath(char* filename)
{
    char* path = realpath(filename, NULL);

    if (path == NULL) {
        log_fatal("cannot find file with name '%s'", filename);
        return "";
    } else {
        return path;
    }
}

long long get_file_size(struct dirent* dir)
{
    struct stat st;
    char*       path = get_filepath(dir->d_name);
    stat(path, &st);
    free(path);
    return (long long)st.st_size;
}

/*** data ***/

const char* DIR_COLOR = ANSI_CYAN;
const char* FILE_COLOR = ANSI_GREEN;
const char* EXEC_COLOR = ANSI_MAGENTA;
const char* HIDDEN_FILE_COLOR = ANSI_RED;
const char* HIDDEN_DIR_COLOR = ANSI_BLUE;
const char* FILE_SIZE_COLOR = ANSI_WHITE;

struct dir_item {
    struct dirent** dirs;
    struct dirent** files;
    struct dirent** hidden_dirs;
    struct dirent** hidden_files;
    size_t          dirs_len;
    size_t          files_len;
    size_t          hidden_dirs_len;
    size_t          hidden_files_len;
};

/**
 * new_dir_item creates a new struct dir_item which contains
 * the contents of a given directory and the size of its contents.
 */
struct dir_item new_dir_item(char* path)
{
    struct dir_item di;
    int             n, m, p, q;
    di.dirs = get_subdirectories(path, &n);
    di.dirs_len = (size_t)n;
    di.files = get_files(path, &m);
    di.files_len = (size_t)m;
    di.hidden_dirs = get_hidden_dirs(path, &p);
    di.hidden_dirs_len = (size_t)p;
    di.hidden_files = get_hidden_files(path, &q);
    di.hidden_files_len = q;
    return di;
}
/*** output ***/

typedef struct file_table {
    struct dir_item** dir_list;
    char**            dir_sizes;
} file_table;

size_t longest_filename(struct dir_item di, bool show_hidden)
{
    size_t longest = 1;
    size_t cur;
    int    i;

    if (show_hidden) {
        // iterate through hidden dirs
        for (i = 0; i < di.hidden_dirs_len; i++) {
            struct dirent* d = di.hidden_dirs[i];
            cur = strlen(d->d_name);
            if (cur > longest) {
                longest = cur;
            }
            cur = 0;
        }

        // iterate through hidden files
        for (i = 0; i < di.hidden_files_len; i++) {
            struct dirent* d = di.hidden_files[i];
            cur = strlen(d->d_name);
            if (cur > longest) {
                longest = cur;
            }
            cur = 0;
        }
    }

    // iterate through dirs
    for (i = 0; i < di.dirs_len; i++) {
        struct dirent* d = di.dirs[i];
        cur = strlen(d->d_name);
        if (cur > longest) {
            longest = cur;
        }
        cur = 0;
    }

    // iterate through files
    for (i = 0; i < di.files_len; i++) {
        struct dirent* d = di.files[i];
        cur = strlen(d->d_name);
        if (cur > longest) {
            longest = cur;
        }
        cur = 0;
    }

    return longest;
}

char* readable_file_size(long long sz)
{
    int       kb_sz = 1024;
    long      mb_sz = 1024 * 1024;
    long long gb_sz = 1024 * 1024 * 1024;
    char*     out = "";

    if (sz < kb_sz) {
        asprintf(&out, "%lli %s", sz, "bytes");
    } else if (kb_sz <= sz && sz <= mb_sz) {
        double print_sz = (double)sz / (double)kb_sz;
        asprintf(&out, "%.1f %s", print_sz, "kb");
    } else if (mb_sz <= sz && sz <= gb_sz) {
        double print_sz = (double)sz / (double)mb_sz;
        asprintf(&out, "%.1f %s", print_sz, "mb");
    } else {
        double print_sz = (double)sz / (double)gb_sz;
        asprintf(&out, "%.1f %s", print_sz, "gb");
    }

    return out;
}

void ch_pad(char c, int n)
{
    int i;
    for (i = 0; i < n; i++) {
        fputc(c, stdout);
    }
}

/**
 * print_directory_contents takes in a struct dir_item and prints out.
 * its contents in a formatted way. It first prints out the directories
 * and files with the print_subdirectories and print_files functions.
 * And then prints out any other file with a custom icon depending
 * on its type.
 */
void print_directory_contents(struct dir_item di, bool show_hidden)
{
    size_t longest = longest_filename(di, show_hidden);
    int    i, j;

    if (show_hidden) {
        // print hidden directories
        for (i = 0; i < di.hidden_dirs_len; i++) {
            struct dirent* d = di.hidden_dirs[i];
            size_t         name_len = strlen(d->d_name);
            char*          sz = readable_file_size(get_file_size(d));
            printf(ANSI_BOLD);
            color_print(HIDDEN_DIR_COLOR, "\u25B9 %s", d->d_name);
            if (name_len < longest) {
                ch_pad(' ', longest - name_len);
            }
            puts(sz);
        }
        // print hidden files
        for (i = 0; i < di.hidden_files_len; i++) {
            struct dirent* d = di.hidden_files[i];
            size_t         name_len = strlen(d->d_name);
            char*          sz = readable_file_size(get_file_size(d));
            color_print(HIDDEN_FILE_COLOR, "\u25AA %s", d->d_name);
            if (name_len < longest) {
                ch_pad(' ', longest - name_len);
            }
            puts(sz);
        }
    }
    // print subdirectories
    for (i = 0; i < di.dirs_len; i++) {
        struct dirent* d = di.dirs[i];
        size_t         name_len = strlen(d->d_name);
        char*          sz = readable_file_size(get_file_size(d));
        printf(ANSI_BOLD);
        color_print(DIR_COLOR, "\u25B9  %s ", d->d_name);
        if (name_len < longest) {
            ch_pad(' ', longest - name_len);
        }
        puts(sz);
    }

    // print regular files
    for (i = 0; i < di.files_len; i++) {
        struct dirent* d = di.files[i];
        size_t         name_len = strlen(d->d_name);
        char*          sz = readable_file_size(get_file_size(d));
        if (access(d->d_name, X_OK) == 0) {
            // print in a different color if the file is an executable file.
            color_print(EXEC_COLOR, "\u25AA  %s ", d->d_name);
            if (name_len < longest) {
                ch_pad(' ', longest - name_len);
            }
            puts(sz);
        } else {
            color_print(FILE_COLOR, "\u25AA  %s ", d->d_name, sz);
            if (name_len < longest) {
                ch_pad(' ', longest - name_len);
            }
            puts(sz);
        }
    }
    printf(ANSI_RESET_BOLD);
}

/*** helpers ***/

bool isArg(int argc, char** argv, char* target)
{
    int i;
    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i], target) == 0) {
            return true;
        }
    }
    return false;
}

/*** init ***/

static bool DEBUG = false;

void log_args(int argc, char** argv)
{
    int   i;
    char* out = "";
    for (i = 0; i < argc; i++) {
        asprintf(&out, "%s%s ", out, argv[i]);
    }
    log_info("argc: %d", argc);
    log_info("argv: %s", out);
}

int main(int argc, char** argv)
{
    char* cwd = get_working_directory();

    if (DEBUG) {
        log_args(argc, argv);
        log_info("working directory: %s", cwd);
    }

    struct dir_item di;
    char*           fpath = "null";
    bool            show_hidden = false;
    switch (argc) {
    case 1:
        di = new_dir_item(cwd);
        break;
    case 2:
        if (isArg(argc, argv, "-a")) {
            di = new_dir_item(cwd);
            show_hidden = true;
        } else {
            fpath = get_filepath(argv[1]);
            chdir(fpath);
            di = new_dir_item(fpath);
        }
        break;
    case 3:
        if (!isArg(argc, argv, "-a")) {
            log_fatal("unexpected arguments.");
        }

        int i, idx;
        for (i = 0; i < argc; i++) {
            if (strcmp("-a", argv[i]) == 0) {
                idx = i;
            }
        }

        idx = idx == 1 ? 2 : 1;
        fpath = get_filepath(argv[idx]);

        chdir(fpath);
        di = new_dir_item(fpath);
        show_hidden = true;
    }

    print_directory_contents(di, show_hidden);
    chdir(cwd);

    return 0;
}
