
/**
 * our solution to question 3, first assignment in OS course.
 *
 * A program which creates 5 son processes - each declares a function named "sigCatcher"
 * - to handel SIGINT signals - "sigCatcher" function passes the signal recived
 * to the "next" son process and then exits, then:
 * the father procsess sends a SIGINT signal using kill() to the last son created
 * which uses the sigCatcher to pass the signal to all son processes, until they all are zombies,
 * then the father checks if they all really died prints it - and exits.
 *
 *
 * @author Evgeny Vendrov and David Voihanski
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

//global variable used to indicate which procces am i
int valinum;

//the signal handler for SIGINT
void sigCatcher(int signo)
{
    //checking we indeed recived a SIGINT
    if(signo==SIGINT) {
        //printing the needed data
        printf("PID %d caught one \n", getpid());
        //checking whether this isn't the last processes -
        //if it is we don't need to send another "kill" as all processes are done
        if(valinum!=0) {
            if(kill((getpid()-1),SIGINT)<0) {
                printf("\n ERROR---> KILL {SON: %d} DIDNT SUCSEED! \n",getpid());
                exit(1);
            }
        }
        sleep(1);
        exit(1);
    }
}

int main (void) {
    int stat;
    pid_t deadPid ;
    int indexOfProcess = 0;
    pid_t  pidForked;
    pid_t pidArr[5];
    while(indexOfProcess<5) {
        pidForked=fork();
        //in a case of "fork" error
        if(pidForked<0) {
            printf("\n ERROR---> FORK DIDNT SUCSEED! \n");
            exit(1);
        }
        //if this is fathers process
        else if(pidForked>0) {
            //saving this pid in an array
            pidArr[indexOfProcess]=pidForked;
            indexOfProcess++;
        }
        //if this is sons process
        else {
            valinum=indexOfProcess;
            if (signal(SIGINT, sigCatcher) == SIG_ERR)
                printf("\n ERROR---> SIGNAL CATCHER DIDNT SUCSEED! \n");
            printf("PID %d ready \n",getpid());
            break;
        }
    }

//if im a son process - i want to be stuck in a never ending loop unti i got the signal to exit
    if(pidForked==0) {
        while(1);
    }

//if im the father process - ill sleep to validate that all sons are ready - then send the first signal
//and then wait until all the sons are dead - then ill exit myself
    if(pidForked>0) {
        sleep(1);
        if(kill(pidForked,SIGINT)<0)
            printf("\n ERROR---> KILL {FATHER} DIDNT SUCSEED! \n");
        for(int i=0; i<5; i++) {
            deadPid=waitpid(pidArr[i], &stat, 0);
            printf("%d is dead \n",deadPid);
        }
        exit(1);
    }
}