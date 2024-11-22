#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Verifică dacă numărul de argumente este diferit de 2 (numele programului + numele fișierului)
    if (argc != 2) {
        fprintf(stderr, "Verwendung: %s <Dateiname>\n", argv[0]);
        return 1;
    }
    
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Fehler beim Öffnen der Datei");
        return 1;
    }
    
    FILE *temp_file = tmpfile();	
    if (temp_file == NULL) {	
        perror("Fehler beim Erstellen der temporären Datei");
        fclose(input_file);	
        return 1;	 
    }
    
    int position = 0;
    int byte;	//stocam un byte citit 
    while ((byte = fgetc(input_file)) != EOF) {	//ruleaza pana la finalul fisierului
        if (position % 2 == 0) {  
            fputc(byte, temp_file);
        }
        position++;	
    }
    
    fclose(input_file);	
    rewind(temp_file);	//repozitionare la inc de fisier temporar 
    
//deschidere fisier orig pt suprascriere -> stergere
    input_file = fopen(argv[1], "w");
    if (input_file == NULL) {
        perror("Fehler beim Öffnen der Datei");	 
        fclose(temp_file);	
        return 1;
    }
    
    //copiaza cont fisier temp in fisier orig
    while ((byte = fgetc(temp_file)) != EOF) {	 
        fputc(byte, input_file);	//din cel temp in cel orig
    }
    
    fclose(input_file);	
    fclose(temp_file);	 
    
    return 0;
}
