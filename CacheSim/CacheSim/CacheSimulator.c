/*
Cache sim - CacheSimulator.c:
- The program simulates two configurations of a computer cache:
* L1 - One level cache which interconnects directly to the Main memory.
* L1 + L2 - Two level caches, L1 interconnects from CPU to L2, and L2 interconnects from L1 to Main memory.
- The program also provides data statistics regrading the performance and characterization of the cache.

Input Arguments, entered by argv[] command line string:
@param levels: An integer parameter that indicates one of the configuration, '1' for L1 cache and '2' for two level caches.
@param b1 & b2: An integer numbers (base 10) that represents the ammount of each block size in bytes, respectively.
@param trace.txt: A text file which holds inside all CPU requests for Reading & Writing transactions from and to the cahce.
@param memin.txt: A text file which holds the main memory data before running the program, each line consists one byte starting from ADD 0x0.
@param memout.txt: A text file which holds the main memory data after running the program, each line consists one byte starting from ADD 0x0.
@param l1.txt: A text file which holds L1 cache data before running the program, each line consists one byte starting from ADD 0x0.
@param l2way0.txt: A text file which holds way 0 data of L2 cache before running the program, each line consists one byte starting from ADD 0x0.
* This parameter is valid only if we use two level configuration.
@param l2way1.txt: A text file which holds way 1 data of L2 cache before running the program, each line consists one byte starting from ADD 0x0.
* This parameter is valid only if we use two level configuration.
@param StatsInfoArray.txt: A text file which holds data statistics regarding running of the program, such as no. of write\read hits, no. of write\read misses, local\global miss rate, AMAT.

Output files:
@Output memout.txt- The text file holding the main memory data after running the program.
@Output l1.txt- The text file holding L1 cache data after running the program.
@Output l2way0.txt- The text file holding way 0 data of L2 cache after running the program.
@Output l2way1.txt- The text file holding way 1 data of L2 cache after running the program.
@Output StatsInfoArray.txt- The text file holding data statistics of the program after running.

*/

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "CacheSimulator.h"

/* Definitions: */
//#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'


#define DATA_BUS_WIDTH_CPU_TO_L1_IN_BITS 32
#define Address_IN_BITS 24
#define BUS_WIDTH_L1_TO_L2_IN_BITS 256
#define MainMem_SIZE_IN_BYTES 16777216
#define BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS 64
#define MAIN_MEM_MISS_PENALTY 100
#define L2CACHE_MISS_PENALTY 4

#define MISS_TIME -1							//TODO: what is it??
#define CACHE_L1_SIZE_IN_BYTES 4096				// L1 Directed-Mapped cache
#define CACHE_L2_SIZE_IN_BYTES 32768			// L2 Two-Way-Set-Associative with LRU replacement policy
#define WORD_IN_BITS 32
#define L1_HIT_TIME 1
#define L2_HIT_TIME 4

#define MAX_NUM_OF_CHARS_IN_LINE 20				// '1' for inst between Store/Load, '1' for L/S inst, '6' for ADD, '8' for DATA, '4' Spaces
#define NUM_OF_StatsInfoArray_CRITERIA 12				// StatsInfoArray.txt includes statisics regarding running of the program divided into 12 criterias.

/* Function Declerations */
int PrintError(int errNum);
int CountNumOfLines(FILE *fileInput);
int CastCharHexToDec(char ch);
int ConvertDataOrAddressToDecNum(char* LinePtr, int num);
void initializeStatsInfoArrayArray(int StatsInfoArray[12]);
int CreateMemory(int ways, int blockSize, int memSize, Memory* memory);
void InitializeMem(FILE* MemInFile, Memory* MainMem, int MemInTotalNumberOfLines);
void CopyMemoryIntoFile(Memory *memory, FILE *FilePtr, int level);
void CopyStatsInfoArrayFile(int StatsInfoArray[], FILE *StatsInfoArrayFile);
void Load(Memory *L1Cache, Memory *L2Cache, Memory *MainMem, int Address, int StatsInfoArray[], unsigned char *DataValueToLoad);
void Store(Memory *L1Cache, Memory *L2Cache, Memory *MainMem, int Address, int StatsInfoArray[], unsigned char *DataValueToStore);
void read(int Address, Memory *memory, int sizeAllocatedToReadDataInBytes, int BusSizeInBytes, unsigned char **DataValueToLoad, int *cycles, int ReplacementFactor);
int write(int Address, Memory* memory, int busWidth, unsigned char **dataToWrite, int Dirty);


/* Start of Program: */



int CacheSimulator(int CacheLevel, int L1CacheBlockSize, int L2CacheBlockSize, char *TraceFileName, char *MemInFileName, char *MemOutFileName, char *L1CacheFileName, char *L2Way0CacheFileName, char *L2Way1CacheFileName, char *StatsInfoArrayFileName) {
	
	/* Internal Declerations */
	FILE *TraceFile = NULL, *MemInFile = NULL, *MemOutFile = NULL, *L1CacheFile = NULL, *L2Way0CacheFile = NULL, *L2Way1CacheFile = NULL, *StatsInfoArrayFile = NULL;
	Memory *MainMem = NULL, *L1Cache = NULL, *L2Cache = NULL;
	int Address = 0, i = 0, j = 0, m = 0, reg_load = 0, data_val = 0, TraceTotalNumberOfLines = 0, MemInTotalNumberOfLines = 0;		// TODO: 
	int StatsInfoArray[NUM_OF_StatsInfoArray_CRITERIA] = { 0 };
	int ways[3] = { 1, 1, 2 };			// way[0] = 1 for MainMemory, way[1] = 1 for L1Cache (DM), way[2] = 2 for L2Cache (Two-Way-Set)
	long double x = 2;
	//int memSize=(int)(pow(x,24)/1);
	char InstructionLinePtr[MAX_NUM_OF_CHARS_IN_LINE];
	unsigned char *DataValueToLoad = NULL;
	unsigned char *DataValueToStore = NULL;

	// Memory allocations
	DataValueToLoad = (unsigned char*)malloc(sizeof(unsigned char));
	if (DataValueToLoad == NULL) {
		PrintError(2);
	}
	DataValueToStore = (unsigned char*)malloc((L1CacheBlockSize) * sizeof(unsigned char));
	if (DataValueToStore == NULL) {
		PrintError(2);
	}
	MainMem = (Memory*)malloc(sizeof(Memory));
	if (MainMem == NULL) {
		PrintError(2);
	}
	L1Cache = (Memory*)malloc(sizeof(Memory));
	if (L1Cache == NULL) {
		PrintError(2);
	}
	if (CacheLevel == 2) {
		L2Cache = (Memory*)malloc(sizeof(Memory));
		if (L2Cache == NULL) {
			PrintError(2);
		}
	}

	// Open TraceFile & MemInFile
	TraceFile = fopen(TraceFileName, "rt");
	if (!TraceFile) {
		PrintError(1);
	}

	MemInFile = fopen(MemInFileName, "rt");
	if (!MemInFile) {
		PrintError(1);
	}

	// Count number of lines in TraceFile & MemInFile
	TraceTotalNumberOfLines = CountNumOfLines(TraceFile);
	MemInTotalNumberOfLines = CountNumOfLines(MemInFile);


	// Initialize StatsInfoArray Array before entering Data statistics 
	initializeStatsInfoArrayArray(StatsInfoArray);


	// Create main memory according to input pararmeters sizes
	CreateMemory(ways[0], WORD_IN_BITS / 8, MainMem_SIZE_IN_BYTES, MainMem);

	// Create L1Cache memory according to input pararmeters sizes
	CreateMemory(ways[1], L1CacheBlockSize, CACHE_L1_SIZE_IN_BYTES, L1Cache);

	// If exists, Create L2Cache memory according to input pararmeters sizes
	if (CacheLevel == 2)
		CreateMemory(ways[2], L2CacheBlockSize, CACHE_L2_SIZE_IN_BYTES, L2Cache);

	// Filling main memory according to 'memin' text file initial data
	InitializeMem(MemInFile, MainMem, MemInTotalNumberOfLines);

	rewind(TraceFile);

	// Go over TraceFile and execute Load \ Store inst. according to required line
	do {
		if (!fgets(InstructionLinePtr, MAX_NUM_OF_CHARS_IN_LINE, TraceFile)) {
			printf("Couldn't read line from file,  error %ul\n", GetLastError());
			exit(1);
		}

		if (InstructionLinePtr[2] == 'L') {
			// Load instruction
			StatsInfoArray[0] += InstructionLinePtr[0] - '0';					// Count the number of cycles from last L\S inst. to the current one
			Address = ConvertDataOrAddressToDecNum(InstructionLinePtr, 6);
			Load(L1Cache, L2Cache, MainMem, Address, StatsInfoArray, DataValueToLoad);
		}

		if (InstructionLinePtr[2] == 'S') {
			// Store instruction
			DataValueToStore = (unsigned char*)ConvertDataOrAddressToDecNum(InstructionLinePtr, 8);
			Address = ConvertDataOrAddressToDecNum(InstructionLinePtr, 6);
			StatsInfoArray[0] += InstructionLinePtr[0] - '0';					// Count the number of cycles from last L\S inst. to the current one
			for (i = 0; i < (WORD_IN_BITS / 8); ++i) {
				DataValueToStore[i] = (unsigned char)ConvertDataOrAddressToDecNum(InstructionLinePtr + 11 + i * 2, 2);		// Insert data value as chunks of bytes into DataValueToStore
				Store(L1Cache, L2Cache, MainMem, Address, StatsInfoArray, DataValueToStore);
			}
			//StatsInfoArray[0] += str[0] - '0';	// why??
			//c = fgetc(fin_trace);
		}

		j++;
	} while (j < TraceTotalNumberOfLines);

	// Copying Main Memory content into MemOutFile
	MemOutFile = fopen(MemOutFileName, "w");
	if (!MemOutFile) {
		PrintError(1);
	}
	CopyMemoryIntoFile(MainMem, MemOutFile, 0);
	fclose(MemOutFile);

	// Copying L1Cache content into L1CacheFile
	L1CacheFile = fopen(L1CacheFileName, "w");
	if (!L1CacheFile) {
		PrintError(1);
	}
	CopyMemoryIntoFile(L1Cache, L1CacheFile, 0);
	fclose(L1CacheFile);

	// Copying L2Cache content into L2CacheFile if exists
	if (CacheLevel == 2) {
		// way 0
		L2Way0CacheFile = fopen(L2Way0CacheFileName, "w");
		if (!L2Way0CacheFile) {
			PrintError(1);
		}
		CopyMemoryIntoFile(L2Cache, L2Way0CacheFile, 0);
		fclose(L2Way0CacheFile);
		// way 1
		L2Way1CacheFile = fopen(L2Way1CacheFileName, "w");
		if (!L2Way1CacheFile) {
			PrintError(1);
		}
		CopyMemoryIntoFile(L2Cache, L2Way1CacheFile, 1);
		fclose(L2Way1CacheFile);
	}

	// Copying StatsInfoArray content into StatsInfoArrayFile 
	StatsInfoArrayFile = fopen(StatsInfoArrayFileName, "w");
	CopyStatsInfoArrayFile(StatsInfoArray, StatsInfoArrayFile);
	fclose(StatsInfoArrayFile);
	fclose(TraceFile);

	return 0;
}


/* Function Definitions */
int PrintError(int errNum) {

	switch (errNum) {
	case 1:
		printf("cannot open file\n");
		exit(1);
	case 2:
		printf("Allocating memory failed\n");
		exit(2);
	}
	printf("Unknown error occured\n");
	exit(99);
}

int CountNumOfLines(FILE *fileInput) {
	// fgetc() is inspired by http://stackoverflow.com/questions/12733105/c-function-that-counts-lines-in-file
	int ch = 0;
	int TotalNumberOfLines = 0;
	while (!feof(fileInput)) {
		ch = fgetc(fileInput);
		if ((ch == '\n') || (ch == EOF)) {
			TotalNumberOfLines++;
		}
	}
	// Resetting pointer to the start of file
	rewind(fileInput);

	//fclose(fileInput);
	return TotalNumberOfLines;
}

int CastCharHexToDec(char ch) {

	int Dec = 0;
	if (ch >= '0' && ch <= '9')
		Dec = (ch - '0');
	if (ch >= 'A' && ch <= 'F')
		Dec = (ch - 'A') + 10;

	return Dec;
}

int ConvertDataOrAddressToDecNum(char* LinePtr, int num) { //TODO: Need to update comments
	/* This function gets an argument called CommandLinePtr, a pointer to a line in trace.txt, and another argument called num, an integer indicating what type of conversion should be done.
	If num equals 6, The program should convert hex Address.
	Else The program should convert hex data.
	Either conversions should output a decimal number.
	*/
	int i = 0, val = 0, weight = 0;
	int power = num - 1;
	int decimalNumber = 0;
	long double base = 16;
	char ch;

	for (i = 0; i<num; i++) {
		if (num == 2)
			ch = LinePtr[0 + i];				//for the init memory action
		if (num == 6)
			ch = LinePtr[4 + i];				// Address value in LinePtr begins in the 4th-9th char in trace.txt
		if (num == 8)
			ch = LinePtr[11 + i];				// Data value in LinePtr begins in the 11th-18th char in trace.txt

		val = CastCharHexToDec(ch);
		weight = (int)pow(base, power);
		decimalNumber += (val * weight);
		power--;
	}
	return decimalNumber;
}

void initializeStatsInfoArrayArray(int StatsInfoArray[12]) {	// TODO: Need to remove value inside StatsInfoArrayArray, why not to sent StatsInfoArrayArray as a pointer

	for (int i = 0; i < 12; i++) {
		StatsInfoArray[i] = 0;
	}
}

int CreateMemory(int ways, int blockSize, int memSize, Memory* memory) {

	int i = 0, j = 0, k = 0, NumOfBlocks = memSize / blockSize;
	Block *blocks = NULL;

	// Allocate blocks in size of Block
	blocks = (Block*)malloc((memSize * ways) * sizeof(Block));
	if (blocks == NULL) {
		PrintError(2);
	}
	memory->blocks = (Block**)malloc(ways * sizeof(Block*));
	if (memory->blocks == NULL) {
		PrintError(2);
	}

	// Placing blocks inside memory by shifting the blocks according to ways in memory
	for (i = 0; i < ways; i++) {
		memory->blocks[i] = blocks + i*(NumOfBlocks);
	}
	for (i = 0; i<ways; i++) {
		// Create arrays of bytes and replace into blocks
		for (j = 0; j<NumOfBlocks; j++) {
			// Assign number of bytes according to blockSize in each 'block' in memory martrix
			memory->blocks[i][j].bytes = (unsigned char*)calloc(blockSize, sizeof(unsigned char));
			for (k = 0; k < blockSize; k++) {
				// In each block, initialize fields of Block
				memory->blocks[i][j].bytes[k] = 0;
				memory->blocks[i][j].dirty = 0;
				memory->blocks[i][j].valid = 0;
				memory->blocks[i][j].tag = 0;
				memory->blocks[i][j].LRU = 0;
			}
		}
	}
	// Assign input arguments representing the memory into Block fields 
	memory->blockSize = blockSize;
	memory->size = memSize;
	memory->ways = ways;

	return 0;
}

void InitializeMem(FILE* MemInFile, Memory* MainMem, int MemInTotalNumberOfLines) {
	int i = 0, k = 0;
	char BytePtr[4];
	long double base = 2;

	// Calculating Memory Address space
	int Memory_Size = (int)(pow(base, Address_IN_BITS) / 1);

	for (k = 0; k<(Memory_Size / 4) - 1; k++) {
		// Initialize bytes & valid fields of MainMem
		MainMem->blocks[0][k / 4].bytes[k % 4] = 0;
		MainMem->blocks[0][k / 4].valid = 1;
	}

	// go over MemInFile, catch a different Byte each time, and place it inside its matching position in Main memory
	while ((fgets(BytePtr, 4, MemInFile) != NULL) && (i<MemInTotalNumberOfLines)) {
		MainMem->blocks[0][i / 4].bytes[i % 4] = (unsigned char)ConvertDataOrAddressToDecNum(BytePtr, 2);
		MainMem->blocks[0][i / 4].valid = 1;					// Valid is '1' as a result of copying real data inside memory. 
		++i;
	}
	// At the end of this loop, blocks will hold useful data

	//if the number of lines in 'memin' text file is smaller than the size of the MainMem -> Initialize remained MainMem into 0
	MainMem->ways = 1;

}

void CopyMemoryIntoFile(Memory *memory, FILE *FilePtr, int level) {
	//This function gets a pointer to a relevant File and filling it by the corresponding Memory content.
	int i = 0, cnt = 0;

	for (i = 0; i<memory->size; i++) {
		if (cnt % 30 == 0) {
			if (cnt != 0) {
				fprintf(FilePtr, "\n");
			}
		}
		if (i % (memory->blockSize) == 0) {
			if (i != 0) {
				cnt = 0;
				fprintf(FilePtr, "\n");
				fprintf(FilePtr, "\n");
			}
		}
		fprintf(FilePtr, "%4u", memory->blocks[level][i / memory->blockSize].bytes[i % memory->blockSize]);
		cnt++;
	}
	fprintf(FilePtr, "\n");
}

void CopyStatsInfoArrayFile(int StatsInfoArray[], FILE *StatsInfoArrayFile) {

	int i = 0;
	char *DataStatisticsName[NUM_OF_StatsInfoArray_CRITERIA] = {
		"Program running time in cycles",
		"Number of read hits in L1",
		"Number of write hits in L1",
		"Number of read misses in L1",
		"Number of write misses in L1",
		"Number of read hits in L2",
		"Number of write hits in L2",
		"Number of read misses in L2",
		"Number of write misses in L2",
		"L1 local miss rate",
		"Global miss rate",
		"AMAT"
	};

	for (i = 0; i < NUM_OF_StatsInfoArray_CRITERIA; i++) {
		fprintf(StatsInfoArrayFile, "%s is %d\n", DataStatisticsName[i], StatsInfoArray[i]);
	}

}

void Load(Memory *L1Cache, Memory *L2Cache, Memory *MainMem, int Address, int StatsInfoArray[], unsigned char *DataValueToLoad) {

	int cycles = 0, i = 0, index = 0, offset = 0, tempOffset = 0;
	unsigned char *TempDataValueToLoad;

	// Try first to read the required data from the specified Address in L1Cache
	read(Address, L1Cache, WORD_IN_BITS / 8, (DATA_BUS_WIDTH_CPU_TO_L1_IN_BITS / 8), &DataValueToLoad, &cycles, 0);
	//cycles=-1;

	if (cycles != MISS_TIME) {
		// We have a hit in L1
		StatsInfoArray[1]++;
		// Update number of cycles for hit in L1
		StatsInfoArray[0] += cycles;
		return;
	}

	// We have a miss in L1, go to further memories
	StatsInfoArray[3]++;
	cycles = 0;

	// Check if L2Cache configuration exists
	if (L2Cache != NULL) {
		// Try second to read the required data from the specified Address in L2Cache
		read(Address, L2Cache, L1Cache->blockSize, (BUS_WIDTH_L1_TO_L2_IN_BITS / 8), &DataValueToLoad, &cycles, 0);
		StatsInfoArray[0] += L2CACHE_MISS_PENALTY;

		//	cycles=-1;
		if (cycles != MISS_TIME) {
			// We have a hit in L2
			StatsInfoArray[5]++;
			// After retrieving with valid value from L2, update the value in L1 as well
			cycles += write(Address, L1Cache, BUS_WIDTH_L1_TO_L2_IN_BITS / 8, &DataValueToLoad, 0);
			StatsInfoArray[0] += cycles;
			return;
		}
		else {
			// We have a miss in L2, go to Main memory
			StatsInfoArray[7]++;
		}
	}

	// Load the required data from MainMem
	cycles = 0;
	if (L2Cache == NULL) {
		read(Address, MainMem, L1Cache->blockSize, (BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8), &DataValueToLoad, &cycles, 0);
	}
	if (L2Cache != NULL) {
		read(Address, MainMem, L2Cache->blockSize, (BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8), &DataValueToLoad, &cycles, 0);
		cycles += write(Address, L2Cache, BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8, &DataValueToLoad, 0);
		cycles += L2CACHE_MISS_PENALTY;
		offset = ((Address % (L2Cache->size / L2Cache->ways)) % L2Cache->blockSize);
		tempOffset = (offset / L1Cache->blockSize) * L1Cache->blockSize;
		TempDataValueToLoad = (unsigned char *)malloc(L1Cache->blockSize * sizeof(unsigned char));

		for (i = 0; i < L1Cache->blockSize; i++) {
			TempDataValueToLoad[i] = DataValueToLoad[tempOffset + i];
			cycles += write(Address, L1Cache, BUS_WIDTH_L1_TO_L2_IN_BITS / 8, &TempDataValueToLoad, 0);
		}
	}
	else {
		cycles += write(Address, L1Cache, BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8, &DataValueToLoad, 0);
	}

	StatsInfoArray[0] += cycles + MAIN_MEM_MISS_PENALTY;

	return;
}

void Store(Memory *L1Cache, Memory *L2Cache, Memory *MainMem, int Address, int StatsInfoArray[], unsigned char *DataValueToStore) {

	int cycles = 0, i = 0, j = 0, k = 0, tempOffset = 0, waysToWrite = 0, n = 0;
	unsigned char *TempDataValueToStore = NULL, *tempArray = NULL, *arrayForL2Cache = NULL;
	int index = ((Address % (L1Cache->size / L1Cache->ways)) / L1Cache->blockSize);
	int indexL2Cache = 0, indexMainMem = 0;

	// Check if L2Cache exists
	if (L2Cache != NULL) {
		// Create indexes for L2Cache & MainMem as well
		indexL2Cache = (((Address % (L2Cache->size / L2Cache->ways)) / L2Cache->blockSize));
		indexMainMem = (Address / L2Cache->blockSize) * L2Cache->blockSize;
	}

	read(Address, L1Cache, L1Cache->blockSize, (DATA_BUS_WIDTH_CPU_TO_L1_IN_BITS / 8), &TempDataValueToStore, &cycles, 0);
	if (cycles != MISS_TIME) {
		// We have a hit in L1
		cycles = 0;
		tempOffset = (((Address % (L1Cache->size / L1Cache->ways)) % L1Cache->blockSize) / (WORD_IN_BITS / 8)) * (WORD_IN_BITS / 8);
		for (i = 0; i < WORD_IN_BITS / 8; i++) {
			TempDataValueToStore[i + tempOffset] = DataValueToStore[i];
		}
		write(Address, L1Cache, BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8, &TempDataValueToStore, 1);
		StatsInfoArray[0]++;
		// We have a write hit in L1
		StatsInfoArray[2]++;
		return;
	}

	// We have a write miss in L1, go to further memories
	StatsInfoArray[4]++;
	free(TempDataValueToStore);
	// Check if L2Cache configuration exists
	if (L2Cache != NULL) {
		read(Address, L2Cache, L1Cache->blockSize, (BUS_WIDTH_L1_TO_L2_IN_BITS / 8), &TempDataValueToStore, &cycles, 0);
		StatsInfoArray[0] += L2CACHE_MISS_PENALTY;

		if (cycles != MISS_TIME) {
			// We have a write hit in L2
			StatsInfoArray[6]++;
			cycles = 0;
			// Check both sets 
			for (i = 0; i<L2Cache->ways; i++) {
				// Replacement
				if (L1Cache->blocks[0][index].dirty == 1) {
					free(tempArray);
					tempArray = (unsigned char*)malloc(L2Cache->blockSize * sizeof(unsigned char*));
					read(Address, L1Cache, L1Cache->blockSize, (BUS_WIDTH_L1_TO_L2_IN_BITS / 8), &tempArray, &cycles, 1);
					read(Address, L2Cache, L2Cache->blockSize, (BUS_WIDTH_L1_TO_L2_IN_BITS / 8), &arrayForL2Cache, &cycles, 0);
					tempOffset = (((Address % (L2Cache->size / L2Cache->ways)) % L2Cache->blockSize) / (L2Cache->blockSize)) * (L2Cache->blockSize);
					for (i = 0; i < L1Cache->blockSize; i++) {
						arrayForL2Cache[i + tempOffset] = tempArray[i];
					}
					cycles = 0;
					StatsInfoArray[0] += write(Address, L2Cache, BUS_WIDTH_L1_TO_L2_IN_BITS / 8, &tempArray, 1);
				}
				tempOffset = (((Address % (L1Cache->size / L1Cache->ways)) % L1Cache->blockSize) / (WORD_IN_BITS / 8)) * (WORD_IN_BITS / 8);
				for (i = 0; i < WORD_IN_BITS / 8; i++) {
					TempDataValueToStore[i + tempOffset] = DataValueToStore[i];
				}
				write(Address, L1Cache, BUS_WIDTH_L1_TO_L2_IN_BITS / 8, &TempDataValueToStore, 1);
				StatsInfoArray[0]++;
				cycles++;
				return;
			}
		}

		StatsInfoArray[8]++;//write miss in L2
		free(TempDataValueToStore);
		read(Address, MainMem, L2Cache->blockSize, (BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8), &TempDataValueToStore, &cycles, 0);
		cycles = 0;
		// Check both sets
		for (i = 0; i<L2Cache->ways; i++) {
			if (L2Cache->blocks[i][index].LRU == 0) {
				waysToWrite = i;
				break;
			}
			waysToWrite = i;
		}
		if (L2Cache->blocks[waysToWrite][index].dirty == 1)	{
			read(Address, L2Cache, L2Cache->blockSize, (BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8), &tempArray, &cycles, 0);
			cycles = 0;
			for (j = 0; j<L2Cache->blockSize / BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8; j++) {
				for (k = 0; k<(BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / WORD_IN_BITS); k++) {
					for (n = 0; n<WORD_IN_BITS / 8; n++) {
						//StatsInfoArray[0]+=write(Address,MainMem,MAIN_BUS/8,&tmp_arr,1);
						MainMem->blocks[0][indexMainMem].bytes[n] = tempArray[n + k * 4 + j * 8];
					}
					indexMainMem++;
				}
				StatsInfoArray[0]++;
			}

			tempOffset = (((Address % (L2Cache->size / L2Cache->ways)) % L2Cache->blockSize) / (WORD_IN_BITS / 8)) * (WORD_IN_BITS / 8);
			for (i = 0; i < WORD_IN_BITS / 8; i++) {
				TempDataValueToStore[i + tempOffset] = DataValueToStore[i];
			}
			StatsInfoArray[0] += write(Address, L2Cache, BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8, &TempDataValueToStore, 1);
			StatsInfoArray[0] += MAIN_MEM_MISS_PENALTY;
			cycles++;

			return;
		}
	}
	else {
		// L2Cache configuration doesn't exists
		read(Address, MainMem, L1Cache->blockSize, (BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8), &TempDataValueToStore, &cycles, 0);
		StatsInfoArray[0] += MAIN_MEM_MISS_PENALTY;
		if (L1Cache->blocks[0][index].dirty == 1) {
			free(tempArray);
			read(Address, L1Cache, L1Cache->blockSize, (BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8), &tempArray, &cycles, 0);
			indexMainMem = (Address / L1Cache->blockSize)*L1Cache->blockSize;
			for (j = 0; j<L1Cache->blockSize / BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / 8; j++) {
				for (k = 0; k<BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS / WORD_IN_BITS; k++) {
					for (n = 0; n<WORD_IN_BITS / 8; n++) {
						//StatsInfoArray[0]+=write(Address,MainMem,MAIN_BUS/8,&tmp_arr,1);
						MainMem->blocks[0][indexMainMem].bytes[n] = tempArray[n + k * 4 + j * 8];
					}
					indexMainMem++;
				}
				StatsInfoArray[0]++;
			}
		}
	}
}

void read(int Address, Memory *memory, int sizeAllocatedToReadDataInBytes, int BusSizeInBytes, unsigned char **DataValueToLoad, int *cycles, int ReplacementFactor) {

	int i = 0, j = 0, k = 0, d = 0, BusTransferringAmount = 0, mode = 0, firstCheck = 1,
		blockTranfferingAmount = 0, byteOffsetInBlock = 0, byteOffsetInReadingData = 0;

	int offset = ((Address % (memory->size / memory->ways)) % memory->blockSize);
	int index = ((Address % (memory->size / memory->ways)) / memory->blockSize);
	int tag = Address / (memory->size / memory->ways);
	int currentBlock = index; 
	int NumberOfBytesToRead = min(BusSizeInBytes, memory->blockSize);
	NumberOfBytesToRead = min(NumberOfBytesToRead, sizeAllocatedToReadDataInBytes);		
	unsigned char *tempArray = NULL;
	tempArray = (unsigned char *)malloc(sizeAllocatedToReadDataInBytes * sizeof(unsigned char));

	if (BusSizeInBytes > sizeAllocatedToReadDataInBytes) {
		// Block size allocated to put data inside is smaller than Bus width, we can read data in one transaction	
		BusTransferringAmount = 1;
	}
	else {
		// Block size allocated to put data inside is bigger than Bus Width, we should able to read data in a couple of transactions
		BusTransferringAmount = sizeAllocatedToReadDataInBytes / BusSizeInBytes;
	}

	if (sizeAllocatedToReadDataInBytes <= memory->blockSize) {
		// Block size allocated to put data inside is smaller than Block size in memory that sends the data
		if (BusSizeInBytes <= memory->blockSize) {
			mode = 0;
			blockTranfferingAmount = 1;
		}

		if (BusSizeInBytes > memory->blockSize)
		{
			mode = 1;
			blockTranfferingAmount = BusSizeInBytes / memory->blockSize;
		}
	}
	else {
		// Block size allocated to put data inside is bigger than Block size in memory that sends the data
		if (BusSizeInBytes <= memory->blockSize)
		{
			mode = 2;
			blockTranfferingAmount = 1;
		}

		if (BusSizeInBytes > memory->blockSize)
		{
			mode = 3;
			blockTranfferingAmount = BusSizeInBytes / memory->blockSize;
		}
	}

	// Fill Reference variables for reading data according to given arguments
	for (j = 0; j<sizeAllocatedToReadDataInBytes; j++) {
		// Transfer data according to BusSizeInBytes recurring 
		for (d = 0; d<blockTranfferingAmount; d++) {
			switch (mode) {
			case 0:
				// Block size allocated to put data inside is smaller than busWidth that is smaller or equal than Block size in memory that sends the data
				currentBlock = index;
				byteOffsetInBlock = memory->blockSize * (offset / memory->blockSize) + (j * BusSizeInBytes);
				// By dividing offset in memory->blockSize we choose a byte in sizeAllocatedToReadDataInBytes
				// Then, we get the sub block in sizeAllocatedToReadDataInBytes we need to read, then we multiply by memory->blockSize to get the exact byte we need to start reading from
				// Adding (j * BusSizeInBytes_size) gives us the offset we get from previous bytes we read
				byteOffsetInReadingData = j * BusSizeInBytes;
				break;

			case 1:
				// Block size allocated to put data inside is smaller than busWidth that is bigger than Block size in memory that sends the data
				currentBlock = index;
				byteOffsetInBlock = memory->blockSize * (offset / memory->blockSize);
				// This returns the exact byte we start reading from
				byteOffsetInReadingData = 0;
				break;

			case 2:
				// Block size allocated to put data inside is bigger than busWidth that is smaller or equal than Block size in memory that sends the data
				currentBlock = (index / sizeAllocatedToReadDataInBytes) * sizeAllocatedToReadDataInBytes;
				byteOffsetInBlock = (j * BusSizeInBytes) / memory->blockSize;
				byteOffsetInReadingData = j * BusSizeInBytes;
				break;

			case 3:
				// Block size allocated to put data inside is bigger than busWidth that is bigger than Block size in memory that sends the data
				currentBlock = (index / sizeAllocatedToReadDataInBytes) * sizeAllocatedToReadDataInBytes + (j * BusSizeInBytes) + d;
				// Index chooses a block in memory->blocks. We need to read a bigger block than the block in memory->blocks
				// We dividing index by sizeAllocatedToReadDataInBytes in order to get the block group we need to read 
				// Then we again multiply by sizeAllocatedToReadDataInBytes to get the exact index we need to start reading from
				// Adding (j * BusSizeInBytes) in order to reach the indexes we read in previous bus runs so we keep reading the next blocks
				// Adding d in order to get the blocks inside each bus
				byteOffsetInBlock = 0;		
				byteOffsetInReadingData = (j * BusSizeInBytes) + (d * memory->blockSize);
				break;
			}

			if ( (memory->blocks[i][currentBlock].tag != tag || memory->blocks[i][currentBlock].valid != 1) && (firstCheck == 1) && (memory->ways == 2) ) {
				// Conditions aren't met to read data from memory, continue
				j++;
			}
			firstCheck = 0;

			if ( (memory->blocks[i][currentBlock].tag == tag) && (memory->blocks[i][currentBlock].valid == 1) ) {
				// Conditions is met to read data from memory, read data from memory in chunks of bytes
				for (k = 0; k<NumberOfBytesToRead; k++) {
					tempArray[k + byteOffsetInReadingData] = memory->blocks[i][currentBlock].bytes[k + byteOffsetInBlock];
				}
			}

			else {
				// You have a miss in reading data
				if (ReplacementFactor == 1) {
					free(tempArray);
				}
				*cycles = MISS_TIME;
				return;	
			}

		}
		// Each running on the bus takes 1 cycle
		*cycles++;
	}
	// Copy data from tempArray into DataValueToLoad, by this DataValueToLoad holds a pointer to the data that have been read
	*DataValueToLoad = tempArray;

	return;
}

int write(int Address, Memory* memory, int busWidth, unsigned char **dataToWrite, int Dirty) {

	int BusTransferringAmount = 0, i = 0, j = 0, k = 0, cycles = 0, tag = 0, index = 0, offset = 0, wayToWrite = 0, sizeToWrite = 0;

	tag = Address / (memory->size / memory->ways);
	index = ((Address % (memory->size / memory->ways)) / memory->blockSize);
	offset = ((Address % (memory->size / memory->ways)) % memory->blockSize);

	if (busWidth <= memory->blockSize) {		// TODO: Check wheter '=' sign is compatible here
		// Block size is bigger than Bus width, we should write in a couple of transactions			
		BusTransferringAmount = memory->blockSize / busWidth;
		sizeToWrite = busWidth;
	}
	else {
		// Block size is smaller than Bus width, we can write in only one transaction	
		BusTransferringAmount = 1;
		sizeToWrite = memory->blockSize;
	}
	// Check both sets
	for (i = 0; i<memory->ways; i++) {
		if (memory->blocks[i][index].LRU == 0) {
			// i holds the way that was Least Recently Used, therefore overwrite data value in that way
			wayToWrite = i;
			break;
		}
		wayToWrite = i;
	}
	// Write data inside block according to BusTransferringAmount
	for (j = 0; j<BusTransferringAmount; j++) {
		for (k = 0; k<sizeToWrite; k++) {
			// Write data into relevant block byte after byte
			memory->blocks[wayToWrite][index].bytes[k + j * busWidth] = *dataToWrite[k + j * busWidth];
		}
		// Each transfer on the bus takes 1 cycle (for all memories)
		cycles++;
	}

	// Update control bits of the relevant block
	// Valid
	memory->blocks[wayToWrite][index].valid = 1;
	// Dirty indicates that we made a write assignment from a higher level memory into lower level memory, 
	// Though we are storing new data into the memory, and not Loading it from lower level memories for a matters of coherence.
	if (Dirty == 1) {
		memory->blocks[wayToWrite][index].dirty = 1;
	}
	// Tag
	memory->blocks[wayToWrite][index].tag = tag;
	// LRU
	if (memory->ways == 2) {
		wayToWrite = i;
		memory->blocks[wayToWrite][index].LRU = 1;
		memory->blocks[(wayToWrite + 1) % 2][index].LRU = 0;
	}
	return cycles;
}