#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define FILE_PATH "zahl.txt"
#define START_VALUE 500
#define MIN_VALUE 0
#define MAX_VALUE 1000

void lock_file(int fd) {
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    while (fcntl(fd, F_SETLKW, &lock) == -1) {
        if (errno != EINTR) {
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
    }
}

void unlock_file(int fd) {
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

void process_increment() {
    int operations = 0;
    while (1) {
        int fd = open(FILE_PATH, O_RDWR);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        lock_file(fd);

        FILE *file = fdopen(fd, "r+");
        if (file == NULL) {
            perror("fdopen");
            close(fd);
            exit(EXIT_FAILURE);
        }

        int number;
        if (fscanf(file, "%d", &number) != 1) {
            perror("fscanf");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        if (number >= MAX_VALUE) {
            fclose(file);
            unlock_file(fd);
            close(fd);
            break;
        }

        number++;
        rewind(file);
        if (fprintf(file, "%d\n", number) < 0) {
            perror("fprintf");
            fclose(file);
            unlock_file(fd);
            close(fd);
            exit(EXIT_FAILURE);
        }

        fflush(file);
        operations++;
        fclose(file);
        unlock_file(fd);
        close(fd);
        usleep(100000);  // Verlangsamt den Prozess, um das Sperren zu demonstrieren
    }
    printf("Increment operations: %d\n", operations);
}

void process_decrement() {
    int operations = 0;
    while (1) {
        int fd = open(FILE_PATH, O_RDWR);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        lock_file(fd);

        FILE *file = fdopen(fd, "r+");
        if (file == NULL) {
            perror("fdopen");
            close(fd);
            exit(EXIT_FAILURE);
        }

        int number;
        if (fscanf(file, "%d", &number) != 1) {
            perror("fscanf");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        if (number <= MIN_VALUE) {
            fclose(file);
            unlock_file(fd);
            close(fd);
            break;
        }

        number--;
        rewind(file);
        if (fprintf(file, "%d\n", number) < 0) {
            perror("fprintf");
            fclose(file);
            unlock_file(fd);
            close(fd);
            exit(EXIT_FAILURE);
        }

        fflush(file);
        operations++;
        fclose(file);
        unlock_file(fd);
        close(fd);
        usleep(100000);  // Verlangsamt den Prozess, um das Sperren zu demonstrieren
    }
    printf("Decrement operations: %d\n", operations);
}

int main() {
    // Initialisiere die Datei mit dem Startwert
    FILE *file = fopen(FILE_PATH, "w");
    if (!file) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    fprintf(file, "%d\n", START_VALUE);
    fclose(file);

    int num_incrementers = 2;
    int num_decrementers = 5;
    pid_t pid;

    // Starte Inkrementierer
    for (int i = 0; i < num_incrementers; i++) {
        if ((pid = fork()) == 0) {
            process_increment();
            exit(EXIT_SUCCESS);
        } else if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        }
    }

    // Starte Dekrementierer
    for (int i = 0; i < num_decrementers; i++) {
        if ((pid = fork()) == 0) {
            process_decrement();
            exit(EXIT_SUCCESS);
        } else if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        }
    }

    // Warten auf alle Kindprozesse
    while (wait(NULL) > 0);
    return EXIT_SUCCESS;
}
