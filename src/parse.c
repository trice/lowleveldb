#include "parse.h"
#include "common.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

int create_db_header(int fd, struct dbheader_t **headerOut) {
    struct dbheader_t* db_header = calloc(1, sizeof(struct dbheader_t));
    if (db_header == NULL) {
        printf("DB header allocation failed\n");
        return STATUS_FAILURE;
    }

    db_header->version = 0x1;
    db_header->magic = HEADER_MAGIC;
    db_header->count = 0;
    db_header->filesize = sizeof(struct dbheader_t);

    *headerOut = db_header;

    return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
    if (fd < 0) {
        printf("invalid file descriptor\n");
        return STATUS_FAILURE;
    }

    struct dbheader_t* db_header = calloc(1, sizeof(struct dbheader_t));
    if (db_header == NULL) {
        printf("DB header allocation failed\n");
        return STATUS_FAILURE;
    }

    struct stat stat_buf = {0};
    fstat(fd, &stat_buf);

    ssize_t read_result = read(fd, db_header, sizeof(struct dbheader_t));
    if (read_result <=0) {
        perror("read");
        free(db_header);
        return STATUS_FAILURE;
    }

    db_header->magic = ntohl(db_header->magic);
    db_header->count = ntohs(db_header->count);
    db_header->version = ntohs(db_header->version);
    db_header->filesize = ntohl(db_header->filesize);

    int result = 0;
    result |= db_header->magic == HEADER_MAGIC;
    result &= db_header->version == 1;

    if (!result) {
        return STATUS_FAILURE;
    }

    *headerOut = db_header;
    return STATUS_SUCCESS;
}


int read_employees(int fd, struct dbheader_t *dbh, struct employee_t **employeesOut) {
    if (fd < 0) {
        printf("invalid file descriptor\n");
        return STATUS_FAILURE;
    }

    lseek(fd, 0, SEEK_SET);
    read(fd, dbh, sizeof(struct dbheader_t));

    dbh->magic = ntohl(dbh->magic);
    dbh->count = ntohs(dbh->count);
    dbh->version = ntohs(dbh->version);
    dbh->filesize = ntohl(dbh->filesize);

    for (int i=0; i < dbh->count; i++) {
        read(fd, &employeesOut[i], sizeof(struct employee_t));
        employeesOut[i]->hours = ntohs(employeesOut[i]->hours);
    }

    return STATUS_SUCCESS;
}


int output_file(int fd, struct dbheader_t *dbh, struct employee_t *employees) {
    if (fd < 0) {
        printf("invalid file descriptor\n");
        return STATUS_FAILURE;
    }

    if (!dbh) {
        printf("invalid header\n");
        return STATUS_FAILURE;
    }

    int local_count = dbh->count;
    dbh->magic = htonl(dbh->magic);
    dbh->version = htons(dbh->version);
    dbh->count = htons(dbh->count);
    dbh->filesize = htonl(sizeof(struct dbheader_t)) + (sizeof(struct employee_t) * dbh->count);

    lseek(fd, 0, SEEK_SET);
    write(fd, dbh, sizeof(struct dbheader_t));

    for (int i=0; i < local_count; i++) {
        employees[i].hours = htonl(employees[i].hours);
        write(fd, &employees[i], sizeof(struct employee_t));
    }

    return STATUS_SUCCESS;
}

// Sending a single employee to the function, with main providing the current employee.
// Omitting dbheader_t to avoid unnecessary dependencies for this function's purpose.
int add_employee(struct employee_t *employee, char* employee_data) {
    printf("%s\n", employee_data);
    char* name = strtok(employee_data, ",");
    char* addr = strtok(NULL, ",");
    char* hours = strtok(NULL, ",");

    printf("%s %s %s\n", name, addr, hours);

    strncpy(employee->name, name, sizeof(employee->name));
    strncpy(employee->address, addr, sizeof(employee->address));
    employee->hours = atoi(hours);

    return STATUS_SUCCESS;
}
