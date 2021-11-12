// Erick Tran ericktran@csu.fullerton.edu
// CPSC351 Kenytt Avery
// Project 3
// Due November 12, 2021
// Purpose:

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// main funct
int main(int argc, char *argv[]) {
  // Display command args
  if (argc == 1) {
    std::cerr << "Usage: ./rebuild COMMAND [FILE]\n";
    exit(EXIT_FAILURE);
  }
  else {
    //execvp(argc)
  }

  return EXIT_SUCCESS;
}
