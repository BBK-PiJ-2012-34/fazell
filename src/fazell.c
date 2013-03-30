/************************************************
 * Fazell v0.1 - A rather curious obfuscator    *
 *                                              *
 * Syntax: C90                                  *
 *                                              *
 * Author:  Hisham Khalifa                      *
 * Email:   hisham@binarybakery.com             *
 *                                              *
 * This program is licensed under LGPL.         *
 * See README.md for more information.          *
 ************************************************
 * Version history: 0.1 - 29 March 2013         *
 ************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// C90 doesn't have bool definition
typedef int bool;
#define true 1
#define false 0

#define BUFFER_SIZE 256

void checkArguments(int argc, char **argv);
void printTitle();
void enterToContinue();
void printAsText(FILE *fp);
unsigned long printAsBytes(FILE *fp);
bool bitIsOnMask(char c, int bitIndex);
bool bitIsOnMod(char c, int bitIndex);
void printBits(char c);
void reverseBytes(FILE *in, FILE *out);

int main(int argc, char **argv) {
    // C used in Haiku is C90, not C99 - identifiers need to be at start of block
    FILE *inFP;
    FILE *outFP;
    unsigned long numberOfBytes;

    printTitle();
    checkArguments(argc, argv);

	printf("File selected: %s\n\n", argv[1]);  
  	enterToContinue();
  
    inFP = fopen(argv[1],"r+");
    outFP = fopen(argv[2], "w");

    reverseBytes(inFP, outFP);
    fclose(outFP);

    return 0;
}

void checkArguments(int argc, char **argv) {
	if (argc==1){
    	printf("Usage: %s in-file out-file\n", argv[0]);
    	exit(-1);
    }
}

void printTitle() {
    printf ("Fazell v0.1 - A rather curious obfuscator by Hisham Khalifa - 29 March 2013\n");
}

void printAsText(FILE *fp) {
	char byte;
	byte = (char)fgetc(fp);

    while (byte != EOF) {
        printf("%c",byte);
        byte = (char)fgetc(fp);
        
    }   
}

unsigned long printAsBytes(FILE *fp) {
	char byte;
	unsigned long byteCount;
	byte = (char)fgetc(fp);
	byteCount = 0;
    while (byte != EOF) {
    	byteCount++;
        byte = (char)fgetc(fp);
        printBits((char)byte);
    }
    
    return byteCount;
}

void reverseBytes(FILE *in, FILE *out) {
	char buffer[BUFFER_SIZE];
	int  bytesRead;
	char mask;

	bytesRead = 0;
	mask = 0xff; // 255, ie, 11111111 to reverse all bits using XOR bitwise op.

	do {
		bytesRead = fread(buffer, sizeof(char), BUFFER_SIZE, in);
		{
			int x;

			for (x = 0; x < bytesRead; x++) {
				buffer[x] = buffer[x]^mask;
			}
			fwrite(buffer, sizeof(char), bytesRead, out);
		}
	} while ( ! feof(in) );
}

void enterToContinue() {
	printf("Press enter to continue...\n");
	getchar();   
}

bool bitIsOnMask(char c, int bitIndex) {
	if (bitIndex >=0 && bitIndex <=7) {
		char mask;
		char result;
		
		mask = pow(2,bitIndex);
		
		result = c & mask;
		
		if (result == c) {
			return true;
		}
		
		return false;			
	}
}

bool bitIsOnMod(char c, int bitIndex) {
	int x;
	bool on;
	on = false;
	for (x = 0; x <= bitIndex; x++) {
		float remainder;
		remainder = (int)c % 2;
		c = c / 2; // Make sure to halve byte's value in readiness for the next iteration		
		on = (remainder > 0) ? true : false;
	}

	return on;	
}

void printBits(char c) {
	int x;
	// Note that little-endian means the bytes will from lowest (leftest = 1) to highest (rightest = 128).
	// So for human consumption we'll place bits in an array and in big-endian order.
	char byteString[9];
	
	for (x = 0; x < 8; x++) {
		byteString[7-x] = bitIsOnMod(c,x) ? '1' : '0';
	}
	
	byteString[8] = '\0';
	
	printf("%s",byteString);
}
