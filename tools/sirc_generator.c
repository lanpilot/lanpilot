#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(char* prgname)
{
	fprintf(stderr, 
		"Usage:\n"
		"        %s [address_in_hex] [command_in_hex]\n"
		"\n",
		prgname
	);
	exit(1);
}

int hex_single_digit(char c)
{
	if((c >= '0') && (c <= '9'))
		return c - '0';
	if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	return -1;
}

int read_hex_num(char* in)
{
	if((strlen(in) == 4) || (strlen(in) == 3)){
		// there might be input in format 0x[digit][digit]
		if((in[0] == '0') && (in[1] == 'x'))
			return read_hex_num(in+2);
		else
			return -1;
	}
	if((strlen(in) > 2) || (strlen(in) < 1))
		return -1;

	int out = hex_single_digit(in[0]);
	if(out < 0)
		return -1;
	if(in[1] == '\0')
		return out;

	int out2 = hex_single_digit(in[1]);
	if(out2 < 0)
		return -1;
	

	return out * 16 + out2;
}

void emit_message(int frequency, int base_length, int* message, int length)
{
	/*
	int j;
	for(j=0; j<length-1; ++j)
		fprintf(stderr, "%d,",message[j]);
	fprintf(stderr, "%d\n", message[length-1]);
	*/

	printf("%d_%d", frequency, base_length*4);

	int i;
	for(i=0; i<length; ++i){
		printf("_%d", base_length); //separation from prievous bit
		if(message[i]){
			printf("_%d", 2*base_length);
		}else{
			printf("_%d", base_length);
		}
	}
	puts("");
}


int main(int argc, char** argv)
{
	if(argc != 3)
		print_usage(argv[0]);

	int message[12];

	int address = read_hex_num(argv[1]);
	int command = read_hex_num(argv[2]);
	if((command < 0) || (address < 0))
		print_usage(argv[0]);

	if(address >= (1 << 5)){
		fprintf(stderr, "Address must be smaller than 0x20 (32 in decimal))\n");
		exit(1);
	}
	if(command >= (1 << 7)){
		fprintf(stderr, "Command must be smaller than 0x80 (128 in decimal))\n");
		exit(1);
	}
	
	message[0] = (command & (1 << 0)) != 0;
	message[1] = (command & (1 << 1)) != 0;
	message[2] = (command & (1 << 2)) != 0;
	message[3] = (command & (1 << 3)) != 0;
	message[4] = (command & (1 << 4)) != 0;
	message[5] = (command & (1 << 5)) != 0;
	message[6] = (command & (1 << 6)) != 0;

	message[7] = (address & (1 << 0)) != 0;
	message[8] = (address & (1 << 1)) != 0;
	message[9] = (address & (1 << 2)) != 0;
	message[10] = (address & (1 << 3)) != 0;
	message[11] = (address & (1 << 4)) != 0;

	emit_message(40, 24, message, 12);

	return 0;
}
