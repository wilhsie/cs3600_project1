/**
 * CS3600, Spring 2013
 * Project 1 Starter Code
 * (c) 2013 Alan Mislove
 *
 * You should use this (very simple) starter code as a basis for 
 * building your shell.  Please see the project handout for more
 * details.
 */

#include "3600sh.h"
#include <unistd.h>
#include <stdio.h>

#define USE(x) (x) = (x)

char **separate(char *input);

int main(int argc, char*argv[]) {
  // Code which sets stdout to be unbuffered
  // This is necessary for testing; do not change these lines
  USE(argc);
  USE(argv);
  setvbuf(stdout, NULL, _IONBF, 0); 

  // Variables for printing the prompt
  char *user = getenv("USER");
  char *pwd = getenv("PWD");
  char hostname[1024];
  hostname[1023] = '\0';
  gethostname(hostname, 1023);

  // Used during fork:
  pid_t pid;
  
  // Main loop that reads a command and executes it
  while (1) {
    // You should issue the prompt here
    printf("%s@%s:%s>", user, hostname, pwd);

    // Variables for character reading
    int allocation_size = 32;
    char *input_string = malloc(allocation_size * sizeof(char));
    char **separated = malloc(100 * sizeof(char *));
    int input_chari = 0;
    char input_charc = ' ';
    int count = 0;
    
    // Read input
    while(((input_chari = getchar()) != EOF) && (input_chari != 0) && (input_chari != '\n')){
      input_charc = (char)(input_chari);
      // Increase memory buffer if we anticipate overflow
      if(count > allocation_size){
	allocation_size = allocation_size * 2;
	input_string = realloc(input_string, (allocation_size * sizeof(char)));
      }
      input_string[count] = input_charc;
      count++;
    }

    // Check to see if exit command has been input
    if(strcmp(input_string, "exit") == 0){
      free(input_string);
      free(separated);
      do_exit();
    }
    
    input_string[count++] = '\0';

    separated = separate(input_string);
    
    printf("%s\n", separated[0]);

    pid = fork();
    if (pid < 0){
      fprintf(stderr, "Fork Failed");
      return 1;
    }
    else if(pid == 0){
      execvp(separated[0], separated);
      printf("Child in Progress\n");
      exit(0);
    }
    else{
      wait(NULL);
      printf("Child Complete\n");
    }
	
  }
  
  return 0;
}

// Function that separates the input string into multiple words
char **separate(char *input){
  char **args = (char **) malloc(100 * sizeof(char *));
  char *temp = (char *) malloc(100 * sizeof(char));
  *temp = 0;
  int argcount = 0;
  int tempcount = 0;
  for(int i = 0; i < ((int)strlen(input)); i++){
    // Get rid of leading spaces
    while(input[i] == ' '){
      i++;
    }
    // If we reach the end of an argument
    if(input[i] == ' ' && strlen(temp) != 0){
      args[argcount] = temp;
      free(temp);
      temp = (char *) malloc(100 * sizeof(char));
      argcount++;
      tempcount = 0;
    }
    // If normal character, add to temporary string
    else{
      temp[tempcount] = input[i];
      tempcount++;
    }
    // Add the temporary string to the args char **
    args[argcount] = temp;
  }
  free(temp);
  return args;
}


// Function which exits, printing the necessary message
//
void do_exit() {
  printf("So long and thanks for all the fish!\n");
  exit(0);
}
