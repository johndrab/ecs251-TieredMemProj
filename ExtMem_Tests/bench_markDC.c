#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define FILENAME "large_file.dat"
#define FILE_SIZE (5ULL * 1024 * 1024 * 1024) // 5GB
#define CHUNK_SIZE 4096

void write_to_file(int fd, size_t size) {
    char *buffer = (char *)malloc(CHUNK_SIZE);
    if (!buffer) {
        perror("Failed to allocate memory for buffer");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i += CHUNK_SIZE) {
        for (size_t j = 0; j < CHUNK_SIZE; j++) {
            buffer[j] = rand() % 256; //Fill with rand values 
        }
        ssize_t written = write(fd, buffer, CHUNK_SIZE); // file write
        if (written != CHUNK_SIZE) {
            perror("Write error");
            free(buffer);
            exit(EXIT_FAILURE);
        }
    }

    free(buffer);
}


void read_from_file(int fd, size_t size) {
    char *buffer = (char *)malloc(CHUNK_SIZE);
    if (!buffer) {
        perror("Failed to allocate memory for buffer");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; i += CHUNK_SIZE) {
        ssize_t read_bytes = read(fd, buffer, CHUNK_SIZE);
        if (read_bytes != CHUNK_SIZE) {
            if (read_bytes == -1) {
                perror("Read error");
            }
            break;
        }
    }

    free(buffer);
}


void random_read_write(int fd, size_t size, int num_operations) {
    srand(time(NULL)); //Rand generator setup
    char *write_buffer = (char *)malloc(CHUNK_SIZE);
    char *read_buffer = (char *)malloc(CHUNK_SIZE);

    if (!write_buffer || !read_buffer) {
        perror("Failed to allocate memory for buffers");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_operations; i++) {
        off_t offset = (rand() % (size / CHUNK_SIZE)) * CHUNK_SIZE;
        off_t read_offset = (rand() % (size / CHUNK_SIZE)) * CHUNK_SIZE;

        for (size_t j = 0; j < CHUNK_SIZE; j++) {
            write_buffer[j] = rand() % 256;
        }

        lseek(fd, offset, SEEK_SET); //Move location in file using the randomized offet
        ssize_t written = write(fd, write_buffer, CHUNK_SIZE);
        if (written != CHUNK_SIZE) {
            perror("Write error");
            free(write_buffer);
            free(read_buffer);
            exit(EXIT_FAILURE);
        }

        //Random read operation
        lseek(fd, read_offset, SEEK_SET);
        ssize_t read_bytes = read(fd, read_buffer, CHUNK_SIZE);
        if (read_bytes != CHUNK_SIZE) {
            if (read_bytes == -1) {
                perror("Read error");
            }
        }
    }

    free(write_buffer);
    free(read_buffer);
}

int main() {
    int fd = open(FILENAME, O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    // Write large amount of data to the file
    printf("Writing %llu bytes to file %s...\n", FILE_SIZE, FILENAME);
    write_to_file(fd, FILE_SIZE);

    // Perform random read/write operations
    printf("Performing random read/write operations...\n");
    random_read_write(fd, FILE_SIZE, 1000);

    // Perform reads on data from the file
    printf("Reading data from file...\n");
    lseek(fd, 0, SEEK_SET);
    read_from_file(fd, FILE_SIZE);

    close(fd);
}

