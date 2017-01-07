/*
Cache sim - main.c:
- The program simulates two configurations of a computer cache.
- The program calls CacheSimulator executer and returns 0 if it succeeds.
Tomer Shahar 301359410, Or Shoshani
*/

/* Libraries: */
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "CacheSimulator.h"

int main(int argc, char *argv[]) {
	int returnValue = 0;
	// Define argv[] variables
	int L1CacheBlockSize = 0, L2CacheBlockSize = 0, CacheLevel = 0;
	char *TraceFileName = NULL, *MemInFileName = NULL, *MemOutFileName = NULL, *L1CacheFileName = NULL, *L2Way0CacheFileName = NULL, *L2Way1CacheFileName = NULL, *StatsFileName = NULL;
	//char *CommandFileName = NULL, *BalanceReportFileName = NULL, *RunTimeLogFileName = NULL;


	// Verify that the number of command line argument is correct
	if (argc != 11) {
		printf("Number of Command line Arguments isn't compatible,  error %ul\n", GetLastError());
		exit(1);
	}

	CacheLevel = atoi(argv[1]);
	L1CacheBlockSize = atoi(argv[2]);
	L2CacheBlockSize = atoi(argv[3]);
	TraceFileName = argv[4];
	MemInFileName = argv[5];
	MemOutFileName = argv[6];
	L1CacheFileName = argv[7];
	L2Way0CacheFileName = argv[8];
	L2Way1CacheFileName = argv[9];
	StatsFileName = argv[10];

	returnValue =  CacheSimulator(CacheLevel, L1CacheBlockSize, L2CacheBlockSize, TraceFileName, MemInFileName, MemOutFileName, L1CacheFileName, L2Way0CacheFileName, L2Way1CacheFileName, StatsFileName);

	return returnValue;
}

