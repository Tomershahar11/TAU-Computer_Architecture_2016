/*
Cache sim - main.c:
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
@param stats.txt: A text file which holds data statistics regarding running of the program, such as no. of write\read hits, no. of write\read misses, local\global miss rate, AMAT.

Output files:
@Output memout.txt- The text file holding the main memory data after running the program.
@Output l1.txt- The text file holding L1 cache data after running the program.
@Output l2way0.txt- The text file holding way 0 data of L2 cache after running the program.
@Output l2way1.txt- The text file holding way 1 data of L2 cache after running the program.
@Output stats.txt- The text file holding data statistics of the program after running.

*/

/* Libraries: */
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <math.h>


/* Structures declarations */ // TODO: Move to Header.h
typedef struct Block
{
	int tag;
	int valid;
	int dirty;
	int LRU;
	unsigned char *bytes;
}Block;

typedef struct Memory
{
	int ways;
	int blockSize;
	int size;
	Block **blocks;
}Memory;


/* Definitions: */
#define DATA_BUS_WIDTH_CPU_TO_L1_IN_BITS 32
#define ADDRESS_BUS_WIDTH_CPU_TO_L1_IN_BITS 24
#define BUS_WIDTH_L1_TO_L2_IN_BITS 256
#define MAIN_MEMORY_SIZE_IN_BYTES 16777216
#define BUS_WIDTH_CACHE_TO_MAIN_MEM_IN_BITS 64
#define MAIN_MEM_ACCESS_TIME 100
#define MISS_TIME -1							//TODO: what is it??
#define CACHE_L1_SIZE_IN_BYTES 4096				// L1 Directed-Mapped cache
#define CACHE_L2_SIZE_IN_BYTES 32768			// L2 Two-Way-Set-Associative with LRU replacement policy
#define WORD_IN_BITS 32
#define L1_HIT_TIME 1
#define L2_HIT_TIME 4


/* Start of Program: */

int PrintError(int errNum);
/**
*countlines count the number of lines in text fille.
* Accepts:
* --------
*filename-string that continas the name of file

* Returns:
* --------
* the number of lines in text fille **/

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

	fclose(fileInput);
	return TotalNumberOfLines;
}

//int countlines(char *file_name)
//{
//  FILE *fp=fopen(file_name, "r");                 
//  int ch=0;
//  int lines=0;
//
//  lines++;
//  while ((ch = fgetc(fp)) != EOF)
//    {
//      if (ch == '\n')
//    lines++;
//    }
//  fclose(fp);
//  return lines;
//}


/**
*Cast_char_hex_to_dec cast hex digit to decimal number.
* Accepts:
* --------
*c-a char that repesents a hexadecimal digit

* Returns:
* --------
* decimal value. **/


int convertHexToDec(char ch)
{
	if (ch >= '0' && ch <= '9') {
		return (ch - '0');
	}
	else if (ch >= 'A' && ch <= 'F') {
		return (ch - 'A') + 10;
	}
}

/*
This function converts either hex data or hex address as a function of its integer number.
@param *str: input pointer to line in trace.txt.
@param num: input number indicates which to convert, '6' means the conversion is from hex address, o.w means the conversion is from hex data.
@Output: Returns the decimal value of conversion.

The num is worth to 6 if the program needs to convert hex address and it is worth to 6 if the program needs to convert hex data.
It returns the decimal value of the data/address*/
int convert_data_or_address(char* str, int num)
{
	int j, value, z;
	int power = num - 1;//for the calaculate of the casting. the power change according to the location of the digit.
	int decimalNumber = 0;
	long double x = 16;
	char c;
	for (j = 0; j<num; j++)
	{
		if (num == 2)//for the init memory action
			c = str[0 + j];
		if (num == 6)//if the program asks to convert the address
			c = str[4 + j];//the address beging in the fourth place in each line in trace.txt
		if (num == 8)//if the program asks to convert the data (only in sw case)
			c = str[11 + j];//the data beging in the 11th place line in trace.txt which asks to do sw instruction.
		value = Cast_char_hex_to_dec(c);
		z = pow(x, power);
		decimalNumber = decimalNumber + (value*z);
		power--;
	}
	return decimalNumber;
}

void init_stats(int stats[12])
{
	int i;
	for (i = 0; i<12; i++)
		stats[i] = 0;
}


void init_memory(FILE* fin_mem, Memory* main_mem, char *filename)
{
	int j = 0, i = 0;//bytes
	int k;
	char str[4];
	int num_lines = countlines(filename);
	long double x = 2;
	int mem_Size = (int)(pow(x, ADDRESS) / 1);
	for (k = 0; k<mem_Size - 1; k++)
	{
		main_mem->blocks[0][k / 4].bytes[k % 4] = 0;
		main_mem->blocks[0][k / 4].valid = 1;
	}

	while ((fgets(str, 4, fin_mem) != NULL) && (j<num_lines))
	{
		main_mem->blocks[0][j / 4].bytes[j % 4] = (unsigned char)Convert_data_or_address(str, 2);
		main_mem->blocks[0][j / 4].valid = 1;
		++j;
	}
	//if the number of lines in memin.txt is smaller the the size of the main mem (in bytes)


	main_mem->ways = 1;
}


int createMemory(int ways, int blockSize, int memSize, Memory* mem)
{
	int i, j, k;
	Block *blocks = (Block*)malloc(sizeof(Block)*memSize*ways);
	if (blocks == NULL)
	{
		PrintError(2);
	}
	mem->blocks = (Block**)malloc(sizeof(Block*)*ways);
	if (blocks == NULL)
	{
		PrintError(2);
	}
	for (i = 0; i<ways; i++)
		mem->blocks[i] = blocks + i*(memSize / blockSize);
	for (i = 0; i<ways; i++)
	{
		for (j = 0; j<memSize / blockSize; j++)//creates arrays of bytes and puts into blocks
		{
			mem->blocks[i][j].bytes = (unsigned char*)calloc(blockSize, sizeof(unsigned char));
			for (k = 0; k<blockSize; k++)
				mem->blocks[i][j].bytes[k] = 0;
			mem->blocks[i][j].dirty = 0;
			mem->blocks[i][j].valid = 0;
			mem->blocks[i][j].tag = 0;
			mem->blocks[i][j].LRU = 0;
		}
	}
	mem->blockSize = blockSize;
	mem->size = memSize;
	mem->ways = ways;
	return 0;
}

/*
bus size entered in bytes not bits!
*/
void read(int adress, Memory* mem, int block_size_to_return, int bus_size, unsigned char **data_read, int *cycles)
{
	int i, j, k, d, tag, offset_for_bus, buses_in_block_to_return, blocks_in_bus, byte_offset_in_block, byte_offset_in_data_read, counter_of_block = 0;
	int offset = (((adress % (mem->size / mem->ways)) % mem->blockSize));
	int index = (((adress % (mem->size / mem->ways)) / mem->blockSize));
	int current_block = index;
	int mode = 0;
	unsigned char* temp_arr;
	if (bus_size>block_size_to_return)
		buses_in_block_to_return = 1;
	else
		buses_in_block_to_return = block_size_to_return / bus_size;
	if (block_size_to_return <= mem->blockSize)
	{
		if (bus_size <= mem->blockSize)
		{
			mode = 0;
			blocks_in_bus = 1;
		}

		if (bus_size>mem->blockSize)
		{
			mode = 1;
			blocks_in_bus = bus_size / mem->blockSize;
		}
	}
	else
	{
		if (bus_size <= mem->blockSize)
		{
			mode = 2;
			blocks_in_bus = 1;
		}

		if (bus_size>mem->blockSize)
		{
			mode = 3;
			blocks_in_bus = bus_size / mem->blockSize;
		}
	}
	offset = ((adress % (mem->size / mem->ways)) % mem->blockSize);
	temp_arr = (unsigned char *)malloc(sizeof(unsigned char)*block_size_to_return);
	tag = adress / (mem->size / mem->ways);
	for (i = 0; i<mem->ways; i++)//check both sets
	{
		for (j = 0; j<buses_in_block_to_return; j++)//read can only be done 1 bus at a time
		{
			for (d = 0; d<blocks_in_bus; d++)//bus can some times be larger then block in memory
			{
				switch (mode)
				{
				case 0://for example when reading from level 2 to level 1 with level_1_block_size which is larger then 32 bytes
					current_block = index;//only one block is read
					byte_offset_in_block = mem->blockSize*(offset / mem->blockSize) + j*bus_size;
					//offset chooses a byte in block_size_to_return if we devide it by mem->blockSize 
					//we get the sub block in block_to_return we need to read, then we multiply by mem->blockSize to get the exact byte we need to start reading from
					//j*bus_size gives us the offset we get from previous bytes we read
					byte_offset_in_data_read = j*bus_size;// read is done bus by bus
					break;
				case 1://for example when reading from level 2 to level 1 with level_1_block_size which is smaller then 32 bytes
					current_block = index;//only one block is read
					byte_offset_in_block = mem->blockSize*(offset / mem->blockSize);//this returns the exact byte we start reading from(same as case 0)
					byte_offset_in_data_read = 0;//only 1 bus read
					break;
				case 2:// will only happen if block size of level 1 is larger then that of level 2 which is imposible
					current_block = (index / block_size_to_return)*block_size_to_return;//
					byte_offset_in_block = bus_size*j / mem->blockSize;
					byte_offset_in_data_read = j*bus_size;// read is done bus by bus
					break;

				case 3://for example when reading from main mem to level 2
					current_block = (index / block_size_to_return)*block_size_to_return + j*bus_size + d;
					//index chooses a block in mem->blocks. we need to read a bigger block then the block in mem->blocks
					//first we devide by block_size_to_return to get the block group we need to read 
					//then we again multiply by block_size_to_return to get the exact index we need to start reading from
					//j*bus_size adds the indexes we read in previous buss runs so we keep reading the next blocks
					//d adds the blocks inside each bus
					byte_offset_in_block = 0;//only 1 bus in each block in mem->block
					byte_offset_in_data_read = j*bus_size + d*mem->blockSize;//read is done bus by bus  block in bus after block
					break;
				}
				if (mem->blocks[i][current_block].tag == tag && mem->blocks[i][current_block].valid == 1)// if the tag is the same and block is valid we can read from it
				{
					for (k = 0; k<mem->blockSize; k++)
					{
						temp_arr[k + byte_offset_in_data_read] = mem->blocks[i][current_block].bytes[k + byte_offset_in_block];
					}
				}
				else
				{
					free(temp_arr);
					(*cycles) = MISS;
					return;
				}
			}
			(*cycles)++;// each run on the bus takes 1 cycle (for all memories)
		}
	}
	(*data_read) = temp_arr;
	return;
}
int write(int adress, Memory* mem, int bus_size, unsigned char **data_to_write)
{
	int busses_amount, i, j, k, cycles = 0, tag, index, offset, offset_for_bus;
	int size_to_write;
	tag = adress / (mem->size);
	index = ((adress % (mem->size)) / mem->blockSize);
	offset = ((adress % (mem->size)) % mem->blockSize);
	if (bus_size <= mem->blockSize)
	{
		//busses_amount=1;
		busses_amount = mem->blockSize / bus_size;
		size_to_write = bus_size;
	}
	else
	{
		//busses_amount=mem->blockSize/bus_size;
		busses_amount = 1;
		size_to_write = mem->blockSize;
	}
	for (i = 0; i<mem->ways; i++)//check both sets
	{
		if (mem->blocks[i][index].LRU == 0) //
		{
			for (j = 0; j<busses_amount; j++)//write can only be done 1 bus at a time
			{
				for (k = 0; k<size_to_write; k++)
				{
					mem->blocks[i][index].bytes[k + j*bus_size] = (*data_to_write)[k + j*bus_size];//sadly we cant simulate bus paralerly so we read byte after byte
				}
				cycles++;// each run on the bus takes 1 cycle (for all memories)
			}
			mem->blocks[i][index].valid = 1;
			mem->blocks[i][index].dirty = 1;
			if (mem->ways == 2)
			{
				mem->blocks[i][index].LRU = 1;
				mem->blocks[(i + 1) % 2][index].LRU = 0;
			}
		}
	}
	return cycles;
}

void Load(Memory *cache1, Memory *cache2, Memory *main_memory, int address, int cache1_block_size, int cache2_block_size, int stats[12], unsigned char* arr)
{
	int cycles = 0, i, index;
	unsigned char *help_arr;
	read(address, cache1, WORD / 8, (BUS_LEVEL_1 / 8), &arr, &cycles);
	//cycles=-1;

	if (cycles != MISS)
	{
		stats[1]++;//תwe hit in L1
		stats[0] += cycles;//hit time for L1
		return;
	}
	//arr=NULL;
	cycles = 0;
	if (cache2 != NULL)
	{
		read(address, cache2, cache1_block_size, (BUS_LEVEL_2_TO_LEVEL_1 / 8), &arr, &cycles);
		stats[0] += LEVEL_2_MISS_PENALTY;
		stats[3]++;//we missed in L1
				   //	cycles=-1;
		if (cycles != MISS)
		{
			stats[5]++;//we hit in L2
			cycles += write(address, cache1, BUS_LEVEL_2_TO_LEVEL_1 / 8, &arr);//after reading from L2 we want to load the block to L1
			stats[0] += cycles;//cycles used to read from L2 and cycles used to write to L1
			return;
		}
	}
	//arr=NULL;
	cycles = 0;
	stats[7]++;//we missed in L2

	if (cache2 != NULL)
	{
		read(address, main_memory, cache2_block_size, (MAIN_BUS / 8), &arr, &cycles);
		cycles += write(address, cache2, MAIN_BUS / 8, &arr);
		cycles += LEVEL_2_MISS_PENALTY;
		index = address%cache1->size / cache1->blockSize;
		help_arr = (unsigned char *)malloc(sizeof(unsigned char)*cache1->blockSize);
		for (i = 0; i<cache1->blockSize; i++)
			help_arr[i] = arr[((index % (cache2->blockSize / cache1->blockSize))*cache1->blockSize) + i];
		cycles += write(address, cache1, BUS_LEVEL_2_TO_LEVEL_1 / 8, &help_arr);
	}
	else
	{
		read(address, main_memory, cache1_block_size, (MAIN_BUS / 8), &arr, &cycles);
		cycles += write(address, cache1, MAIN_BUS / 8, &arr);
	}

	stats[0] += cycles + MAIN_MEM_MISS_PENALTY;// read from main_mem +2 writes + level 2 miss
}

void print_mem(Memory *mem, FILE* fp, int level)
{
	int i, counter = 0;
	//for(i=0;i<mem_Size;i++)
	for (i = 0; i<mem->size; ++i)
	{
		if (counter % 30 == 0)
			if (counter != 0)
			{
				fprintf(fp, "\n");

			}
		if (i%mem->blockSize == 0)
			if (i != 0)
			{
				counter = 0;
				fprintf(fp, "\n");
				fprintf(fp, "\n");
			}
		fprintf(fp, "%4u", mem->blocks[level][i / mem->blockSize].bytes[i%mem->blockSize]);
		counter++;
	}
	fprintf(fp, "\n");
}


int main(int argc, char* argv[])
{
	FILE *fin_trace = NULL;
	FILE *fin_mem = NULL;
	FILE *fp = NULL;
	Memory* main_mem = NULL;
	Memory* cache1 = NULL;
	Memory* cache2 = NULL;
	int add_val;
	int stats[12];
	int ways[3] = { 1, 1, 2 };
	long double x = 2;
	//int memSize=(int)(pow(x,24)/1);
	int block_size_1 = atoi(argv[2]);
	int block_size_2 = atoi(argv[3]);
	int data_val = 0;
	int num_lines = 0, num_lines_memin = 0;
	int reg_load;
	int i = 0, j = 0, m;
	char hexadecimalNumber1[9];//for the word that we read from the main memory in load action.ùøùåø ùì 4 áúéí
	char hexadecimalNumber2[3];
	char hexadecimalNumber3[3];
	char hexadecimalNumber4[3];
	char hexadecimalNumber5[9];//for the store action
	char str[20];//the  max number of chars in each line 8+6+1+1+4spaces
	unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char));
	main_mem = (Memory*)malloc(sizeof(Memory));
	if (main_mem == NULL)
	{
		PrintError(2);
	}
	cache1 = (Memory*)malloc(sizeof(Memory));
	if (cache1 == NULL)
	{
		PrintError(2);
	}
	if (atoi(argv[1]) == 2) {
		cache2 = (Memory*)malloc(sizeof(Memory));
		if (cache2 == NULL)
		{
			PrintError(2);
		}
	}

	fin_trace = fopen(argv[4], "rt");
	if (!fin_trace)
	{
		PrintError(1);
	}
	num_lines = countlines(argv[4]);
	fin_mem = fopen(argv[5], "rt");
	if (!fin_mem)
	{
		PrintError(1);
	}
	init_stats(stats);
	createMemory(ways[0], WORD / 8, MAIN_MEMORY_SIZE, main_mem);
	createMemory(ways[1], block_size_1, CACHE_LEVEL_1_SIZE, cache1);
	if (atoi(argv[1]) == 2)
		createMemory(ways[2], block_size_2, CACHE_LEVEL_2_SIZE, cache2);

	//init the main memory by the infomation in "memin.txt"
	init_memory(fin_mem, main_mem, argv[5]);
	/* print_mem(main_mem,4,cache1,cache2);*/

	//load or store
	while ((fgets(str, 20, fin_trace) != NULL) && (j<num_lines))
	{
		if (str[2] == 'L')
		{
			add_val = Convert_data_or_address(str, 6);
			/* printf("%d\n",add_val);*/
			Load(cache1, cache2, main_mem, add_val, block_size_1, block_size_2, stats, data);
			++j;
		}

		// Cast_dec_to_hex(hexadecimalNumber1,main_mem->blocks[add_val+3].data,2);//according to little endian
		// Cast_dec_to_hex(hexadecimalNumber2,main_mem->blocks[add_val+2].data,2);//according to little endian
		//Cast_dec_to_hex(hexadecimalNumber3,main_mem->blocks[add_val+1].data,2);//according to little endian
		//Cast_dec_to_hex(hexadecimalNumber4,main_mem->blocks[add_val].data,2);//according to little endian
		//strcat(hexadecimalNumber1,hexadecimalNumber2);
		//strcat(hexadecimalNumber1,hexadecimalNumber3);
		//strcat(hexadecimalNumber1,hexadecimalNumber4);
		//puts(hexadecimalNumber1);

		// if (str[2]=='S')
		//{
		// //add_val=Convert_data_or_address(str,6);
		// //printf("%d\n",add_val);
		// //data_val=Convert_data_or_address(str,8);
		// // printf("%d\n",data_val);
		// // ++j;
		// // printf("%d\n",main_mem->blocks[4].data);
		// // /*Cast_dec_to_hex(hexadecimalNumber5,main_mem->blocks[add_val].data,8);
		// // puts(hexadecimalNumber5);*/
		// // m=3;
		// // for (k=0;k<8;k=k+2)
		// // {
		// // main_mem->blocks[add_val+m].data=Convert_data_or_address(str+11+k,2);//according to little endian
		// // --m;
		//  }
	}
	fp = fopen(argv[6], "w");
	print_mem(main_mem, fp, 0);
	fclose(fp);
	fp = NULL;
	if (atoi(argv[1]) == 2)
	{
		fp = fopen(argv[8], "w");
		print_mem(cache2, fp, 0);
		fclose(fp);
		fp = NULL;
		fp = fopen(argv[9], "w");
		print_mem(cache2, fp, 1);
		fclose(fp);
		fp = NULL;
	}
	fp = fopen(argv[7], "w");
	print_mem(cache1, fp, 0);
	fclose(fp);
	fp = NULL;

	fp = fopen(argv[10], "w");
	for (i = 0; i<12; i++)
		fprintf(fp, "stat %d is %d\n", i, stats[i]);
	fclose(fp);
	fp = NULL;
	return 0;

}

int PrintError(int errNum)
{
	switch (errNum)
	{
	case 1:
		printf("error opening file\n");
		exit(1);
	case 2:
		printf("eror alocating memory\n");
		exit(2);

	}
	printf("unknown error\n");
	exit(99);
}
