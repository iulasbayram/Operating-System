#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#pragma warning(disable:4996)

//Our static numbers
#define NUMBER_OF_PROCESS 5
#define BUFFER_SIZE 1024 

//Our named pipes
char* pipeName[NUMBER_OF_PROCESS] = { "\\\\.\\Pipe\\PipeName0" ,"\\\\.\\Pipe\\PipeName1" ,"\\\\.\\Pipe\\PipeName2" ,"\\\\.\\Pipe\\PipeName3", "\\\\.\\Pipe\\PipeName4" };

char child_write[BUFFER_SIZE]; //this char array belongs to write_message function to sent message to parent processes
char child_read[BUFFER_SIZE]; //this char array belongs to read_message function to sent message to parent processes
DWORD cbBytes;
HANDLE hPipe;
void construct_pipe(int id);
void read_message();
void write_message();
int random_num_generator();

int main(int argc, char* argv[])
{
	
	char* message = "";	
	message = argv[1];

	int id = atoi(message); // parameter from message determine that which process will be active
	srand(time(NULL) + 100 * id);
	
	// I construct named pipe for a particular child process by taking pipe name.
	construct_pipe(id);

	int execution_order = 1;

	while (strcmp(child_read, "finish") != 0) // When receiving "finish" message from parent process, our child processes terminate their works.
	{
		printf("------------%d. cycle----------\n\n",execution_order); // to print execution order
		
		printf("Process %d started\n", id+1);
		int random_num = random_num_generator(); // to generate burst time randomly
		
		
		Sleep(random_num); // to simulate working of child processes
		printf("%d. process worked at %dms.\n", id+1, random_num); 
		printf("Process %d ended\n", id+1); 

		read_message(); //to read message from parent process and this message is assigned to child_read array

		sprintf(child_write, "%d", random_num);
		write_message(); // to sent real burst time to parent process
		execution_order++;
		printf("\n\n");
	}
	
	// to release our pipe
	CloseHandle(hPipe);

	system("pause");
	return 1; 
}

// to construct pipe for a particular pipe name
void construct_pipe(int id) {
	hPipe = CreateFile(pipeName[id], GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hPipe) {
		printf("\nError occurred while connecting"
			" to the server: %d", GetLastError());

		return 1;
	}
}

// to write message to parent process
void write_message() {
		WriteFile(
		hPipe,                 
		child_write,             
		strlen(child_write) + 1,
		&cbBytes,            
		NULL);               

}

// to read message from parent process
void read_message() {
		ReadFile(
		hPipe,                
		child_read,           
		sizeof(child_read),   
		&cbBytes,             
		NULL);               
}

// to generate random number between 50 and 300
int random_num_generator() {
	int r = rand()%250 + 50;
	return r;
}