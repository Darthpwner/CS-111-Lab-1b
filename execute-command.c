// UCLA CS 111 Lab 1 command execution

#include "command.h"
#include "command-internals.h"
#include <unistd.h> //For dup2
#include <fcntl.h> //For open
#include <sys/wait.h>
#include <string.h>
#include <error.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

int
command_status (command_t c)
{
  return c->status;
}

//and function
void execAND(command_t c){
	//check either side of and and then call executeCommand on both sides
	executeCommand((c->u).command[0]); // execute the first command 
	//set status
	c->status= (c->u).command[0]->status;
	if(c->status == 0){ // only execute the second command if first one succeeds
		executeCommand((c->u).command[1]);
		// reset status 
		c->status = (c->u).command[1]->status;
	}
			
}

void execOR(command_t c){
	//check either side of and and then call executeCommand on both sides
	// want to only execute the second one if the first one fails
	executeCommand((c->u).command[0]);
	//set status
	c->status = (c->u).command[0]->status;
	if(c->status != 0){
		executeCommand((c->u).command[1]);
		// again reset status
		c->status = (c->u).command[1]->status;
	} 
}

void execSEQUENCE(command_t c){
	// want to execute each command seperately
	//will probably need to modify this later for timeshell
	executeCommand((c->u).command[0]);
	executeCommand((c->u).command[1]);
	c->status = (c->u).command[1]->status; // execution is from left to rightz
}

void execPIPE(command_t c)
{
  //Check both sides of the pipe and call executeCommand on both sides
  int fd[2];
  pipe(fd);
  pid_t firstPid = fork(); //execute right command
  
  printf("ENTERED EXECPIPE\n");

  //Case 1: Spawned a child process
  if(firstPid == 0)
    {
      close(fd[1]); //Close unused write command
      dup2(fd[0], 0);
      execvp(c->u.command[1], c->u.command); /*Use a system call to execute 
					       the program on the right child*/
      
      // close(fd[0]); //Close after you execute the program
    }
  
  else
    {
      pid_t secondPid = fork(); //execute left command
      
      //Case 2: The second process id's fork spawned a child process
      if(secondPid == 0)
	{
	  close(fd[0]); //Close unused read command
	  dup2(fd[1], 1);	  
	  execvp(c->u.command[0],c->u.command); /*Use a system call to execute 
						the program on the left child*/
	  
	  // close(fd[1]); //Close after you execute the program
	}

      /*Case 3: We reached the second process id but we are not in a child 
	process*/
      else
	{
	  close(fd[0]);
	  close(fd[1]);
	  int status;
	  waitpid(-1, &status, 0);
	  waitpid(-1, &status, 0);
	}
    }
}

void execSIMPLE(command_t c){
	// need to handle input and output redirects if there are any 
	int status; 
	pid_t child;
	child = fork();
	if(child == 0){
	  //input and output defined in command-internals.h
		if(c->input != NULL)
		  {
		    int fd;
		    fd  = open(c->input, O_RDONLY, 644);
		    if(fd < 0)
		      error(1,0,"failed to find valid file descriptor");
		    else if(dup2(fd, 0) < 0)
		      error(1,0,"couldn't do input redirect");
		    close(fd);
		  }
		
        	if(c->output != NULL)
		  {
		    int fd2;
		    fd2 = open(c->output, O_CREAT | O_TRUNC | O_WRONLY, 644);
		    if(fd2 < 0)
			error(1,0,"failed to find valid file descriptor");
		    else if( dup2(fd2, 1) < 0)
			error(1,0,"couldn't do output redirect");
		    close(fd2);
		  }
		//execute command otherwise print out error
		char * exec = "exec";
		//strcmp returns 0 if identical
		if(!strcmp((c->u).word[0], exec)){
			execvp((c->u).word[1], &((c->u).word[1]));
		}
		else{
			execvp((c->u).word[0], (c->u).word);
		}
		error(1,0, "This simple command is invalid");
	}
	else if(child > 0) {
		waitpid(child, &status, 0);
		c->status=status;
	}
	else {
		error(1,0, "failure in fork");
	}
}

void execSUBSHELL(command_t c)
{  
  (c->u.subshell_command)->output = c->output;
  (c->u.subshell_command)->input = c->input;
  executeCommand(c->u.subshell_command);
}

executeCommand(command_t c){
	switch(c->type){
		//insert functions into switch that handle each case
		case AND_COMMAND:
		  execAND(c);
		  break;
		case OR_COMMAND:
		  execOR(c);
		  break;
		case SEQUENCE_COMMAND:
		  execSEQUENCE(c);
		  break;
	        case PIPE_COMMAND: //Matt
		  execPIPE(c);
		  break;
		case SIMPLE_COMMAND:
		  execSIMPLE(c);
		  break;
	        case SUBSHELL_COMMAND: //Matt 
		  execSUBSHELL(c);
		  break;
		default:
		  error(1,0,"hit command that is not valid");
		  break;
	}
}

void
execute_command (command_t c, bool time_travel)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
//  error (1, 0, "command execution not yet implemented");
	executeCommand(c);
}
