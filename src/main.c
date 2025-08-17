#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char* argv[]) {
    printf("usage: %s -n -f <filepath>\n", argv[0]);
    printf("\t -n - create a new file\n");
    printf("\t -f - (required) file path\n");
    printf("\t -a - (required) employee info\n");
}

int main(int argc, char *argv[]) {
    bool new_file = false;
    bool add_new = false;
    char* file_path = NULL;
    int db_fd = DB_INVALID_DESCRIPTOR;
    struct dbheader_t* dbh = NULL;
    struct employee_t* employee_data = NULL;
    char* new_employee = NULL;

    int c;
    while ((c = getopt(argc, argv, "a:nf:")) != -1) {
        switch (c) {
            case 'n':
                new_file = true;
                break;
            case 'f':
                file_path = optarg;
                break;
            case 'a':
                new_employee = optarg;
                add_new = true;
                break;
            case '?':
                printf("unknown option %c\n", c);
                break;
            default:
                return EXIT_FAILURE;
        }
    }

    if (file_path == NULL) {
        printf("filepath is required\n");
        print_usage(argv);
        return EXIT_FAILURE;
    }

    if (add_new && new_employee == NULL) {
        printf("employee is required when using -a\n");
        print_usage(argv);
        return EXIT_FAILURE;
    }

    if (new_file) {
        db_fd = create_db_file(file_path);
        if (db_fd == DB_INVALID_DESCRIPTOR) {
            printf("could not create %s\n", file_path);
            return EXIT_FAILURE;
        }

        if (create_db_header(db_fd, &dbh)!=STATUS_SUCCESS) {
            printf("failed to create header\n");
            return EXIT_FAILURE;
        }
    } else {
        db_fd = open_db_file(file_path);
        if (db_fd == DB_INVALID_DESCRIPTOR) {
            printf("could not open %s\n", file_path);
            return EXIT_FAILURE;
        }

        if (validate_db_header(db_fd, &dbh) != STATUS_SUCCESS) {
            printf("Invalid header\n");
            return EXIT_FAILURE;
        };
    }

    if (add_new) {
        dbh->count++;
        employee_data = calloc(dbh->count, sizeof(struct employee_t));
        add_employee(&employee_data[dbh->count-1], new_employee);
        output_file(db_fd, dbh, employee_data);
    }

    close(db_fd);
    free(dbh);

    return EXIT_SUCCESS;
}
