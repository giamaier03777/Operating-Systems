#include <stdio.h>     
#include <stdlib.h>   
#include <unistd.h>     
#include <dirent.h>   
#include <string.h>   
#include <sys/types.h> 
#include <sys/stat.h> 

#define BUFFER_SIZE 1024   //pt citirea datelor

int main() {
    int pipefd[2];    
    pid_t pid;              

    if (pipe(pipefd) == -1) {   //daca fct a fost eplata cu succes 
        perror("pipe");         //daca nu, eroare 
        exit(EXIT_FAILURE);   
    }

    pid = fork();               //proces copil

    if (pid == -1) {       
        perror("fork");       
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {             //pt procesul copil (server)
        close(pipefd[1]);       //inchidere scriere nefol
        char directory_name[BUFFER_SIZE];  //sir de caract ce va stoca numele directorului
        read(pipefd[0], directory_name, BUFFER_SIZE);  //citire nume director din pipe
        close(pipefd[0]);       //inchide citire nef

        DIR *dir = opendir(directory_name); //deschidere director
        if (dir == NULL) {     
            perror("opendir");  
            exit(EXIT_FAILURE); 
        }

        struct dirent *entry;   //struct care va stoca informatii despre fis din director
        int found = 0;          //daca exista .txt
        while ((entry = readdir(dir)) != NULL) {  //parcurgere fisier din director
            if (entry->d_type == DT_REG) {  //verificarea daca elementul curent e un fisier obis
                //gasirea pozitiei ultimei aparitii a '.' in nume 
                char *dot_position = strrchr(entry->d_name, '.');
                if (dot_position != NULL) {  
                    if (strcmp(dot_position, ".txt") == 0) { 
                        printf("%s\n", entry->d_name);  //afis nume fisier 
                        found = 1; 
                    }
                }
            }
        }
        closedir(dir);  

        if (!found) {
            fprintf(stderr, "Nu au fost găsite fișiere .txt în director.\n");
        }
    } else { 
        close(pipefd[0]);  
        char directory_name[BUFFER_SIZE];  
        printf("Introduceți numele directorului: ");  
        fgets(directory_name, BUFFER_SIZE, stdin);    //citirea nume director 
        directory_name[strcspn(directory_name, "\n")] = 0;  //eliminare caracter newline
        write(pipefd[1], directory_name, strlen(directory_name) + 1);  //scriere nume in pipe
        close(pipefd[1]);  
    }

    return 0;  
}
