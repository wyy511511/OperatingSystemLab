#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Prompt the user to enter the file path
    char file_path[100];
    printf("Enter the file path: ");
    scanf("%s", file_path);

    // Open the file for reading and writing
    FILE *file = fopen(file_path, "r+");

    if (file == NULL) {
        printf("Error: failed to open file\n");
        exit(EXIT_FAILURE);
    }

    // Main program loop
    while (1) {
        // Prompt the user to choose an operation
        char operation;
        printf("Enter 'r' to read from the file, 'w' to write to the file, 's' to seek in the file, or Ctrl+D to exit: ");
        scanf(" %c", &operation);

        switch (operation) {
            case 'r': {
                // Read from the file
                int num_bytes;
                printf("Enter the number of bytes you want to read: ");
                scanf("%d", &num_bytes);

                char *buffer = malloc(num_bytes + 1); // +1 for null terminator

                if (buffer == NULL) {
                    printf("Error: failed to allocate buffer\n");
                    exit(EXIT_FAILURE);
                }

                size_t bytes_read = fread(buffer, 1, num_bytes, file);

                if (bytes_read == 0) {
                    printf("Error: failed to read from file\n");
                    exit(EXIT_FAILURE);
                }

                buffer[bytes_read] = '\0'; // Null-terminate the buffer

                printf("%s\n", buffer);

                free(buffer);
                break;
            }

            case 'w': {
                // Write to the file
                printf("Enter the string you want to write: ");

                char input[100];
                fgets(input, sizeof(input), stdin); // Read a line of input from the user
                fputs(input, file);

                fflush(file);
                break;
            }

            case 's': {
                // Seek in the file
                int offset, whence;
                printf("Enter an offset value: ");
                scanf("%d", &offset);

                printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
                scanf("%d", &whence);

                fseek(file, offset, whence);
                break;
            }

            case EOF: // Ctrl+D was pressed
                printf("\n");
                fclose(file);
                return 0;

            default:
                printf("Invalid input, please try again\n");
                break;
        }
    }
}

