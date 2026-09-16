#ifndef DUPFINDER_H
#define DUPFINDER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
    char *path;
    off_t size;
    uint64_t hash;
} FileEntry;

typedef struct {
    FileEntry *items;
    size_t count;
    size_t capacity;
} FileList;

typedef struct {
    char **paths;
    size_t count;
    size_t capacity;
    off_t size;
} DupGroup;

/* Scanner */
void init_file_list(FileList *list);
void free_file_list(FileList *list);
void scan_directory(const char *path, FileList *list);

/* Hashing */
uint64_t hash_file(const char *path);
int files_equal(const char *path1, const char *path2);

/* Grouper */
void find_duplicates(FileList *files, DupGroup **groups, size_t *group_count);
void free_groups(DupGroup *groups, size_t group_count);

/* UI */
void print_report(DupGroup *groups, size_t group_count);
void interactive_delete(DupGroup *groups, size_t group_count);

#endif

