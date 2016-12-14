#pragma once
/*
ex3 - Header.h:
- The header file for BankManager.c which contains the execution for all commands in Command file.
- This file contains the definitions, used structs and the decleration which used to call the main
function in BankManager.c
*/

#ifndef BankManager_H
#define BankManager_H

///* Constants: */
//#define LONG_SLEEP_TIME 5000
//#define SHORT_SLEEP_TIME 5
//#define SECOND_DIVIDER 10000000000
//#define MILLI_SECOND_DIVIDER 1000000


/* Libraries: */
#include <Windows.h>


///* Struct Declarations: */
//typedef struct PROCESS_TIME_INF {
//	FILETIME CreationTime;
//	FILETIME ExitTime;
//	FILETIME KernelTime;
//	FILETIME UserTime;
//} PROCESS_TIME_INF;
//
//typedef struct FILE_TIME {
//	DWORD second;
//	DWORD millisecond;
//} FILE_TIME;

/* Function Declarations: */
int executeBankManager(int argc, char *argv[]);

#endif#pragma once
#pragma once