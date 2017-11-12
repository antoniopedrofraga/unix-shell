#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFSIZE 1024
#define TOK_DELIM " \t\r\n\a"
#define TOK_BUFSIZE 64;

void allocation_error_checker(char * buffer) {
	if (!buffer) {
		printf("shell:: allocation error!\n");
		exit(EXIT_FAILURE);
	}
}
void allocation_error_checker(char ** buffer) {
	if (!buffer) {
		printf("shell:: allocation error!\n");
		exit(EXIT_FAILURE);
	}
}

char * read_line() {
	int bufsize = BUFSIZE;
	int position = 0;
	char *buffer = (char*)malloc(sizeof(char) * bufsize);
	int c;

	allocation_error_checker(buffer);

	while (1) {
		c = getchar();

		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;
		if (position >= bufsize) {
			bufsize += BUFSIZE;
			buffer = (char*)realloc(buffer, bufsize);
			allocation_error_checker(buffer);
		}
	}
}

char ** split_line(char * line) {
	int bufsize = TOK_BUFSIZE; 
	int position = 0;
	char ** tokens = (char**)malloc(bufsize * sizeof(char*));
	char * token;

	allocation_error_checker(tokens);

	token = strtok(line, TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

		if (position >= bufsize) {
			bufsize += TOK_BUFSIZE;
			tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
			allocation_error_checker(tokens);
		}

		token = strtok(NULL, TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

int execute(char ** args) {
	pid_t pid, wpid;
	int status;

	pid = fork();

	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
	    	perror("shell");
	    }
	    exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("shell");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

void shell_loop() {
	char * line;
	char ** args;
	int status;

	do {
		printf("> ");
		line = read_line();
		args = split_line(line);
		status = execute(args);

		free(line);
		free(args);
	} while(status);
}

int main() {
	shell_loop();
	return EXIT_SUCCESS;
}