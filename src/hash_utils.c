#include "../include/dupfinder.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE 65536

#define FNV_OFFSET_BASIS 14695981039346656037ULL
#define FNV_PRIME 1099511628211ULL

uint64_t hash_file(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (!file) {
        perror(path);
        return 0;
    }

    unsigned char buffer[BUFFER_SIZE];

    uint64_t hash = FNV_OFFSET_BASIS;

    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {

        for (size_t i = 0; i < bytes_read; i++) {

            hash ^= buffer[i];

            hash *= FNV_PRIME;
        }
    }

    fclose(file);

    return hash;
}

int files_equal(const char *path1, const char *path2)
{
    FILE *f1 = fopen(path1, "rb");
    FILE *f2 = fopen(path2, "rb");

    if (!f1 || !f2) {

        if (f1)
            fclose(f1);

        if (f2)
            fclose(f2);

        return 0;
    }

    unsigned char buffer1[BUFFER_SIZE];
    unsigned char buffer2[BUFFER_SIZE];

    size_t n1;
    size_t n2;

    int equal = 1;

    do {

        n1 = fread(buffer1, 1, BUFFER_SIZE, f1);
        n2 = fread(buffer2, 1, BUFFER_SIZE, f2);

        if (n1 != n2 ||
            memcmp(buffer1, buffer2, n1) != 0) {

            equal = 0;
            break;
        }

    } while (n1 > 0);

    fclose(f1);
    fclose(f2);

    return equal;
}
