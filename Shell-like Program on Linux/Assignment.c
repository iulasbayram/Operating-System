#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h> 
#include <signal.h>
#include <sys/types.h>

// Ismail Ulas Bayram - 220201040

// node struct for keeping command argument
struct tsk_node_s {
	char argument[50];
	struct tsk_node_s* next;
};


struct tsk_node_s* front = NULL;
struct tsk_node_s* rear = NULL;
struct tsk_node_s* node = NULL;

// size of queue
int SizeOfQueue(){
	struct tsk_node_s* temp = front; 
	int num = 0;
	while(temp != NULL) {
		num += 1;
		temp = temp->next;
	}
	return num;	
}

// dequeue
struct tsk_node_s *Task_dequeue(){
	struct tsk_node_s* temp = front; 
	
	if (front == NULL)
	{
		printf("There is no task in queue\n");
		return NULL;
	}else{
		
		if (front == rear)
		{	
			front = NULL; 
			rear = NULL;
			return temp;

		}else{

			front = front->next;
			return temp;
		}
		
	}
}

// enqueue
void Task_enqueue(char *arg){
	struct tsk_node_s *temp = (struct tsk_node_s*)malloc(sizeof(struct tsk_node_s));
	
	strcpy(temp->argument, arg);
	temp->next = NULL;
	if (rear == NULL && front == NULL)
	{
		front = temp; 
		rear = temp;

	}else{
		rear->next = temp;
		rear = temp;
	}
}

// print last 10 argument
void print_History(){
	struct tsk_node_s *temp = front;
	int num = 1;
	while(temp != NULL){
		printf("[%d] %s\n",num, temp->argument);
		temp = temp->next;
		num += 1;
	}
}

// record arguments.. if arguments is greater than 10, we dequeue first one
void record_command(char* command){
	if (SizeOfQueue() >= 10)
	{
		Task_dequeue();
		Task_enqueue(command);
	}else{
		Task_enqueue(command);
	}
	
}

int main(int argc, char const *argv[]){
	char* HOME; // home directory
	char* PATH = getenv("PATH"); // path directory

	while(1){
		printf("myshell>");

		char command[100];

		scanf("%99[^\n]",command);
		char* command_copy = malloc(sizeof(char) * 254); // copy of command to get argument. if i do not copy of command line, it records first token. So, I need to create copy of it.
		strcpy(command_copy,command);
		char cwd[PATH_MAX];
		record_command(command_copy); // record command to history
		
		char* token = strtok(command," ");
		
		char* param[100]; // keep all arguments except ampersand
		char* ampersand; // keep only ampersand
		int i = 0;
		
		while( token != NULL ) {
			if (strcmp(token,"&") == 0)
			{
				ampersand = token;
			}else{
				*(param+i) = token;
			}

			token = strtok(NULL," ");
			i += 1;
		}

		// ------------------------------- Below that, these operations for parsing vertical line(|) and checking whether pipe is needed or not -----------
		// !!!!!! IMPORTANT !!!!! Sometimes strtok() function does not parse command argument in despite of giving vertical line (|) as strtok argument
		// For example, when I tried to give command argument like ls -l | sort, output was correct four times like ls, -l, |, sort but two times output was not correct like ls, -l, | sort
		int x = 0;
		int param2_i = 0;
		int param3_i = 0;
		int vertical_line_i = 0;
		int check = 0;
		char* param2[100];
		char* param3[100];
		char* vertical_line[10];
		while(x < i){ 
			if(strcmp(*(param+x),"|") == 0){
				*(vertical_line+vertical_line_i) = *(param+x);
				vertical_line_i += 1;
				check += 1;
			}
			else if(check == 0){
				*(param2+param2_i) = *(param+x);
				param2_i += 1;
			}else if(check == 1){
				*(param3+param3_i) = *(param+x);
				param3_i += 1;
			}
			x += 1;
		}

		if(*(param3+param3_i) != NULL)
			*(param3+param3_i) = NULL;

		if(*(param2+param2_i) != NULL)
			*(param2+param2_i) = NULL;
		//------------------------------------------------------------------------------------------------------------------------------------------------


		if (strcmp(vertical_line[0],"|")==0){
			int read_and_write[2];
			if(pipe(read_and_write) == -1) {
				perror("Pipe failed");
				exit(1);
			}

		    if(fork() == 0)            //first fork
		    {
		        close(STDOUT_FILENO);   // This is close operation for stdout
		        dup(read_and_write[1]);  // Replace operation for stdout with write pipe     
		        close(read_and_write[0]); //Close operation both write and read
		        close(read_and_write[1]);  //Close operation both write and read
		        execvp(param2[0], param2);
		        exit(1);
		    }

		    if(fork() == 0)            
		    {
		        close(STDIN_FILENO);   //This is close operation for stdin
		        dup(read_and_write[0]);     //Replace operation for stdin with read pipe
		        close(read_and_write[1]);    //Close operation both write and read
		        close(read_and_write[0]);	//Close operation both write and read

		        execvp(param3[0], param3);
		        exit(1);
		    }

		    close(read_and_write[0]); // At the end of the pipe, closing both read and write operations
		    close(read_and_write[1]);
		    wait(0);
		    wait(0);

		    for (int a = 0; a <= param2_i; ++a) // cleaning tokens in our array for next command argument opearation
		    {
		    	param2[a] = NULL;
		    }

		    for (int a = 0; a <= param3_i; ++a) // cleaning tokens in our array for next command argument opearation
		    {
		    	param3[a] = NULL;
		    }

		    vertical_line[0] = "\0"; // cleaning vertical line for next command argument operation
		    getchar(); // this structure is used for clean command array at the beginning of the while loop. When I do not use this structure, the program does not ask input for second time.


		}else{	


			// below that, this is for preventing strcmp error for ampersand.
			// If ampersand null, strcmp throws error. Also, doing ampersand != NULL condition in if strucute throws error. So, that is for preventing errors.
				if (ampersand == NULL)
				{
					ampersand = "0";
				}


				if (strcmp(param[0],"cd") == 0)
				{	
				if (param[1] != NULL) // If second arguments exist
				{
					chdir(param[1]);
				}else{ 
					HOME = getenv("HOME"); 
					chdir(HOME);
				}
			}

			else if (strcmp(param[0],"dir") == 0)
			{
				getcwd(cwd, sizeof(cwd));
				printf("%s\n", cwd);
			}
			else if (strcmp(param[0],"history") == 0)
			{
				print_History();
			}

			else if (strcmp(param[0],"bye") == 0){
				exit(0);
			}

			else if(strcmp(param[0],"findloc") == 0){
				char* path_reverse[100];
				struct dirent *de;
				bool flag = true;
				int c = 0;
				char* token; 
				char* rest = malloc(sizeof(char) * 254);
				strcpy(rest,PATH);
				int d = 0;
				while ((token = strtok_r(rest, ":", &rest))) {
					*(path_reverse+c) = token;
					d = c;
					c++;
				}
				while(c != 0 && flag == true){

					c -= 1; // after parsing, we search in the parth with reverse order. So, I decrement variable from last index to first index
					DIR *dr = opendir(*(path_reverse+c));

					while (param[1] != NULL && ((de = readdir(dr)) != NULL) && flag == true){ 
						if (de->d_name[0] == '.') // if executable file has dot expression at the beginning of its, I check this situation
						{

							// below that, I concatenate dot expression with our executable file. After re-modification, we keep on the search operation.
							char* dot = ".";
							char *dotstring = malloc(strlen(dot) + strlen(param[1]) + 1);
							strcpy(dotstring,dot);
							strcat(dotstring,param[1]);
							
							if (strcmp(de->d_name,dotstring)==0) // if we find the path, we concatenate it with our exacutable file by adding "/" between them.
							{
								char* full_path =  malloc(strlen(*(path_reverse+c)) + strlen(param[1]) + 2);
								strcpy(full_path,*(path_reverse+c));
								strcat(full_path,"/");
								strcat(full_path,param[1]);
								flag = false;
								printf("%s\n", full_path);
							}
						}else{ // if there is no dot expression at the beginning of our executable file, so we can keep on the seaching.

							if (strcmp(de->d_name,param[1])==0)
							{
								char* full_path =  malloc(strlen(*(path_reverse+c)) + strlen(param[1]) + 2);
								strcpy(full_path,*(path_reverse+c));
								strcat(full_path,"/");
								strcat(full_path,param[1]);
								flag = false;
								printf("%s\n", full_path);
							}
						}
					}
				}

				for (int a = 0; a <= d; ++a) // cleaning tokens in our array for next findloc opearation
				{
					path_reverse[a] = NULL;
				}


			}
			else{

				// below that, we call fork() to create child process and child process applies its task
				// if there is no ampersand, parent process should not wait for child process
				// if there is ampersand, parent process should wait for child process
				pid_t pid = fork();
				if (pid == 0){
					int i = execvp(param[0], param);
					printf("i: %d\n", i);

				}
				else if (pid > 0 && ampersand != NULL && strcmp(ampersand,"&")!= 0){
					wait(NULL);
				}
			}

		getchar(); // this structure is used for clean command array at the beginning of the while loop. When I do not use this structure, the program does not ask input for second time.

		for (int a = 0; a <= i; ++a) // cleaning tokens in our array for next command argument opearation
		{
			param[a] = NULL;
		}
	}	
}
return(0);
}