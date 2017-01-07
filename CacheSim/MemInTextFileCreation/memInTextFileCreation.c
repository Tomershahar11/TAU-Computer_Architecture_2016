#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<math.h>
/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo Defines OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

//#define _CRT_SECURE_NO_DEPRECATE // avoid getting errors for '_s functions'
#define ADDRESS2 24

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo Main OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
int main(int argc, char* argv[])
{
	char *memInFileName = argv[5];
	FILE *FpMenin = NULL;
	long double x = 2;
	int i;
	int cycle;
	int memSize = (int)(pow(x, ADDRESS2) / 1);
	FpMenin = fopen(memInFileName, "w");
	for (i = 0; i< memSize / 16 - 1; ++i)
		//for(i=0;i<2000;++i)
	{
		cycle = i % 16;
		switch (cycle)
		{
		case 0:
			fprintf(FpMenin, "00\n");
			break;

		case 1:
			fprintf(FpMenin, "11\n");
			break;

		case 2:
			fprintf(FpMenin, "22\n");
			break;

		case 3:
			fprintf(FpMenin, "33\n");
			break;

		case 4:
			fprintf(FpMenin, "44\n");
			break;

		case 5:
			fprintf(FpMenin, "55\n");
			break;

		case 6:
			fprintf(FpMenin, "66\n");
			break;

		case 7:
			fprintf(FpMenin, "77\n");
			break;

		case 8:
			fprintf(FpMenin, "88\n");
			break;

		case 9:
			fprintf(FpMenin, "99\n");
			break;

		case 10:
			fprintf(FpMenin, "AA\n");
			break;

		case 11:
			fprintf(FpMenin, "BB\n");
			break;

		case 12:
			fprintf(FpMenin, "CC\n");
			break;

		case 13:
			fprintf(FpMenin, "DD\n");
			break;

		case 14:
			fprintf(FpMenin, "EE\n");
			break;

		case 15:
			fprintf(FpMenin, "FF\n");
			break;
		}

	}
	return 0;
}