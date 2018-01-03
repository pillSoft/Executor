#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <fcntl.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    //Numero del prossimo file da creare
    int fileNumber=1;
    //Numero di figli creati
    int pidCount = 0;
    //Motivo per cui un processo termina
    int status = 0;
    //Variabile di controllo per la fine dell'inserimento di un comando
    int exit = 0 ;
    //Numero di argomenti del comando
    int argNumber = 0;
    //Comando
    char **command ;
    //Numero di caratteri di un comando
    int countChar;
    //Stringa di appoggio per il salvataggio di un argomento
    char *copy;
    //Array e puntatore utilizzati per leggere il comando e gli agomenti
    char *c;
    char arr[128];

    printf("Inserisci tutti i comandi e premi invio quando hai finito,\nverranno eseguiti in modo sequenziale.\n");

    do {
        countChar = 0;
        exit = 0 ;
        argNumber = 0;
        c = arr;
        //Nome del file da creare
        char out[8] = "out.";
        //Allocazione iniziale
        command = (char **)malloc(sizeof(char **)*(argNumber+1));
        //Lettura di un carattere alla volta del comando
        while((*c = getchar()) == ' ' || *c == EOF);
        if(*c == '\n'){
            break;
        }
        *c++;
        do {
            *c = getchar();
            //Controllo del carattere inserito
            if (*c == ' '){
                if (countChar > 0){//E' stato inserito piu' di un carattere prima di questo
                    *c = '\0';
                    copy = strdup(arr);//Creazione di una copia dell'argomento
                    command = realloc(command,sizeof(char **)*(argNumber+1));//Allocazione del puntatore ad un nuovo argomento
                    command[argNumber] = copy;//Inserimento della copia nel comando
                    c = arr;
                    argNumber++;
                    countChar = 0;
                }
            }else if (*c == '\n') {
                if (countChar > 0) {//E' stato inserito piu' di un carattere prima di questo
                    *c = '\0';
                    copy = strdup(arr);//Creazione di una copia dell'argomento
                    command = realloc(command,sizeof(char **)*(argNumber+2));//Allocazione del puntatore ad un nuovo argomento
                    command[argNumber] = copy;//Inserimento della copia nel comando
                    command[argNumber + 1] = NULL;//Terminazione comando
                    argNumber++;
                }else{
                    //Non ci sono caratteri da salvare, quindi inseriamo solo il terminatore
                    command[argNumber] = NULL;//Terminazione comando
                }
                exit = 1;
            }else {
                countChar++;
                *c++;
            }
        } while (!exit);
        *c = '\0';

        //Composizione del nome del file da creare
        char append[2];
        sprintf(append, "%d", fileNumber);
        strcat(out, append);

        //Creazione del figlio
        int pid = fork();
        if (pid == 0) {
            //Apertura file su cui scrivere
            int fd;
            fd = open(out, O_CREAT | O_TRUNC | O_WRONLY, 0600);
            //Redirezionamento output sul file
            dup2(fd, 1);
            close(fd);
            int resultExec = execvp(command[0], command);
            if (resultExec == -1) {
                //In caso di errore si rimuove il file e si termina
                remove(out);
		        kill(getpid(),SIGKILL);
            }
            perror("main");
        } else {
            //Attendo la terminazione del figlio appena creato
            int res = wait(&status);
            //Controllo stato di esecuzione del figlio
            if(status==0){
                printf("Terminato %d con stato %d \n",res,status);
                fileNumber++;
            }
            else {
                printf("Terminato involontariamente %d con stato %d \n",res,status);
            }
        }

        //Deallocazione della memoria
	    int i;
        for(i = 0; i < argNumber; i++) free(command[i]);
        free(command);

    } while (1);
    return 0;
}
 
