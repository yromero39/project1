// Biggest concerns:

//        - Parsing of quotes
//          I think we need to parse in two phases: first use quotation marks as separators, then parse the odd tokens with whitespace as separator (and newline, just to eliminate the newline at the end)
//          I think we shouldn't use strtok, because we have to handle a lot of whitespace characters (and I don't think it's a good idea too use about 200 separators for strtok). Switching to manual parsing requires more code, but it probably makes it way easier to handle quoted strings.

//        - Handling of the current directory
//          I don't know if we have to keep track of the current directory ourselves... The command 'pwd' somehow retrieves the current directory, which suggests that the OS keeps track of the current directory. On the other hand, I couldn't get the system call chdir() working...

// We still need to check if the input is stdin (then we should display "sish:>" on every line in the output), but this shouldn't be too hard
// I didn't even start looking at the piping...
// Error handling (for exit codes other than 0) still needs to be implemented. See manpage for execvp

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>

const int bufferSize = 1024;
const char *separators = " \t\n";
const char *prefix = "sish:> ";
int length;
int status;

char **parse(char *input);

int main()
{
  bool exiter = false;
  char *buffer = malloc(bufferSize);
  char *arg[bufferSize];

  int filedes;
  int i;
  
  printf("%s", prefix);

  while (!exiter && fgets(buffer, bufferSize, stdin) != NULL)
    {
       //arg = parse(buffer);
      char *str;
      str = strtok(buffer, " \n");
     
      int x = 0;
      while(str!=NULL){
      	arg[x] = str;
      	str = strtok(NULL, "  \t\n");
      	x++;
      }
      length = x;

      // check for builtin commands manually
      if (strncmp(arg[0], "exit", 4) == 0) exiter = true;

      else if (strncmp(arg[0], "cd", 2) == 0)
	{
	  if (arg[1] == NULL) {} // if there are no arguments, go to ~
	  
	  else chdir(arg[1]); //!!! this doesn change the value that the command pwd returns: I'confused...
	}
      
      else		// else, use fork and execvp
	{ 
	  int check = 0;

	  for(i = 0; i < length; i++){    //loop through args    
	  
	    if((0==strncmp(arg[i],"<",1)) || (0==strncmp(arg[i],">",1))){ 
	      if (fork() == 0)
		{
		  check++;

		  //open the correct file descriptor depending 

		  if (0==strncmp(arg[i],">>",2))
		    
		    filedes = open(arg[i+1], O_WRONLY |  O_SYNC | O_CREAT | O_APPEND, 0666);
		  
		  else if (0==strncmp(arg[i],">",1))
		    filedes = open(arg[i+1], O_WRONLY | O_TRUNC | O_SYNC | O_CREAT, 0666);
		  
		  
		  else if(0==strncmp(arg[i],"<",1))
		    filedes  = open(arg[i+1], O_RDONLY |  O_SYNC , 0666);
		  
		  if (filedes < 0) {
		    exit(1);
		  }
		  
		  //dup appropriate file descriptor
		  
		  if(0==strncmp(arg[i],">",1)){
		  dup2(filedes, 1);
		  close(filedes);
		  }
		  
		  else if(0==strncmp(arg[i],"<",1)){
		    dup2(filedes, 0);
		    close(filedes);
		  }
		  
		  
		  arg[i] = NULL;

		/*	int exitCode = execvp(arg[0], arg);
		//!!! it seems execvp still returns succesfully when arg[0] is a shell builtin (for example, try 'echo'), but does't actually execute the builtin command... this leads to strange behavior as opens an extra instance of our shell which needs to exited
		if (exitCode == -1) {
		  printf("ERROR: exec failed.\n");
		  exiter = true;
		  } */
		  execvp(arg[0], arg);
		  
		  exit(1);
		}
	      else{
		wait(&status);
	      }
	      continue;
	    }

	    if(check == 0 && (i == length -1)){
	      if(fork() == 0){

		execvp(arg[0], arg);
		exit(1);
	      }
	      else
		wait(&status);
	    }
	  }
	  if (!exiter) printf("%s", prefix);
	}
    }
  free(buffer);
  return 0;
}



/* char **parse(char* input) */
/* { */
/*     char **result; */
/*     length = 1; */
/*     int n = 0; */

/*     // find length */
/* 	strtok(input, " \t");		// initiate parsing */
/*     while (strtok(NULL, separators)) length++; */
/*     result = malloc(sizeof(char*) * length); */

/*     // parse again to fill array */
/* 	result[n++] = strtok(input, separators); */
/*     while ((result[n++] = strtok(NULL, separators))); */
/* 	//!!! can we trust that the data that strtok points to is preserved when we exit the function??? */

/*     return result; */
/* } */
