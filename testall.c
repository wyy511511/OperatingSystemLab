#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "r+");

    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("Choose an option (r for read, w for write, s for seek): ");
        char option = getchar();
        while (getchar() != '\n'); // consume remaining input

        switch (option) {
            case 'r':
                printf("Enter the number of bytes you want to read: ");
                size_t nbytes;
                scanf("%zu", &nbytes);
                while (getchar() != '\n'); // consume remaining input

                char *buffer = malloc(nbytes + 1);
                if (buffer == NULL) {
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }

                ssize_t nread = read(fileno(fp), buffer, nbytes);
                if (nread == -1) {
                    perror("read");
                } else {
                    buffer[nread] = '\0';
                    printf("%s\n", buffer);
                }

                free(buffer);
                break;

            // case 'r':
            //     printf("Enter the number of bytes you want to read: ");
            //     size_t nbytes;
            //     scanf("%zu", &nbytes);
            //     while (getchar() != '\n'); // consume remaining input

            //     char *buffer = malloc(nbytes);
            //     if (buffer == NULL) {
            //         perror("malloc");
            //         exit(EXIT_FAILURE);
            //     }

            //     size_t nread = fread(buffer, 1, nbytes, fp);
            //     if (nread < nbytes) {
            //         if (feof(fp)) {
            //             printf("End of file reached\n");
            //         } else if (ferror(fp)) {
            //             perror("fread");
            //         }
            //     } else {
            //         printf("%.*s\n", (int) nread, buffer);
            //     }

            //     free(buffer);
            //     break;

            case 'w':
                printf("Enter the string you want to write: ");
                char buf[1024];
                fgets(buf, sizeof(buf), stdin);
                size_t len = strlen(buf);
                if (buf[len-1] == '\n') {
                    buf[len-1] = '\0';
                    len--;
                }

                size_t nwritten = fwrite(buf, 1, len, fp);
                if (nwritten < len) {
                    perror("fwrite");
                }
                break;

            case 's':
                printf("Enter an offset value: ");
                long offset;
                scanf("%ld", &offset);
                while (getchar() != '\n'); // consume remaining input

                printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
                int whence;
                scanf("%d", &whence);
                while (getchar() != '\n'); // consume remaining input

                int res = fseek(fp, offset, whence);
                if (res != 0) {
                    perror("fseek");
                }
                break;

            default:
                continue;
        }

        if (option == EOF) {
            break;
        }
    }

    fclose(fp);

    return EXIT_SUCCESS;
}
