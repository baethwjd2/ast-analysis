#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80

int read_command(char input[]);
int* parse_args(char input[], char* args[]);
void execute(char* args[], int* param);
void redirection(char* args[], int* param);
void execute_pipe(char* args[], int* param);


int main(void){
	char input[MAX_LINE+2];
	char* args[MAX_LINE/2+1];
	int should_run=1;
	int* param;
	// param[0]: background or not, parm[1]: index of redirection,
	// param[2]: index of pipe, param[3]: length of args

	while (should_run){
		// Read Command from User
		should_run = read_command(input);

		// If should_run=0, Exit
		if(should_run==0) break;

		// Parse to args
		param = parse_args(input, args);

		// Execute Command in Child Process
		execute(args, param);
	}

	return 0;
}



// Read the command entered by the user and stores it in input 
int read_command(char input[]){
	printf("osh>");
	fflush(stdout);
	fgets(input, MAX_LINE+2, stdin);

	// If user types "exit", then return 0
	if(strcmp(input, "exit\n")==0){
		return 0;
	}

	return 1;
}


// Parse the command entered by the user into args[] and param[]
int* parse_args(char input[], char* args[]){
	int len=0;
       	static int param[4];
	char* token = strtok(input, " ");
	
	// Initialize param
	for(int i=0;i<4;i++){
		param[i] = -1;
	}

	while(token!=NULL){
		if(strcmp(token, "<")==0 || strcmp(token, ">")==0){	// If ">" and "<" was entered
			param[1] = len;
		}

		if(strcmp(token, "|")==0){	// If "|" was entered
			param[2] = len;
		}
		
		// Parse to args
		args[len] = token;
		len++;
		token = strtok(NULL, " ");
	}

	if(strcmp(args[len-1], "&\n")==0){	// If "&" was entered
		param[0] = 1;
		args[len-1] = NULL;
		len -= 1;
	}else{	// Remove "\n"
		char* tmp_token = strtok(args[len-1], "\n");
		args[len-1] = tmp_token;
	}

	args[len] = NULL;	// No more args
	param[3] = len;

	return param;
}


// Create a child process and execute the command requested by the user
void execute(char* args[], int* param){
	int status;
	pid_t pid;
	pid = fork();

	if(pid<0){	// If fork() failed
		fprintf(stderr, "Fork failed\n");
	}else if (pid==0){	// If child process
		if(param[1]>-1) redirection(args, param); // If redirection

		if(param[2]>-1){	// If pipe
			execute_pipe(args, param);
		}else{	
			execvp(args[0], args);
			printf("Execution failed.\n"); 	// If execution failed
			fflush(stdout);
		}
	}else{	// If parent process
		if (param[0]==-1){	// If running in the foreground
			waitpid(pid, &status, 0);
		}else{	// If running in the background
			printf("[1] %d\n", getpid());
		}
		waitpid(-1, NULL, WNOHANG);     // Remove zombie process
		sleep(1);
	}
}


// Redirect I/O to a file
void redirection(char* args[], int* param){
	int fd, newfd;
	char* opr=args[param[1]];
	args[param[1]] = NULL;

	if(strcmp(opr, ">")==0){ // Output redirection
		fd = open(args[param[1]+1], O_RDWR|O_CREAT|S_IROTH, 0644);
		newfd = STDOUT_FILENO;
	}else{	// Input redirection
		fd = open(args[param[1]+1], O_RDONLY);
		newfd = STDIN_FILENO;
	}
	
	if (fd < 0){ // Open failed
		perror("Error");
	}else{
		dup2(fd, newfd);
		close(fd);
	}
}


// Execute the command by doing pipe communication
void execute_pipe(char* args[], int* param){;
	// Separate command
	int idx=0, status;
	char* args2[MAX_LINE/2];
	args[param[2]] = NULL;

	for(int i=param[2]+1;i<param[3];i++){
		args2[idx] = args[i];
		idx++;
	}
	args2[idx] = NULL;


	// Pipe communication
	int fd[2];
	pipe(fd);

	pid_t pid;
	pid = fork();
	
	if(pid < 0){	// If fork() failed
		fprintf(stderr, "Fork failed\n");
	}else if (pid==0){	// If grandchild process
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		close(fd[1]);
		execvp(args[0], args);
	}else{	// If child process
		waitpid(pid, &status, 0);
		dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		close(fd[0]);
		execvp(args2[0], args2);
	}
}





