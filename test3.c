#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    // Open the file for reading and writing
    FILE *file = fopen("example.txt", "r+");

    if (file == NULL) {
        printf("Error: failed to open file\n");
        exit(EXIT_FAILURE);
    }

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

    // Write to the file
    printf("Enter the string you want to write: ");

    char input[100];
    fgets(input, sizeof(input), stdin); // Read a line of input from the user
    fputs(input, file);

    // Seek in the file
    int offset, whence;
    printf("Enter an offset value: ");
    scanf("%d", &offset);

    printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
    scanf("%d", &whence);

    fseek(file, offset, whence);

    // Close the file
    fclose(file);

    return 0;
}

