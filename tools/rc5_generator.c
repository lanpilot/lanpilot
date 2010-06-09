#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(char* prgname)
{
	fprintf(stderr, 
		"Usage:\n"
		"        %s [toggle_bit] [address_in_hex] [command_in_hex]\n"
		"\n"
		"where toggle_bit is equal either to 1 or to 0\n",
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

void emit_message(int frequency, int half_bit_length, int* message, int length)
{
	/*
	int j;
	for(j=0; j<length-1; ++j)
		fprintf(stderr, "%d,",message[j]);
	fprintf(stderr, "%d\n", message[length-1]);
	*/

	printf("%d", frequency);

	if(message[0]){
		// if first bit is one, then we just don't emit the initial low
	}else{
		printf("_%d", half_bit_length); //high
	}
	int i;
	for(i=1; i<length; ++i){
		if((message[i-1] == 0) != (message[i] == 0)){
			// bit changed
			// we emit longer signal
			printf("_%d", 2*half_bit_length);
		}else{
			// bit didn't change
			// two shorter signals
			printf("_%d", half_bit_length);
			printf("_%d", half_bit_length);
		}
	}
	//closing the last bit
	printf("_%d", half_bit_length);
	puts("");
	
}


int main(int argc, char** argv)
{
	if(argc != 4)
		print_usage(argv[0]);
	if(strlen(argv[1]) != 1)
		print_usage(argv[0]);
	if((argv[1][0] != '0') && (argv[1][0] != '1'))
		print_usage(argv[0]);

	int message[14];
	message[0] = 1;
	message[1] = 1;
	message[2] = argv[1][0] - '0'; //toggle bit

	int address = read_hex_num(argv[2]);
	int command = read_hex_num(argv[3]);
	if((command < 0) || (address < 0))
		print_usage(argv[0]);

	if(address >= (1 << 5)){
		fprintf(stderr, "Address must be smaller than 0x20 (32 in decimal))\n");
		exit(1);
	}
	if(command >= (1 << 6)){
		fprintf(stderr, "Command must be smaller than 0x40 (64 in decimal))\n");
		exit(1);
	}
	
	message[3] = (address & (1 << 4)) != 0;
	message[4] = (address & (1 << 3)) != 0;
	message[5] = (address & (1 << 2)) != 0;
	message[6] = (address & (1 << 1)) != 0;
	message[7] = (address & (1 << 0)) != 0;

	message[8] = (command & (1 << 5)) != 0;
	message[9] = (command & (1 << 4)) != 0;
	message[10] = (command & (1 << 3)) != 0;
	message[11] = (command & (1 << 2)) != 0;
	message[12] = (command & (1 << 1)) != 0;
	message[13] = (command & (1 << 0)) != 0;


	emit_message(36, 32, message, 14);



	
	return 0;
}
