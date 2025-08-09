#include "file.h"
#include "common.h"
#include <stdbool.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


// Private helper
bool file_exists(char* filename) {
    int fd = open(filename, O_RDONLY);
    bool result = fd != -1;
    close(fd);
    return result;
}

int open_db_file(char* filename) {
    if (!file_exists(filename)) {
        printf("file %s was not found\n", filename);
        return STATUS_FAILURE;
    }

    int fd = open(filename, O_RDWR);
    return fd;
}

int create_db_file(char *filename) {
    if (file_exists(filename)) {
        printf("filename %s already exits\n", filename);
        return STATUS_FAILURE;
    }

    int fd = open(filename, O_RDWR | O_CREAT, 0644);
    return fd;
}
