/**
 * our solution to question 2, first assignment in OS course.
 *
 * A program which creates a son process, recives string input from the user (<=20)
 * and pipes it to the son process, the son - uses MD5 hash function which was taken from [http://www.zedwood.com/article/cpp-md5-function]
 * on the input string and returns that hash to the father, the father then recives a signal from the son 
 * which activates the handler function called "checkResult" - in which the hash is checked,
 * if its suitable for the MD5 hash - we print it
 * 
 * @author Evgeny Vendrov and David Voihanski
 */

#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <sys/wait.h>
#include <stdlib.h>
#include"md5.h"

static int pipe_[2];
static pid_t pid;

//"checkResult" handler function for SIGINT
void checkResult(int sig){
    if(sig==SIGINT){
    char* result = (char*)malloc(33*sizeof(char));
    uint length = read(pipe_[0], result, 32);
    if(length == 32)
        printf("encrypted by process %d : %s \n",pid,result);
    free(result);
    }
}

int main(){
    int ret;
    pid_t parentPid = getpid();
    ret = pipe(pipe_);
     //this proccess will call this function when catching SIGINT from now
    if(signal(SIGINT, checkResult) == SIG_ERR) {
            printf("\n ERROR---> SIGNAL HANDLER SETTING WENT WRONG! \n");
            exit(EXIT_FAILURE);        

        }
    //in case the pipe failed
    if(ret < 0){
        printf("\n  ERROR---> PIPE DIDNT SUCSEED!  \n");
        exit(EXIT_FAILURE);        
    }
    pid = fork();
    //in case the fork failed
    if(pid < 0){
        printf("\n ERROR---> FORK DIDNT SUCSEED! \n");
        exit(EXIT_FAILURE);        
    }
    //child
    if(pid == 0){
        sleep(1); //wait for parent to write and pause
        char *buf = (char*)malloc(20*sizeof(char));
        uint length = read(pipe_[0], buf, 20);
        std::string str(buf);
        str = md5(str);
        write(pipe_[1],str.c_str() ,str.length());
        kill(parentPid, SIGINT); //send signal that finished 
        free(buf);
        pause();//wait to be killed by parent
    }

    //parent
    else{
        std::string input;
        printf("plain text: ");
        getline(std::cin, input);
        if(input.length() > 20){
            printf("Too long\n");
            exit(EXIT_FAILURE);        
        }
        write(pipe_[1],input.c_str() ,input.length());
        pause();//wait for child to finish
        kill(pid, SIGKILL);//kill child
        exit(EXIT_SUCCESS);
    }
}
