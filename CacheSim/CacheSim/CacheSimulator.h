/*This is the header file for main.c
*/

#ifndef __CACHESIMULATOR_H
#define __CACHESIMULATOR_H


/* Structures declarations */ 
typedef struct Block
{
	int tag;
	int valid;
	int dirty;
	int LRU;
	unsigned char *bytes;
} Block;

typedef struct Memory
{
	int ways;
	int blockSize;
	int size;
	Block **blocks;
} Memory;

/* Function Declarations: */
int CacheSimulator(int CacheLevel, int L1CacheBlockSize, int L2CacheBlockSize, char *TraceFileName, char *MemInFileName, char *MemOutFileName, char *L1CacheFileName, char *L2Way0CacheFileName, char *L2Way1CacheFileName, char *StatsFileName);

#endif#pragma once
