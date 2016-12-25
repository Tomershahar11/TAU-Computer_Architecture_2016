#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<time.h>
#include<math.h>

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo Functions OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/


int Cast_char_hex_to_dec(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'A' && c <= 'F')
		return (c - 'A') + 10;

}

//It returns the decimal value of the address
int Convert_address_hex_to_dec(char* str)
{
	int j, value, z;
	int power = 5;
	int decimalNumber = 0;
	long double x = 16;
	char c;
	for (j = 0; j<6; j++)
	{
		c = str[j];
		value = Cast_char_hex_to_dec(c);
		z = pow(x, power);
		decimalNumber = decimalNumber + (value*z);
		power--;
	}
	return decimalNumber;
}

void Convert_address_dec_to_hex(int num, char hexadecimalNumber[7])
{

	long int decimalNumber, remainder, quotient;
	int i = 1, j, temp;
	char help_hexadecimalNumber[7];
	char c;
	int k;

	decimalNumber = (long int)num;
	quotient = decimalNumber;

	while (quotient != 0) {
		temp = quotient % 16;

		//To convert integer into character
		if (temp < 10)
			temp = temp + 48;
		else
			temp = temp + 55;

		help_hexadecimalNumber[i++] = temp;
		quotient = quotient / 16;
	}

	k = 0;
	for (j = i - 1; j> 0; j--)
	{
		printf("%c", help_hexadecimalNumber[j]);
		hexadecimalNumber[k] = help_hexadecimalNumber[j];
		++k;
	}
	hexadecimalNumber[6] = '\0';
}

/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo Main OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/

int main(int argc, char* argv[])
{
	FILE* FpTrace;
	char address[8];
	int len = 6;
	int i;
	int k;
	int dec_num;
	time_t t;
	char hexadecimalNumber[8];
	static const char alphanum[] = "ABCDEF0123456789";
	static const char divin4[] = "C048";
	FpTrace = fopen("trace.txt", "w");
	srand((unsigned)time(NULL));
	/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo Random address OoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/
	for (k = 0; k < 5; ++k)
	{

		for (i = 0; i < len - 1; ++i)
		{
			address[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

		}
		//the last digit is limit in the rand because we get aligned addresses
		address[len - 1] = divin4[rand() % (sizeof(divin4) - 1)];
		address[len] = '\0';


		/*oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOo Filing trace.txt oOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoOoO*/


		for (i = 0; i<20; i++) //20 גודל שרירותי למספר פקודות טעינה
		{
			fprintf(FpTrace, "7 L %s\n", address);
			dec_num = Convert_address_hex_to_dec(address);
			dec_num = dec_num + 4;
			Convert_address_dec_to_hex(dec_num, hexadecimalNumber);
			puts(hexadecimalNumber);
			strcpy(address, hexadecimalNumber);
			address[6] = '\0';
			puts(address);
		}
		fprintf(FpTrace, "\n");
	}
	fclose(FpTrace);
	return 0;

}
