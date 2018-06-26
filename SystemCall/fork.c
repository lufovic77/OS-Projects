/* Author: KIM KYUNG MIN
 * ID: 2016311600
 * Course: OS
 */
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

void call_kkm(void){
	pid_t pid;

	/*fork a child process*/
	pid=fork();

	if(pid <0){	//error checking
		fprintf(stderr, "FORK FAILED");
		return ;
	}
	else if (pid==0){	//child
		execlp("bin/ls", "ls", NULL);
	}
	else{	//parent
		wait(NULL);
		printf("Child Complete!");
	}
	return ;
}
int main(){
	call_kkm();
	return 0;
}
