#include "../include/dupfinder.h"

#include <string.h>

static int compare_files(const void *a, const void *b)
{
    const FileEntry *fa = a;
    const FileEntry *fb = b;

    if (fa->size < fb->size)
        return -1;

    if (fa->size > fb->size)
        return 1;

    if (fa->hash < fb->hash)
        return -1;

    if (fa->hash > fb->hash)
        return 1;

    return 0;
}

static void add_to_group(
    DupGroup *group,
    const char *path
)
{
    if (group->count == group->capacity) {

        group->capacity *= 2;

        char **temp = realloc(
            group->paths,
            group->capacity * sizeof(char *)
        );

        if (!temp) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }

        group->paths = temp;
    }

    group->paths[group->count] = strdup(path);

    if (!group->paths[group->count]) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    group->count++;
}

void find_duplicates(
    FileList *files,
    DupGroup **groups,
    size_t *group_count
)
{
    *groups = NULL;
    *group_count = 0;

    /* Calculate hashes */
    for (size_t i = 0; i < files->count; i++) {
        files->items[i].hash =
            hash_file(files->items[i].path);
    }

    /* Sort by size and hash */
    qsort(
        files->items,
        files->count,
        sizeof(FileEntry),
        compare_files
    );

    size_t i = 0;

    while (i < files->count) {

        size_t j = i + 1;

        while (j < files->count &&
               files->items[j].size ==
               files->items[i].size &&
               files->items[j].hash ==
               files->items[i].hash) {

            j++;
        }

        if (j - i >= 2) {

            DupGroup *temp = realloc(
                *groups,
                (*group_count + 1) * sizeof(DupGroup)
            );

            if (!temp) {
                perror("realloc");
                exit(EXIT_FAILURE);
            }

            *groups = temp;

            DupGroup *group =
                &(*groups)[*group_count];

            group->count = 0;
            group->capacity = 2;
            group->size = files->items[i].size;

            group->paths =
                malloc(group->capacity * sizeof(char *));

            if (!group->paths) {
                perror("malloc");
                exit(EXIT_FAILURE);
            }

            /*
             * Compare files against the first file.
             * This confirms actual byte equality.
             */
            add_to_group(
                group,
                files->items[i].path
            );

            for (size_t k = i + 1; k < j; k++) {

                if (files_equal(
                        files->items[i].path,
                        files->items[k].path)) {

                    add_to_group(
                        group,
                        files->items[k].path
                    );
                }
            }

            if (group->count >= 2) {
                (*group_count)++;
            }
            else {
                free(group->paths);
            }
        }

        i = j;
    }
}

void free_groups(
    DupGroup *groups,
    size_t group_count
)
{
    for (size_t i = 0; i < group_count; i++) {

        for (size_t j = 0;
             j < groups[i].count;
             j++) {

            free(groups[i].paths[j]);
        }

        free(groups[i].paths);
    }

    free(groups);
}
