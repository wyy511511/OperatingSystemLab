#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void print_error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

void print_prompt(const char *msg) {
    printf("%s", msg);
    fflush(stdout);
}

int read_int() {
    int value;
    if (scanf("%d", &value) != 1) {
        print_error("Invalid input");
    }
    return value;
}

void read_string(char *buffer, size_t size) {
    fgets(buffer, size, stdin);
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_error("Usage: ./program <file>");
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        print_error(strerror(errno));
    }

    while (1) {
        print_prompt("Choose an option (r for read, w for write, s for seek): ");
        char option;
        read_string(&option, 1);

        switch (option) {
            case 'r': {
                print_prompt("Enter the number of bytes you want to read: ");
                int nbytes = read_int();

                char *buffer = malloc(nbytes + 1);
                if (buffer == NULL) {
                    print_error(strerror(errno));
                }

                ssize_t nread = pread(fd, buffer, nbytes, lseek(fd, 0, SEEK_CUR));
                if (nread == -1) {
                    print_error(strerror(errno));
                }

                buffer[nread] = '\0';
                printf("%s\n", buffer);

                free(buffer);
                break;
            }

            case 'w': {
                print_prompt("Enter the string you want to write: ");
                char buffer[BUFFER_SIZE];
                read_string(buffer, BUFFER_SIZE);

                ssize_t nwritten = pwrite(fd, buffer, strlen(buffer), lseek(fd, 0, SEEK_CUR));
                if (nwritten == -1) {
                    print_error(strerror(errno));
                }
                break;
            }

            case 's': {
                print_prompt("Enter an offset value: ");
                int offset = read_int();

                print_prompt("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
                int whence = read_int();

                off_t newpos = lseek(fd, offset, whence);
                if (newpos == -1) {
                    print_error(strerror(errno));
                }
                break;
            }

            default:
                continue;
        }

        if (option == EOF) {
            break;
        }
    }

    close(fd);

    return EXIT_SUCCESS;
}

