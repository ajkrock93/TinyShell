/*
	Author: Andrew Krock
	Filename: shell.cpp
	Description: A tiny shell
	Last Edit: 02/23/15
*/

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <stdlib.h>
#include <list>
//#include "shell.h"

using namespace std;

//Function Prototypes
int main(void);
void split(char *,char **, char);
void execute(char **,bool);
int lastval(char **);
int charLocater(char **, char*);
void clean(char **,int);
void printarray(char **);



int main(){
	char command[1024];
	char *argv[64];
	static char cwd[1024];
	//getcwd(cwd,sizeof(cwd));

	while(1){
		//cout<<cwd;		//Displays path
		cout<<"/Andy's_shell>>";		//What's displayed on the command line
		cin.getline(command,1024);		//Read in from command line
		split(command,argv,' ');		//Parse it out and generate arguement vecotr
		bool inputdirect=false;
   	bool outputdirect=false; 
		int inputloc;		//input '<' location
		int outputloc;		//output'>' location
    
		//input
		if ((inputloc = charLocater(argv,(char *)"<")) != -1){	
        char *input = argv[inputloc + 1];
        freopen(input, "r", stdin);		//read in from file
        inputdirect = true;
    }
		
		//output
		if ((outputloc=charLocater(argv,(char *)">")) != -1){
				char *output=argv[outputloc+1];
				freopen(output,"w",stdout);		//write to a file
				outputdirect=true;
		}
		
		//cleans arg vector
    if (inputdirect)	clean(argv, inputdirect);
		if (outputdirect) clean(argv, outputdirect);

		if (argv[0] == NULL){		//Do Nothing
		}
		else if (strcmp(argv[0], "exit") == 0){		//Exit the program
    	exit(0); 
		}
		else if (strcmp(argv[0], "version") == 0){		//Displays version of program
			cout<<"Andrew Krock Shell v0.1\n";
		}
		else if (strcmp(argv[0], "cd") == 0){		//Changes current working directory
				if (chdir(argv[1]) == -1){
					cout<<"Directory does not exsist\n";
				}
				else{
					chdir(argv[1]);
				}
		}
		else if (strcmp(argv[0], "pwd") == 0){		//Displays current file path
				if (getcwd(cwd,sizeof(cwd)) != NULL){
					cout<<cwd;
					cout<<"\n";
				}
		}
		else {
			bool background = false;
        if (strcmp(argv[lastval(argv)-1], "&") == 0){ //checks for & background option
        printarray(argv);
            argv[lastval(argv)-1] = NULL; //removes & from argument vector
            background = true;
				}
				execute(argv,background);
		}
	}
}

/*
command=string being parsed
argv= where the parsed string goes
delim=what it is split on
*/

void split(char *command,char **argv, char delim){
		while (*command != '\0'){		//checking to see if end of line
			while (*command == delim )		//Replace spaces with \0
      	*command++ = '\0';     
        *argv++ = command;		//Stores address of arguement
        while (*command != '\0' && *command != delim && *command != '\t' && *command != '\n') 
        	command++;            
     }
     *argv = '\0';		//End of arguement parser
}
//Executes the program
void execute(char **argv, bool background){
	int status;
	static list<pid_t> children;
	static int programs_backgrounded = 0;
  static int backgrounds_finished = 0;
	pid_t pid = fork();		//Returns 0 to child, childs pid to the parent
	
	if (pid == -1) {		//Fork error
  	printf("ERROR forking child process\n");
		exit(1);
 	}
  else if (pid == 0) {		//Child Procces
  	if (execvp(argv[0], argv) < 0) {
    	printf("ERROR execution failed\n");
			exit(1);
    }
  }
  else if (pid > 0){		//Parent Procces
		for (list<pid_t>::iterator i = children.begin(); i != children.end(); i++){ 
    	pid_t killed = waitpid(*i, &status, WNOHANG|WUNTRACED); 
      if (killed > 0){
      	children.erase(i++); 
        backgrounds_finished++;
        printf("[%d] Done\n", backgrounds_finished);
      }
    }
    if (background){
    	children.push_back(pid);
      programs_backgrounded++;
      printf("[%d] %d\n", programs_backgrounded, pid);
    }
    waitpid(pid, &status, (background) ? WNOHANG : WUNTRACED);	
 	}
}

//Finds the index of an element in an array
int charLocater(char **array,char* element){
	int index=-1;
	for(int i=0;i<lastval(array);i=i+1){
		if(strcmp(array[i],element) == 0){
			index=i;
			break;
		}
	}
	return index;
}

//Returns the number of items in an array
int lastval(char **array){
	int i=0;
	while(array[i] != NULL){
		i++; 
	}
	return i;
}

//Prints an array
void printarray(char **array){
	for (int i=0;i<lastval(array);i++){
		printf("Arg[%d] %s\n",i,array[i]);
	}
}

void clean(char **array, int index){
	for (int i=index;i<lastval(array);i++){
		array[i]=NULL;
	}
}
