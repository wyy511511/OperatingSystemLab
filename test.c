#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_PATH "/dev/chardev"

int main(int argc, char *argv[]) {
    // Open the device file for reading and writing
    int device_fd = open(DEVICE_PATH, O_RDWR);

    if (device_fd == -1) {
        printf("Error: failed to open device file\n");
        exit(EXIT_FAILURE);
    }

    // Ask the user whether they want to read or write to the device
    char action;
    printf("Do you want to (r)ead or (w)rite to the device? ");
    getchar();
    scanf("%c", &action);

    if (action == 'r') {
        // Read from the device
        int num_bytes;
        printf("Enter the number of bytes you want to read: ");
        scanf("%d", &num_bytes);

        char *buffer = malloc(num_bytes + 1); // +1 for null terminator

        if (buffer == NULL) {
            printf("Error: failed to allocate buffer\n");
            exit(EXIT_FAILURE);
        }

        ssize_t bytes_read = read(device_fd, buffer, num_bytes);

        if (bytes_read == -1) {
            printf("Error: failed to read from device file\n");
            exit(EXIT_FAILURE);
        }

        buffer[bytes_read] = '\0'; // Null-terminate the buffer

        printf("%s\n", buffer);

        // Free the buffer
        free(buffer);
    } else if (action == 'w') {
        // Write to the device
        printf("Enter the string you want to write: ");

        char input[100];
        fgets(input, sizeof(input), stdin); // Read a line of input from the user

        ssize_t bytes_written = write(device_fd, input, strlen(input));

        if (bytes_written == -1) {
            printf("Error: failed to write to device file\n");
            exit(EXIT_FAILURE);
        } else {
            printf("Successfully wrote %zd bytes to device\n", bytes_written);
        }
    } else {
        // Invalid action
        printf("Error: invalid action\n");
        exit(EXIT_FAILURE);
    }

    // Close the device file
    close(device_fd);

    return 0;
}
