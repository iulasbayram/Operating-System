
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>

#define NO_OF_PROCESS 7  //we have 7 child processes

HANDLE childStdInRead[NO_OF_PROCESS];
HANDLE childStdInWrite[NO_OF_PROCESS];
HANDLE childStdOutRead[NO_OF_PROCESS];
HANDLE childStdOutWrite[NO_OF_PROCESS];

void WriteToPipe(void);  //to send day informations to child processes.
void ErrorExit(PTSTR);

typedef struct  //after receiving neccesary informations from child processes, informations is kept in that structure.
{
	int cokeCount;
	int milkCount;
	int biscuitCount;
	int chipsCount;
	int day;
}INFORMATIONS;

void mostSoldItemForEachDay();
void totalSoldItem();


int main()
{
	HANDLE processHandles[NO_OF_PROCESS];
	SECURITY_ATTRIBUTES sa[NO_OF_PROCESS];
	PROCESS_INFORMATION pi[NO_OF_PROCESS];
	STARTUPINFO si[NO_OF_PROCESS];
	INFORMATIONS* info = malloc(sizeof(INFORMATIONS)* NO_OF_PROCESS);

	int i = 0;
	for (i = 0; i < NO_OF_PROCESS; i++) {
		sa[i].nLength = sizeof(SECURITY_ATTRIBUTES);
		sa[i].bInheritHandle = TRUE;
		sa[i].lpSecurityDescriptor = NULL;
		//pipes are created at below.
		if (!CreatePipe(&childStdOutRead[i], &childStdOutWrite[i], &sa[i], 0))
			ErrorExit(TEXT("StdoutRd CreatePipe"));

		if (!SetHandleInformation(childStdOutRead[i], HANDLE_FLAG_INHERIT, 0))
			ErrorExit(TEXT("Stdout SetHandleInformation"));

		if (!CreatePipe(&childStdInRead[i], &childStdInWrite[i], &sa[i], 0))
			ErrorExit(TEXT("Stdin CreatePipe"));

		if (!SetHandleInformation(childStdInWrite[i], HANDLE_FLAG_INHERIT, 0))
			ErrorExit(TEXT("Stdin SetHandleInformation"));

		ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
		ZeroMemory(&si[i], sizeof(STARTUPINFO));
		si[i].cb = sizeof(STARTUPINFO);
		si[i].hStdError = childStdOutWrite[i];
		si[i].hStdOutput = childStdOutWrite[i];
		si[i].hStdInput = childStdInRead[i];
		si[i].dwFlags |= STARTF_USESTDHANDLES;

		if (!CreateProcess(NULL, "Child.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
			ErrorExit(TEXT("CreateProcess"));
		}
		else
		{
			processHandles[i] = pi[i].hProcess;  //creation is successful.
			printf("Child %d has created succesfully!\n", i + 1);
		}
	}//end for

	WriteToPipe(); //sending day informations via pipe.
	printf("\nAll day informations has sent to the childs by parent.\nWaiting to receive all informations from child...\n");

	WaitForMultipleObjects(NO_OF_PROCESS, processHandles, TRUE, INFINITE); //parent waits child to finish reading file and send neccessary informations to it.

																		   //At that point child finished its job, we have neccessary informations in pipe, next step is read them and show final results.
	DWORD dwRead;
	char information[256];
	printf("\nChild processes finished their works,all informations is reading by parent now...\n");

	for (i = 0; i < NO_OF_PROCESS; i++) {

		if (!ReadFile(childStdOutRead[i], information, strlen(information), &dwRead, NULL) || dwRead == 0) {
			printf("Error while reading from pipe!");
			system("pause");
			exit(0);
		} //if there is no error, we took final informations from child processes, I send a string like 21212, means indexes means day,milk,biscuit,chips and coke respectively.
		info[i].day = information[0] - '0';
		info[i].milkCount = information[1] - '0';
		info[i].biscuitCount = information[2] - '0';
		info[i].chipsCount = information[3] - '0';
		info[i].cokeCount = information[4] - '0';
	}//END OF READING
	 //Final step: show final results with functions below.
	printf("\nFinal informations is giving below...\n\n");
	
	mostSoldItemForEachDay(info);
	totalSoldItem(info);

	for (i = 0; i < NO_OF_PROCESS; i++) {
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}
	system("pause");
	return 1;
}

void WriteToPipe(void)

{
	int i = 0;
	int bytesToWrite = 0;
	int bytesWritten = 0;
	char buffer[20];

	for (i = 0; i < NO_OF_PROCESS; i++) {
		sprintf_s(buffer, sizeof(buffer), "%d", i + 1); //i+1 becomes string named buffer, this specifies the day of child processes 
		bytesToWrite = strlen(buffer);
		bytesToWrite++;

		if (!WriteFile(childStdInWrite[i], buffer, bytesToWrite, &bytesWritten, NULL))
		{
			system("pause");
			exit(0);
		} //if there is no error, day informations has been sent succesfullt
		if (!CloseHandle(childStdInWrite[i]))  //Close the pipe handle so the child process stops reading
			ErrorExit(TEXT("StdInWr CloseHandle"));
	}
}

void ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box, 
// and exit from the application.
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	system("pause");
	exit(0);
}

void mostSoldItemForEachDay(INFORMATIONS* finalInformations) {
	char *item = NULL;
	int i = 0;
	char buffer[100];

	for (i = 0; i < NO_OF_PROCESS; i++) {
		int mostDay = 0;
		if (finalInformations[i].milkCount >= mostDay) {
			mostDay = finalInformations[i].milkCount;
			sprintf_s(buffer, sizeof(buffer), "%s", "MILK");
		}
		if (finalInformations[i].biscuitCount >= mostDay) {
			if (finalInformations[i].biscuitCount == mostDay) {
				sprintf_s(buffer, sizeof(buffer), "%s %s", item, "BISCUIT");
			}
			else {
				mostDay = finalInformations[i].biscuitCount;
				sprintf_s(buffer, sizeof(buffer), "%s", "BISCUIT");
			}

		}
		if (finalInformations[i].chipsCount >= mostDay) {
			if (finalInformations[i].chipsCount == mostDay) {
				sprintf_s(buffer, sizeof(buffer), "%s %s", buffer, "CHIPS");
			}
			else {
				mostDay = finalInformations[i].chipsCount;
				sprintf_s(buffer, sizeof(buffer), "%s", "CHIPS");
			}

		}
		if (finalInformations[i].cokeCount >= mostDay) {
			if (finalInformations[i].cokeCount == mostDay) {
				sprintf_s(buffer, sizeof(buffer), "%s %s", buffer, "COKE");
			}
			else {
				mostDay = finalInformations[i].cokeCount;
				sprintf_s(buffer, sizeof(buffer), "%s", "COKE");
			}
		}
		printf("Most Sold item/items in Day%d :> %s with count %d.\n", finalInformations[i].day, buffer, mostDay);
	}
}
void totalSoldItem(INFORMATIONS* finalInformations) {
	int totalInWeek = 0;
	int i = 0;
	int milkCount = 0;
	int biscuitCount = 0;
	int chipsCount = 0;
	int cokeCount = 0;
	for (i = 0; i < NO_OF_PROCESS; i++) {
		int totalInDay = 0;
		totalInDay += finalInformations[i].milkCount;
		totalInDay += finalInformations[i].biscuitCount;
		totalInDay += finalInformations[i].chipsCount;
		totalInDay += finalInformations[i].cokeCount;
		milkCount += finalInformations[i].milkCount;
		biscuitCount += finalInformations[i].biscuitCount;
		chipsCount += finalInformations[i].chipsCount;
		cokeCount += finalInformations[i].cokeCount;
		printf("Total count of items sold in DAY%d: %d\n", finalInformations[i].day, totalInDay);
		totalInWeek += totalInDay;
	}
	
	printf("\nTotal count of milks sold in end of week: %d\n", milkCount);
	printf("Total count of biscuits sold in end of week: %d\n", biscuitCount);
	printf("Total count of chips in sold end of week: %d\n", chipsCount);
	printf("Total count of cokes in sold end of week: %d\n", cokeCount);

	int mostWeek = 0;
	char *item = NULL;
	for (i = 0; i < NO_OF_PROCESS; i++) {
		if (milkCount > mostWeek) {
			mostWeek = milkCount;
			item = "MILK";
		}
		if (biscuitCount > mostWeek) {
			mostWeek = biscuitCount;
			item = "BISCUIT";
		}
		if (chipsCount > mostWeek) {
			mostWeek = chipsCount;
			item = "CHIPS";

		}
		if (cokeCount > mostWeek) {
			mostWeek = cokeCount;
			item = "COKE";
		}
	}
	printf("\nMost Sold item in seven days => %s with count => %d\n\n", item, mostWeek);

	printf("\nTotal count of items in end of week: %d\n\n", totalInWeek);

}
