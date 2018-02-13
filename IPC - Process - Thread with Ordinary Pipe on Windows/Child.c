#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

typedef struct //every thread has its own item,itemCount,day and threadNo
{
	int* itemCount;  //number of item which thread responsible to count
	char *item;   //name of item which thread responsible to count
	int day;      //thread responsible to count items in that day
	int threadNo; //to differ threads each other, because we have 4 threads
}THREAD_PARAMETERS;

DWORD WINAPI threadWork(LPVOID parameters); //threads read file in that common function

int main(void)
{
	DWORD dwRead;
	HANDLE hStdin, hStdout;
	HANDLE* handles;
	THREAD_PARAMETERS* lpParameter;
	int* threadID;
	int threadCount = 4; //we have 4 threads

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE); //neccessary handles are set here
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if ((hStdout == INVALID_HANDLE_VALUE) || (hStdin == INVALID_HANDLE_VALUE))
	{
		printf("Error:Invalid handles!");
		system("pause");
		exit(0);
	}

	char message[6];
	if (!ReadFile(hStdin, message, strlen(message), &dwRead, NULL) || dwRead == 0)
	{
		printf("Cannot read message of parent!");
		system("pause");
		exit(0);
	}
	int day = atoi(message); //if day is assingned to mes, there is no error in reading pipe

	handles = malloc(sizeof(HANDLE)* threadCount);
	lpParameter = malloc(sizeof(THREAD_PARAMETERS)* threadCount);
	threadID = malloc(sizeof(int)* threadCount);

	int milkCount = 0;
	int biscuitCount = 0;
	int chipsCount = 0;
	int cokeCount = 0;
	int i = 0;
	for (i = 0; i < threadCount; i++)
	{
		lpParameter[i].threadNo = i + 1;
		if (lpParameter[i].threadNo == 1)  //threads share items at below, each thread read one item, and in a child their days are equal.
		{
			lpParameter[i].item = "MILK";
			lpParameter[i].day = day;
			lpParameter[i].itemCount = 0; //item count will be incremented after threads read their identical items.
		}
		else if (lpParameter[i].threadNo == 2)
		{
			lpParameter[i].item = "BISCUIT";
			lpParameter[i].day = day;
			lpParameter[i].itemCount = 0;
		}
		else if (lpParameter[i].threadNo == 3)
		{
			lpParameter[i].item = "CHIPS";
			lpParameter[i].day = day;
			lpParameter[i].itemCount = 0;
		}
		else
		{
			lpParameter[i].item = "COKE";
			lpParameter[i].day = day;
			lpParameter[i].itemCount = 0;
		}
		handles[i] = CreateThread(NULL, 0, threadWork, &lpParameter[i], 0, &threadID[i]);

		if (handles[i] == INVALID_HANDLE_VALUE)
		{
			printf("error when creating thread\n");
			system("pause");
			exit(0);
		}
	}//end for
	printf("selam\n");
	WaitForMultipleObjects(threadCount, handles, TRUE, INFINITE);
	// at that point threads finished their works on threadWork function and we have neccesary informations in their structures
	for (i = 0; i < threadCount; i++)
	{
		if (i == 0) { //item counts are taken from threads
			milkCount = lpParameter[i].itemCount;
		}
		if (i == 1) {
			biscuitCount = lpParameter[i].itemCount;
		}
		if (i == 2) {
			chipsCount = lpParameter[i].itemCount;
		}
		if (i == 3) {
			cokeCount = lpParameter[i].itemCount;
		}
		CloseHandle(handles[i]); //at the end close threads
	}

	char buffer[256];
	sprintf_s(buffer, sizeof(buffer), "%d%d%d%d%d", day, milkCount, biscuitCount, chipsCount, cokeCount); //neccessary informations will be sent with that pattern

	int bytesToWrite = 0;
	int bytesWritten = 0;
	bytesToWrite = strlen(buffer);
	bytesToWrite++;

	if (!WriteFile(hStdout, buffer, bytesToWrite, &bytesWritten, NULL))
	{
		printf("Error in writing to pipe!");
		system("pause");
		exit(0);
	}
	//at that point writing pipe is completed succesfully,parent will read informations from pipe and show final results.
	free(handles);
	free(lpParameter);
	free(threadID);

	system("pause");
	return 1;
}

DWORD WINAPI threadWork(LPVOID parameters)
{
	char str1[] = "START"; //this is to control day of thread is equal to day in file
	char str2[] = "END";   //this is to control thread read its day part and stop there

	THREAD_PARAMETERS* param = (THREAD_PARAMETERS*)parameters;
	FILE *stream; //every thread has its own stream to prevent data inconsistency
	errno_t err;
	char * pch;  //for parsing

	int day_temp = 0; //this is to control thread day and day in file is equal
	char line[1000];
	char *rest = line;
	int itemCount = 0;

	if ((err = fopen_s(&stream, "market.txt", "r")) == 0) { //file is opened
		while (fgets(line, sizeof line, stream) != NULL) { //a line from file is read
			pch = strtok_s(line, "# ,()_", &rest);   //that line is parsed
			if (strcmp(pch, str1) == 0) {  //if first word is START, take two times strtok and reach day in file, control
				pch = strtok_s(NULL, "# ,()_", &rest);
				pch = strtok_s(NULL, "# ,()_", &rest);
				day_temp = atoi(pch);
				pch = strtok_s(NULL, "# ,()_", &rest);// we reached day in file, these two line is finishes the line and we pass another line
				pch = strtok_s(NULL, "# ,()_", &rest);
			}
			while (pch != NULL) {  //find  the day in text and compare with thread day
				if (param->day == day_temp) {  //if they are equal, thread will update its itemCount in that part
					if (strcmp(pch, param->item) == 0) {
						itemCount++;
					}
					pch = strtok_s(NULL, "# ,()", &rest);
				}
				else { //else just skip the unneccessary lines.
					pch = strtok_s(NULL, "# ,()_", &rest);
				}

			}
		}
	}
	else //if there is an error to open file, give an error
	{
		fprintf(stderr, "Cannot open file, error %d\n", err);
	}
	fclose(stream); //close file

	param->itemCount = itemCount; //every thread updates itemCount in its structure.

	return 1;
}