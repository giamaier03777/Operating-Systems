#include <stdio.h>     
#include <stdlib.h>  
#include <unistd.h>    //pt procese si pipe
#include <sys/types.h> 
#include <sys/wait.h>

int main() {
    int pipe1[2];       //array de 2 inturi pt pipe1
    int pipe2[2];       //array de 2 inturi pt pipe2

    //verif crearea pipe-urilor
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe"); //eroare daca nu
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); //creare proces copil 
    if (pid == -1) {    //verif daca a esuat
        perror("fork"); 
        exit(EXIT_FAILURE); 
    }

    if (pid > 0) {      //proc parinte 
        close(pipe1[0]); //inchidere pipe citire pipe1
        close(pipe2[1]); //inchidere pipe scriere pipe2

        int num1, num2; //var nr intro
        printf("Bitte zwei Zahlen angeben: "); 
        scanf("%d %d", &num1, &num2); //citire nr 

        //scriere nr in pipe1
        write(pipe1[1], &num1, sizeof(num1));
        write(pipe1[1], &num2, sizeof(num2));
        close(pipe1[1]); //inchidere capat scriere pipe1
        int sum, product; //var suma si produs
        //citire suma si produs din pipe2
        read(pipe2[0], &sum, sizeof(sum));
        read(pipe2[0], &product, sizeof(product));
        close(pipe2[0]); //inchidere capat citire

        //afis suma si produs 
        printf("Summe: %d\n", sum);
        printf("Produkt: %d\n", product);

        wait(NULL); //asteptare proc copil sa termine 
    } else {       //ramura proces copil 
        close(pipe1[1]); //capat scriere 
        close(pipe2[0]); //capat citire 

        int num1, num2; //var nr citite 
        //citire nr din pipe1
        read(pipe1[0], &num1, sizeof(num1));
        read(pipe1[0], &num2, sizeof(num2));
        close(pipe1[0]); /capat citire pipe1

        int sum = num1 + num2; //suma
        int product = num1 * num2; //produs

        //scriere suma si produs in pipe2
        write(pipe2[1], &sum, sizeof(sum));
        write(pipe2[1], &product, sizeof(product));
        close(pipe2[1]); //scriere inchisa 

        exit(EXIT_SUCCESS); //terminare proc copil 
    }

    return 0; 
}
