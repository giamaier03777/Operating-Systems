#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    int *numbers;
    int n;
    int result;
} GroupData;

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

void parallel_gcd_group(int *numbers, int n) {
    int group_count = 3; //3 grupuri
    int group_size = n / group_count; //dim fiecarui grup
    int remaining = n % group_count; //nr eleml ramase

    int start_index = 0;

    for (int i = 0; i < group_count; ++i) {	//un proces copil pt fiecare grup 
        pid_t pid = fork(); 

        if (pid == 0) {
            int end_index = start_index + group_size;
            if (i < remaining) {
                end_index++; //daca mai sunt elem ramase le adaug la dim grup
            }

            GroupData group_data;
            group_data.n = end_index - start_index;	//calc nr de elem din grup 
            group_data.numbers = &numbers[start_index];
            group_data.result = group_data.numbers[0];	//rez cu primul nr din grup 

            for (int j = 1; j < group_data.n; ++j) {
                group_data.result = gcd(group_data.result, group_data.numbers[j]);	//calculam ggt pt fiecare grup 
            }

            exit(0); //terminare proc copil 
        } else { //procesul parinte
            start_index += group_size;
            if (i < remaining) {
                start_index++; //actualizare index start pt urm grup 
            }
        }
    }

    for (int i = 0; i < group_count; ++i) {	//term toate proc copil
        wait(NULL);
    }
}

int main() {
    int n;
    printf("Introduceti numarul de numere: ");
    scanf("%d", &n); 

    int *numbers = (int *)malloc(n * sizeof(int)); //alocare memorie
    printf("Introduceți cele %d numere:\n", n); 
    for (int i = 0; i < n; ++i) {
        scanf("%d", &numbers[i]); //citire nr intro 
    }

    parallel_gcd_group(numbers, n); //calcularea gcd pt fiecare grup 

    free(numbers); //elib memorie 
    return 0; 
}
