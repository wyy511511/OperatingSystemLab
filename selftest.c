#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    int fd;
    char buf[BUF_SIZE];
    ssize_t nread;
    off_t offset;
    int whence;
    size_t nbytes;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Choose an option (r for read, w for write, s for seek): ");
        char option = getchar();
        while (getchar() != '\n'); // consume remaining input

        switch (option) {
            case 'r':
                printf("Enter the number of bytes you want to read: ");
                scanf("%zu", &nbytes);
                while (getchar() != '\n'); // consume remaining input

                char *buffer = malloc(nbytes);
                if (buffer == NULL) {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }

                nread = fread(buffer, 1, nbytes, fd);
                if (nread == 0) {
                    perror("fread");
                    free(buffer);
                    exit(EXIT_FAILURE);
                }

                printf("%.*s\n", (int) nread, buffer);
                free(buffer);
                break;
            case 'w':
                printf("Enter the string you want to write: ");
                fgets(buf, BUF_SIZE, stdin);
                size_t len = strlen(buf);
                if (buf[len-1] == '\n') {
                    buf[len-1] = '\0';
                }

                ssize_t nwritten = fwrite(buf, 1, len, fd);
                if (nwritten == -1) {
                    perror("fwrite");
                    exit(EXIT_FAILURE);
                }
                break;
            case 's':
                printf("Enter an offset value: ");
                scanf("%lld", &offset);
                while (getchar() != '\n'); // consume remaining input

                printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
                scanf("%d", &whence);
                while (getchar() != '\n'); // consume remaining input

                off_t newpos = lseek(fd, offset, whence);
                if (newpos == -1) {
                    perror("lseek");
                    exit(EXIT_FAILURE);
                }

                // read 0 bytes to manipulate file cursor position
                char tmp_buf[1];
                fread(tmp_buf, 0, 0, fd);
                break;
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
