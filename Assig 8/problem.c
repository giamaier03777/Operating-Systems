#include <stdio.h>     
#include <stdlib.h>    
#include <unistd.h>    
#include <string.h>    
#include <sys/wait.h> 
#include <fcntl.h>     //pt controlul fisierelor
#include <sys/stat.h>  

#define MAX_BUF 30     
#define FIFO1 "fifo1" 
#define FIFO2 "fifo2" 
#define FIFO3 "fifo3"  

//process1:citeste din intrarea standard si scrie in FIFO1
//process2:citeste din FIFO1, inlocuieste cifrele cu 9 și scrie în FIFO2
//process3:citeste din FIFO2, inlocuieste literele cu 0 si scrie în FIFO3
//process4:citeste din FIFO3, sorteaza liniile și le afiseaza


//procesul 1
void process1(const char* fifo_write) {
    char buffer[MAX_BUF]; //buffer pt citirea datelor
    ssize_t bytes_read;   //pt octeti cititi
    int fd_write;         //pt scriere

    printf("Entered process1\n"); 

    fd_write = open(fifo_write, O_WRONLY); //FIFO pt scriere
    if (fd_write == -1) { // verif daca nu s-a deschis
        perror("Open fifo1 for writing"); 
        exit(EXIT_FAILURE); //iesire
    }

    //citire din intrare
    while ((bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1)) > 0) {
//cat citirea returneaza un nr de octeti adica exista date de citit, progr ruleaza
        buffer[bytes_read] = '\0'; //termina de citit sirul
        write(fd_write, buffer, bytes_read); //scriere in FIFO
    }

    close(fd_write); 

    printf("Exited process1\n"); 
}

//procesul 2
void process2(const char* fifo_read, const char* fifo_write) {
    char buffer[MAX_BUF]; //citire date
    ssize_t bytes_read;   //nr octeti cititi 
    int fd_read, fd_write; //descriptori pt citire si scriere 

    printf("Entered process2\n"); 

    fd_read = open(fifo_read, O_RDONLY); //FIFO pt citire
    if (fd_read == -1) { //verif deschiderera
        perror("Open fifo1 for reading"); 
        exit(EXIT_FAILURE); 
    }

    fd_write = open(fifo_write, O_WRONLY); //FIFO pt scriere
    if (fd_write == -1) { //verif deschiderera
        perror("Open fifo2 for writing"); 
        exit(EXIT_FAILURE); 
    }

    //citire din FIFO
    while ((bytes_read = read(fd_read, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; //termina de citit
        //inlocuire cifre cu 9
        for (int i = 0; i < bytes_read; i++) {
            if (buffer[i] >= '0' && buffer[i] <= '9') {
                buffer[i] = '9'; 
            }
        }
        write(fd_write, buffer, bytes_read); //scrie in FIFO bufferul modif
    }

    close(fd_read);  // inchidere pt citire
    close(fd_write); //inchidere pt scriere

    printf("Exited process2\n"); 
}

//procesul 3
void process3(const char* fifo_read, const char* fifo_write) {	//de unde citeste si unde scrie
    char buffer[MAX_BUF]; //buffer pt citire date 
    ssize_t bytes_read;   //ptocteti cititi 
    int fd_read, fd_write; //dscriptori pt citire si scriere 

    printf("Entered process3\n"); 

    fd_read = open(fifo_read, O_RDONLY); //FIFO pt citire
    if (fd_read == -1) { 
        perror("Open fifo2 for reading"); 
        exit(EXIT_FAILURE);
    }

    fd_write = open(fifo_write, O_WRONLY); //FIFO pt scriere
    if (fd_write == -1) { 
        perror("Open fifo3 for writing"); 
        exit(EXIT_FAILURE); 
    }

    //citire din FIFO
    while ((bytes_read = read(fd_read, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0'; 
        //inlocuire litere cu 0
        for (int i = 0; i < bytes_read; i++) {
            if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z')) {
                buffer[i] = '0';
            }
        }
        write(fd_write, buffer, bytes_read); //scrie bufferul modif in FIFO
    }

    close(fd_read); 
    close(fd_write); 

    printf("Exited process3\n"); 
}

//procesul 4
void process4(const char* fifo_read) {
    char buffer[MAX_BUF]; //buffer citire date
    char results[MAX_BUF][MAX_BUF]; //vector pt stocarea rez
    int num_results = 0; //contor pt nr de rez 
    int fd_read; //descriptor pt fisier citire 

    printf("Entered process4\n"); 

    fd_read = open(fifo_read, O_RDONLY); //FIFO pt citire
    if (fd_read == -1) {
        perror("Open fifo3 for reading"); 
        exit(EXIT_FAILURE); 
    }

    //citire din FIFO
    while (1) {
        int bytes_read = read(fd_read, buffer, sizeof(buffer) - 1); //citire date din FIFO
        if (bytes_read <= 0) { //cand s-a term citirea
            break; //iesire din bucla
        }
        buffer[bytes_read] = '\0'; /
        strcpy(results[num_results], buffer); //copiere buffer in vect cu rez
        num_results++; //incrementare nr rez 
    }

    close(fd_read);

    //sortare rezultate- ca BubbleSort- pt scrierea datelor in ordine corecta 
    for (int i = 0; i < num_results - 1; i++) {
        for (int j = 0; j < num_results - i - 1; j++) {
            if (strcmp(results[j], results[j + 1]) > 0) {
                char temp[MAX_BUF]; //buffer temporar
                strcpy(temp, results[j]); //copiaza primul sir in buffer
                strcpy(results[j], results[j + 1]); //al doiela sir
                strcpy(results[j + 1], temp); //buffer in sir2
            }
        }
    }

    //rezultate sortate
    for (int i = 0; i < num_results; i++) {
        printf("%s", results[i]); //afis fiecare rez
    }

    printf("Exited process4\n");
}

int main() {
    //creare FIFO-uri
    mkfifo(FIFO1, 0666); //FIFO cu permisiune de citire/scriere
    mkfifo(FIFO2, 0666); //FIFO2 same
    mkfifo(FIFO3, 0666); //FIFO3 same 

    pid_t pid1, pid2, pid3, pid4; //var pt identificatorii de procese
    pid1 = fork(); // proces copil1

    if (pid1 == 0) { //daca e in el
        process1(FIFO1); //se ruleaza
        exit(EXIT_SUCCESS); //iesire din proces 
    } else {
        pid2 = fork(); //proces copil2

        if (pid2 == 0) { //daca e in 2
            process2(FIFO1, FIFO2); //rulare
            exit(EXIT_SUCCESS); //iesire
        } else {
            pid3 = fork(); //proces copil 3

            if (pid3 == 0) { 
                process3(FIFO2, FIFO3);
                exit(EXIT_SUCCESS);
            } else {
                pid4 = fork(); //proces copil 4 

                if (pid4 == 0) { 
                    process4(FIFO3); 
                    exit(EXIT_SUCCESS);
                }
            }
        }
    }

    //asteptare finalizare proc copil 
    waitpid(pid1, NULL, 0); /proces copil 1
    waitpid(pid2, NULL, 0); //2
    waitpid(pid3, NULL, 0); //3
    waitpid(pid4, NULL, 0); //4

    //stergere FIFO-uri
    unlink(FIFO1); //1
    unlink(FIFO2); //2
    unlink(FIFO3); //3

    return 0;
}
