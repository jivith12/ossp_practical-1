#include "../include/dupfinder.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void init_file_list(FileList *list)
{
    list->count = 0;
    list->capacity = 16;
    list->items = malloc(list->capacity * sizeof(FileEntry));

    if (!list->items) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

void free_file_list(FileList *list)
{
    for (size_t i = 0; i < list->count; i++) {
        free(list->items[i].path);
    }

    free(list->items);
}

static void add_file(FileList *list, const char *path, off_t size)
{
    if (list->count == list->capacity) {
        list->capacity *= 2;

        FileEntry *temp = realloc(
            list->items,
            list->capacity * sizeof(FileEntry)
        );

        if (!temp) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        list->items = temp;
    }

    list->items[list->count].path = strdup(path);
    list->items[list->count].size = size;
    list->items[list->count].hash = 0;

    if (!list->items[list->count].path) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    list->count++;
}

void scan_directory(const char *path, FileList *list)
{
    DIR *dir = opendir(path);

    if (!dir) {
        perror(path);
        return;
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[4096];

        snprintf(
            full_path,
            sizeof(full_path),
            "%s/%s",
            path,
            entry->d_name
        );

        struct stat st;

        if (lstat(full_path, &st) == -1) {
            perror(full_path);
            continue;
        }

        /* Skip symbolic links */
        if (S_ISLNK(st.st_mode)) {
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            scan_directory(full_path, list);
        }
        else if (S_ISREG(st.st_mode)) {
            add_file(list, full_path, st.st_size);
        }
    }

    closedir(dir);
}
