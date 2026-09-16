#include "../include/dupfinder.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_report(
    DupGroup *groups,
    size_t group_count
)
{
    printf("\n");
    printf("=============== Duplicate File Report ===============\n");

    if (group_count == 0) {
        printf("No duplicate files found.\n");
        printf("======================================================\n");
        return;
    }

    long long reclaimable = 0;

    for (size_t i = 0; i < group_count; i++) {

        DupGroup *group = &groups[i];

        printf(
            "\nGroup %zu (%zu copies, %lld bytes each)\n",
            i + 1,
            group->count,
            (long long)group->size
        );

        for (size_t j = 0;
             j < group->count;
             j++) {

            printf(
                " [%zu] %s\n",
                j + 1,
                group->paths[j]
            );
        }

        reclaimable +=
            (long long)group->size *
            (long long)(group->count - 1);
    }

    printf(
        "\n======================================================\n"
    );

    printf(
        "Found %zu duplicate group(s).\n",
        group_count
    );

    printf(
        "Reclaimable space: %lld bytes\n",
        reclaimable
    );
}

void interactive_delete(
    DupGroup *groups,
    size_t group_count
)
{
    for (size_t i = 0; i < group_count; i++) {

        DupGroup *group = &groups[i];

        printf(
            "\nGroup %zu: %zu copies\n",
            i + 1,
            group->count
        );

        for (size_t j = 0;
             j < group->count;
             j++) {

            printf(
                " [%zu] %s\n",
                j + 1,
                group->paths[j]
            );
        }

        printf(
            "\nEnter numbers to delete "
            "(example: 2 3), or 'a' to keep first: "
        );

        char input[256];

        if (!fgets(input, sizeof(input), stdin))
            continue;

        if (input[0] == 'a' || input[0] == 'A') {

            for (size_t j = 1;
                 j < group->count;
                 j++) {

                if (unlink(group->paths[j]) == 0) {

                    printf(
                        "Deleted: %s\n",
                        group->paths[j]
                    );

                }
                else {

                    perror(group->paths[j]);
                }
            }

            continue;
        }

        char *ptr = input;

        int selected;

        while (sscanf(ptr, "%d", &selected) == 1) {

            if (selected >= 1 &&
                selected <= (int)group->count) {

                /*
                 * Never delete the first copy.
                 */
                if (selected == 1) {

                    printf(
                        "Keeping first copy: %s\n",
                        group->paths[0]
                    );

                }
                else {

                    const char *file =
                        group->paths[selected - 1];

                    if (unlink(file) == 0) {

                        printf(
                            "Deleted: %s\n",
                            file
                        );

                    }
                    else {

                        perror(file);
                    }
                }
            }

            while (*ptr != ' ' &&
                   *ptr != '\0' &&
                   *ptr != '\n') {
                ptr++;
            }

            while (*ptr == ' ')
                ptr++;
        }
    }
}
