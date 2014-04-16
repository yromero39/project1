#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])
{
  char input[1024];
  
  printf("\nsish:> ");
  
  while(fgets(input, 1024, stdin) != NULL) {

    //  puts(input);
    
    if(strncmp(input,"exit", 4) == 0)
      printf("exit\n");

    else if(*input == '\n')
      printf("sish:> ");
    
    else {
      char *str;
      char *command[100];

      str = strtok(input, " \n");
     
      int i = 0;
      while(str!=NULL){
	//	printf("%s \n", str);
	command[i] = str;
	str = strtok(NULL, "  \n");
	i++;
      }

      int x;

      for(x = 0; x <i; x++)
	printf("%s ", command[x]);

      printf("\n");
    
      
      printf("sish:> ");
    
    }

  }
  printf("\n");
  return 0;
}
