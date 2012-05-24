/*TODO\\\
refactor execvp out of main into the redir and pipewrench function calls
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "tokenizer.h"


#define MAX_LINE 256 /* 1024 chars per line, per command, should be enough*/
int success,j,jay1=0,j2=0;	/*success message flag*/
pid_t pid;
TOKENIZER *tokenizer;
char *tok;

void redirkill () {
	if(jay1>1 || j2>1 || jay1+j2 > 2){
//		perror("Invalid redirections");
		exit(EXIT_FAILURE);
	}
}

void redir (char * tok, TOKENIZER *tokenizer, char **argv, int i) {

	printf("redir; i = '%d'\n",i);
	if((char)tok[0]=='>') {
			for(j=0;j<6;j++){
			printf("redir argv[%d], %s;\n",j,argv[j]);}
		jay1++;
		redirkill();
		tok = get_next_token(tokenizer);
		int pFile = open(tok, O_WRONLY | O_CREAT, 0644);
		dup2(pFile,STDOUT_FILENO);
//		write(STDOUT_FILENO, "Hello World\n",15);
	}
/*	if((char)tok[0]=='<') {
		j2++;
		redirkill();
		tok = get_next_token(tokenizer);
		argv[1]=tok;
		i++;
	}//*/
}

void pipewrench(char * tok, TOKENIZER *tokenizer, char **argv, char **derpv, int i){
	printf("pipewrench; i = '%d'\n",i);//debug
//	char **derpv;
	int x;
	j=0;

	/*derpv construct
	while( (tok = get_next_token(tokenizer)) != NULL ) {
	printf("pipewrench; j = '%d'\n",j);//debug
	derpv[j]=tok; j++;}//*/

	for(x=0;x<4;x++){printf("derpv[%d] = %s;\n",x,derpv[x]);}
	derpv[x]='\0';
	printf("s\n\n");
	for(j=0;j<i;j++){printf("argv[%d] = %s;\n",j,argv[j]);}

	int pipefd[2];
	pid_t cpid;
	if (pipe(pipefd) == -1) {perror("pipe error");exit(EXIT_FAILURE);}
	printf("\n\n");
	if ((cpid = fork()) == -1) {perror("fork error");exit(EXIT_FAILURE);}
	if (cpid == 0) {
		printf("child\n\n");
//		close(pipefd[0]);
		dup2(pipefd[0],0);
		close(pipefd[1]);
//		dup2(pipefd[1],STDIN_FILENO);
//		dup2(pipefd[0],STDIN_FILENO);
//		printf("child out: %d in: %d\n", STDOUT_FILENO, STDIN_FILENO);
		execvp(derpv[0],derpv);
		printf("end child\n\n");
//		exit(0);
	}
	else {
		printf("parent\n\n");
//		close(pipefd[1]);
		dup2(pipefd[1],1);
		close(pipefd[1]);
//		dup2(1,pipefd[1]);
//		dup2(pipefd[0],STDOUT_FILENO);
//		printf("parent out: %d in: %d\n", STDOUT_FILENO, STDIN_FILENO);
		execvp(argv[0],argv);
		printf("end parent\n\n");
	}

}

int main(int argc, char **argv/*, char **envp*/) {

	int length, i;
	char **derpv;
        derpv = malloc(sizeof(char*) * 20);
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */

	while (1) {

		/*prompt user*/
		write(STDIN_FILENO, "REINA$ ", 7);
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
	
			inputBuffer[length-1] = '\0';
			tokenizer = init_tokenizer(inputBuffer);
			i=0;
			char **current=argv;	

			//////////////
			//TOKEN LOOP//
	
			while( (tok = get_next_token(tokenizer)) != NULL ) {
				///*
				current[i]=tok;
				i++;

				//PIPE
				if((char)tok[0]=='|'){
					printf("pipe detected\n");
					current[i-1]=NULL;
					current=derpv;
					i=0;
				}

				if((char)tok[0]=='<'){
					printf("< redirection\n");
					argv[i-1]='\0';
					j2++;
					redirkill();
					tok = get_next_token(tokenizer);
					current[1]=tok;
					i++;
				}

				if((char)tok[0]=='>') {
					printf("> redirection\n");
					argv[i-1]='\0';
					redir(tok,tokenizer,argv,i);			
				}
				//argv[i]='\0';//*/

				/*
				argv[i]=tok;
				if((char)tok[0]=='>' || (char)tok[0]=='<') {
					argv[i]='\0';
					redir(tok,tokenizer,argv,i);			
				}
		
				if((char)tok[0]=='|'){
					argv[i]='\0';
					i++;
					pipewrench(tok,tokenizer,argv,derpv,i);
				}
				i++;
				argv[i]='\0';//*/
			}
	
			printf("fook-------------\n");
			for(j=0;j<4;j++){
			printf("main argv[%d], %s;\n",j,argv[j]);}
			printf("fook-------------\n");
			for(j=0;j<4;j++){
			printf("main derpv[%d], %s;\n",j,derpv[j]);}
///*Testing		
			if (derpv[0]!=NULL){
				printf("\nPIPING-------------\n");
				pipewrench(tok,tokenizer,argv,derpv,i);
			}
			else {
				printf("ELSE-------------\n");
				success = execvp (argv[0],argv);
	
				/* terminate with error code of -1 */
				perror( "Error " );
				exit(-1);
			}
//*/
/*This Works
			success = execvp (argv[0],argv);
	
			/* terminate with error code of -1 
			perror( "Error " );
			exit(-1);//*/
		}
		else {
			wait();
		}
	}
}
