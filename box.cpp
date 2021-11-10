// Erick Tran ericktran@csu.fullerton.edu
// CPSC351 Kenytt Avery
// Project 1
// Due Sept 17, 2021
// Purpose: Runs a command and saves the contents of
// its standard error stream to a file

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  // Display command args
  if (argc == 1) {
    printf("Usage: ./stderr FILE PROG [ARGS]...\n");
    return 0;
  }

  // Create new char *array for child process arguments
  char **fork_argv;
  fork_argv = new char*[argc];
  for (int i = 0; i < argc-1; i++){
    fork_argv[i] = argv[i+2];
  }

  int rc = fork();
  if (rc < 0) {
    fprintf(stderr, "Fork failed\n");
    exit(1);
  }

  else if (rc == 0) {
    // Closes pointer for STDERR and replaces it with a new file pointer
    close(STDERR_FILENO);
    open(strdup(argv[1]), O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);

    rc = execvp(fork_argv[0], fork_argv);
  }

  else {
    wait(NULL);
  }
  return 0;
}
