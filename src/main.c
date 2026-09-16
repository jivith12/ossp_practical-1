#include "../include/dupfinder.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {

        printf(
            "Usage: %s [-i] <directory> [directory2 ...]\n",
            argv[0]
        );

        return 1;
    }

    int interactive = 0;
    int start = 1;

    if (strcmp(argv[1], "-i") == 0) {

        interactive = 1;
        start = 2;

        if (argc < 3) {

            printf(
                "Error: No directory specified.\n"
            );

            return 1;
        }
    }

    FileList files;

    init_file_list(&files);

    for (int i = start; i < argc; i++) {

        printf(
            "Scanning: %s\n",
            argv[i]
        );

        scan_directory(
            argv[i],
            &files
        );
    }

    printf(
        "Scanned %zu file(s).\n",
        files.count
    );

    if (files.count < 2) {

        printf(
            "Not enough files to find duplicates.\n"
        );

        free_file_list(&files);

        return 0;
    }

    DupGroup *groups = NULL;
    size_t group_count = 0;

    find_duplicates(
        &files,
        &groups,
        &group_count
    );

    print_report(
        groups,
        group_count
    );

    if (interactive && group_count > 0) {

        interactive_delete(
            groups,
            group_count
        );
    }

    free_groups(
        groups,
        group_count
    );

    free_file_list(&files);

    return 0;
}
