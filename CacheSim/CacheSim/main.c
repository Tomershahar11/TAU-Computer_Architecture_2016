/*
Main.c: cache simulator.exe
- This program manages all runnings of TestFiles.
- Each file will be sent by TestManager to a set of tests, as a different process.
- The TestManager also outputs a status log file by sampling the processes every time set, noted by 'processStatusCheckFrequency'
- The testManager finishes its work only when all processes are finished running.
Tomer Shahar 301359410, Lior Bialik 301535316
*/




/* Constants: */
#define TIMEOUT_IN_MILLISECONDS 5000
#define BRUTAL_TERMINATION_CODE 0x55
#define RUN 2
#define FINISH 1


/* Libraries: */
#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#include <stdio.h>
#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <tchar.h>
#include <Strsafe.h>
#include <conio.h>
#include <process.h>
#include <sys/stat.h>
#include "TestManager.h"
#include <sys/types.h>
#include <direct.h>




/* Function Declarations: */
char *fileTestOutputLogPathCreation(char*, char*);
char *createRunTimeLogFileInsideOutputDirName(char *outpuDirName, char *runTime_logFileName);
BOOL CreateProcessSimple(LPTSTR CommandLine, PROCESS_INFORMATION *ProcessInfoPtr);
LPTSTR ConvertCharStringToLPTSTR(const char *Source);
//void printListOfProcess(HANDLE *handleProcessArray, DWORD *exitcodeArray);
//void *getDynamicAllocationArrayOfDWORD(DWORD *nameArray, int MUL);
int CountNumOfTests(FILE *fileInput);
int *CountLengthOfEachTest(FILE *fileInput, int TotalNumberOfFiles);
char *FullCommandLineStringCreation(FILE *fileInput, int FilesToTestLength, char *outpuDirName);
void checkProcessStatus(DWORD waitcode, FILE *runTime_logFileOutput, PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, DWORD *exitcodeArray, int TotalNumberOfFiles, int AllProcessFinished);
void PrintSortedProcess(int *exitcodeArray, int TotalNumberOfFiles, PROCESS_INFORMATION *ProcessInfoPtr);
void bubble_sort(PROCESS_INFORMATION *ProcessInfoPtr, HANDLE *handleProcessArray, int n);





int main(int argc, char *argv[]) {
	/* Internal Declarations: */
	FILE *fileInput = NULL; FILE *runTime_logFileOutput = NULL; int i; int AllProcessFinished = 0;
	char *fileName = NULL; char *outputFileName = NULL;
	char *outpuDirName = NULL; LPTSTR dirNameLPTSTR = NULL; char *loopTime = NULL;
	char *runTime_logFileName = NULL;
	char *outputLogFile = NULL; char *FileToTestName = NULL;
	int TotalNumberOfFiles = 0; int *FilesToTestLengthArray = NULL; char *CommandLineArguentStringArray[] = { NULL };
	fileName = argv[1];
	outpuDirName = argv[2];
	loopTime = argv[3];
