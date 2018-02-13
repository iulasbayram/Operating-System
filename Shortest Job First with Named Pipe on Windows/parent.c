#include <windows.h>
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#include <time.h>
#include <stdlib.h>
#pragma warning(disable:4996)

//Our static numbers
#define SIZE 128
#define NUMBER_OF_PROCESS 5
#define NUMBER_OF_CYCLE 5

//Our named pipes
HANDLE hPipe[NUMBER_OF_PROCESS];

char* pipeName[NUMBER_OF_PROCESS] = { "\\\\.\\Pipe\\PipeName0" ,"\\\\.\\Pipe\\PipeName1" ,"\\\\.\\Pipe\\PipeName2" ,"\\\\.\\Pipe\\PipeName3", "\\\\.\\Pipe\\PipeName4" };
char* lpCommandLine[NUMBER_OF_PROCESS] = { "Child.exe 0", "Child.exe 1", "Child.exe 2","Child.exe 3","Child.exe 4" };
char* cycleOrder[NUMBER_OF_PROCESS] = { "First","Second","Third","Fourth","Fifth" };
char* processOrder[NUMBER_OF_PROCESS] = { "P1","P2","P3","P4","P5" };

//I defined several arrays
double est_burst_time[NUMBER_OF_PROCESS] = { 300.0 , 220.0 , 180.0 ,45.0 ,255.0 };
double previous_est_burst_time[NUMBER_OF_PROCESS];
double temp_est_burst_time[NUMBER_OF_PROCESS];
double actual_burst_time[NUMBER_OF_PROCESS];
int execution_order[NUMBER_OF_PROCESS];

double alpha = 0.5;

DWORD cbBytes;
STARTUPINFO si[NUMBER_OF_PROCESS];
PROCESS_INFORMATION pi[NUMBER_OF_PROCESS];
HANDLE processHandles[NUMBER_OF_PROCESS];

char parent_write[SIZE]; //this char array belongs to write_message function to sent message to child processes
char parent_read[SIZE]; //this char array belongs to read_message fuction to read message from child processes

void construct_pipes_and_processes();
void find_execution_order();
void sort_burst_time();
void read_message(int i);
void write_message(int i);
void find_next_est_burst_time();
void find_previous_burst_time();
void print_execution_table();

int main(int argc, char* argv[])
{
	
	//I constructed 5 processes and pipe for each process (5 pipes)
	construct_pipes_and_processes();

	//Below that, most outer for loop represents how many times our child processes will work
	for (int i = 0; i < NUMBER_OF_CYCLE; i++){

		
		find_previous_burst_time(); // to record previous burst times for each child process
		sort_burst_time(); // to sort burst times
		find_execution_order(); // after sorting burst times, I determined execution order by looking sorting burst times
		
		if (i == (NUMBER_OF_CYCLE -1))
		{
			sprintf(parent_write, "%s", "finish"); // this message is sent to child processes and terminated working of them
		}
		
		// Below that, execution order is printed in this for loop.
		printf("%s Execution Order <", cycleOrder[i]);
		for (int i = 0; i < NUMBER_OF_PROCESS; i++)
		{
			printf(" %s,", processOrder[execution_order[i]]);
		}
		printf(" >\n");

		
		for (int i = 0; i < NUMBER_OF_PROCESS; i++){
			printf("P%d started.\n", execution_order[i]+1); // After determining execution order by looking estimated burst times, we can print which process will start
			write_message(execution_order[i]); // I sent "finish" message to terminate working of child processes when most outer loop worked last time
			read_message(execution_order[i]); // Parent process received real burst time from each process
			printf("P%d ended.\n", execution_order[i]+1); // After child process finish its work, we can print which process is done
			actual_burst_time[i] = (double)(atoi(parent_read)); // After receiving real burst time from each process, we can record them to array
		}
		
		find_next_est_burst_time(); // To find new estimated burst times using previous estimated burst times and real burst times
		print_execution_table(); // to print general informations about burst times for child processes in detail

		printf("\n\n\n\n");
	}

	// to release processes and pipes
	for (int i = 0; i < NUMBER_OF_PROCESS; i++)
	{
		CloseHandle(pi[i].hProcess);
		CloseHandle(hPipe[i]);
	}

	system("pause");
	return 1;
}

// to construct processes and named pipes up to the number of processes
void construct_pipes_and_processes() {
	for (int i = 0; i < NUMBER_OF_PROCESS; i++) {
		SecureZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		SecureZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));

		if (!CreateProcess(NULL, lpCommandLine[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
			printf("unable to create process: %d\n", i);
			system("pause");
			ExitProcess(0);
		}
		else {
			processHandles[i] = pi[i].hProcess;
		}

		hPipe[i] = CreateNamedPipe(
			pipeName[i],
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_MESSAGE |
			PIPE_READMODE_MESSAGE |
			PIPE_WAIT,
			PIPE_UNLIMITED_INSTANCES,
			SIZE,
			SIZE,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);

		if (INVALID_HANDLE_VALUE == hPipe[i]) {
			printf("\nError occurred while "
				"creating the pipe: %d", GetLastError());
			return 1;
		}

		// this function connect parent process and child process
		BOOL connection_check = ConnectNamedPipe(hPipe[i], NULL);

		if (FALSE == connection_check)
		{
			printf("\nError occurred while connecting"
				" to the client: %d", GetLastError());
			CloseHandle(hPipe[i]);
			return 1;

		}

	}
}

// to write message to child processes by taking pipe number for specific process
void write_message(int i) {
	WriteFile(
		hPipe[i],
		parent_write,
		strlen(parent_write) + 1,
		&cbBytes,
		NULL);

}

// to read message from child processes by taking pipe number for specific process
void read_message(int i) {
		ReadFile(
		hPipe[i],           
		parent_read,        
		sizeof(parent_read),
		&cbBytes,           
		NULL);              
}

// to find execution order by sorting burst times
void find_execution_order() {
	for (int i = 0; i < NUMBER_OF_PROCESS; i++) {
		int flag = 1;
		int j = 0;

		while (flag == 1 && j < NUMBER_OF_PROCESS) {
			if (temp_est_burst_time[i] == est_burst_time[j]) {
				est_burst_time[j] = 0.0; // I paid attention if there exist two same number respectively and after one process took number of execution, I set to zero this part of array in order that other process which has same burst time with previous process can take order of execution.
				flag = 0;
			}
			else {
				j += 1;
			}
		}
		execution_order[i] = j;
	}
}

// to sort burst times
void sort_burst_time() {
	for (int i = 0; i < NUMBER_OF_PROCESS; i++) {
		temp_est_burst_time[i] = est_burst_time[i];
	}

	int i, j = 0;
	double temp = 0;
	for (i = 0; i < (NUMBER_OF_PROCESS - 1); ++i)
	{
		for (j = 0; j < NUMBER_OF_PROCESS - 1 - i; ++j)
		{
			if (temp_est_burst_time[j] > temp_est_burst_time[j + 1])
			{
				temp = temp_est_burst_time[j + 1];
				temp_est_burst_time[j + 1] = temp_est_burst_time[j];
				temp_est_burst_time[j] = temp;
			}
		}
	}
}

// after obtaining real burst times, we can find next extimated burst times using formula
void find_next_est_burst_time() {
	for (int i = 0; i < NUMBER_OF_PROCESS; i++)
	{
		double next_burst_time = alpha * actual_burst_time[i] + alpha * est_burst_time[execution_order[i]];
		est_burst_time[execution_order[i]] = next_burst_time;
	}
	
}

// to record previous burst times to print to screen
void find_previous_burst_time() {
	for (int i = 0; i < NUMBER_OF_PROCESS; i++)
	{
		previous_est_burst_time[i] = est_burst_time[i];
	}
}

// to print general informations about burst times in detail
void print_execution_table() {
	printf("\nProcess----Estimated Burst Time----Real Burst Time----Next Estimated Burst Time\n\n");
	for (int i = 0; i < NUMBER_OF_PROCESS; i++)
	{
		printf("%s             %lf           %lf           %lf\n",
			processOrder[execution_order[i]],
			previous_est_burst_time[execution_order[i]],
			actual_burst_time[i],
			est_burst_time[execution_order[i]]);
	}
}