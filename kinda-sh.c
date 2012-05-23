#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "tokenizer.h"


#define MAX_LINE 256 /* 1024 chars per line, per command, should be enough*/
int success,j,j1=0,j2=0;	/*success message flag*/
pid_t pid;
  TOKENIZER *tokenizer;
  char *tok;

void redirkill (){
	if(j1>1 || j2>1 || j1+j2 > 2){
		perror("Invalid redirections");
		exit(EXIT_FAILURE);
	}
}

void
redir (const char * tok, TOKENIZER *tokenizer, char **argv, int i)
{
	if((char)tok[0]=='>'){
	j1++;
	redirkill();
	tok = get_next_token(tokenizer);
	int pFile = open(tok, O_WRONLY | O_CREAT, 0644);
	dup2(pFile,STDOUT_FILENO);
	write(STDOUT_FILENO, "Hello Wolrd\n",15);
	}
	if((char)tok[0]=='<'){
	j2++;
	redirkill();
	tok = get_next_token(tokenizer);
	argv[1]=tok;
	i++;
	}
}

void
pipewrench(const char * tok, TOKENIZER *tokenizer, char **argv, int i){
	printf("pipewrench; i = '%d'\n",i);
	for(j=0;j<i+1;j++){
	printf("%d,%s==D ",j,argv[j]);}
	//argv[i]='\0';
	int pfd[2];
	pid_t cpid;
	char buf;
	if (pipe(pfd) == -1) {perror("pipe error");exit(EXIT_FAILURE);}
	printf("\n\n");
	cpid = fork();
	if (cpid == -1) {perror("fork error");exit(EXIT_FAILURE);}

	if (cpid == 0) {
	printf("Child\n");
	close(pfd[0]);   //close read
	dup2(pfd[1],STDOUT_FILENO);
	
	//argv[i]='\0';
	execvp(argv[i],argv);//*/
	close(pfd[1]);
	}else{
	printf("Parent\n");
	close(pfd[1]); //close write
	dup2(STDIN_FILENO,pfd[0]);
/*do something*/
	tok = get_next_token(tokenizer);
	execvp(tok,argv);
	wait(NULL);
close(pfd[0]);
	}
//i++;
}

int main(int argc, char **argv/*, char **envp*/)
{
	int length,
	secs,
	i;

	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */

    while (1){

	/*prompt user*/
	write(STDIN_FILENO, "RENA$ ", 6);
	length = read(STDIN_FILENO, inputBuffer, 255);

	/* ^d was entered, end of user command stream */
	if (length == 0)
		exit(0);

	/* terminate with error code of -1 */
	if (length < 0){
		perror("error reading the command");
		exit(-1);
	}

	pid = fork();
	if (!pid) {
		
		i =0;

		inputBuffer[length-1] = '\0';
		tokenizer = init_tokenizer(inputBuffer);
		i=0;

		//////////////
		//TOKEN LOOP//

		while( (tok = get_next_token(tokenizer)) != NULL ) {
		argv[i]=tok;
		if((char)tok[0]=='>' || (char)tok[0]=='<'){
			argv[i]='\0';
			redir(tok,tokenizer,argv,i);			
		}
		if((char)tok[0]=='|'){
			pipewrench(tok,tokenizer,argv,i);
		}
		i++;
		}
		argv[i]='\0';

		success = execvp (argv[0],argv);

		/* terminate with error code of -1 */
		perror( "Error " );
		exit(-1);
	}
	else {
		wait();
	}
    }
}
