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
#define RLE_BUFFER_SIZE 256

void checkArguments(int argc, char **argv);
void printTitle();
void enterToContinue();
void printAsText(FILE *fp);
unsigned long printAsBytes(FILE *fp);
bool bitIsOnMask(char c, int bitIndex);
bool bitIsOnMod(char c, int bitIndex);
void printBits(char c);
void reverseBytes(FILE *in, FILE *out);

void rleEncodeFile(FILE *in, FILE *out);
int rleEncodeBytes(char* in, char* out, int length);
int findRepeatsInBuffer(char c, char* tBuffer, int startIndex, int endIndex);

void rleDecodeFile(FILE *in, FILE *out);
int rleDecodeBytes(char* in, char *out, int length);

int main(int argc, char **argv) {
    // C used in Haiku is C90, not C99 - identifiers need to be at start of block
    FILE *inFP;
    FILE *outFP;
    unsigned long numberOfBytes;

    printTitle();
    checkArguments(argc, argv);

	printf("File selected: %s\n\n", argv[1]);  
  	enterToContinue();
  
    inFP = fopen(argv[1],"r");
    outFP = fopen(argv[2], "w");

    //reverseBytes(inFP, outFP);
    rleDecodeFile(inFP, outFP);


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

void rleEncodeFile(FILE *in, FILE *out) {
	char buffer[RLE_BUFFER_SIZE]; 

	int  bytesRead;
	int compressedSize;

	do {

		bytesRead = fread(buffer, sizeof(char), RLE_BUFFER_SIZE, in);
		{
			char encodedBytesBuffer[bytesRead];
			compressedSize = rleEncodeBytes(buffer, encodedBytesBuffer, bytesRead);
			fwrite(encodedBytesBuffer, sizeof(char), compressedSize, out);
		}
	} while ( ! feof(in) );
}

void rleDecodeFile(FILE *in, FILE *out) {
	char buffer[RLE_BUFFER_SIZE]; 

	int  bytesRead;
	int decompressedSize;

	do {

		bytesRead = fread(buffer, sizeof(char), RLE_BUFFER_SIZE, in);
		{
			char decodedBytesBuffer[RLE_BUFFER_SIZE*10];
			decompressedSize = rleDecodeBytes(buffer, decodedBytesBuffer, bytesRead);
			fwrite(decodedBytesBuffer, sizeof(char), decompressedSize, out);
		}
	} while ( ! feof(in) );
}

int rleDecodeBytes(char* in, char *out, int length) {
	char lastByte;
	char currentByte;
	int counterByte;
	int x;
	int y;
	int status;

	status = 0;
	x = 0;
	y = 0;

	lastByte = in[x];

	out[y] = lastByte;
	x += 1;
		
	while (x < length) {
		
		currentByte = in[x];

		if (currentByte == lastByte) {
			y += 1;
			out[y] = currentByte;


			x += 1;
			counterByte = in[x];
			printf("y: %i counter: %i\n", y, counterByte);
			status += 2 + counterByte; /* first two repeats plus others */
			
			int t;
			t = 0;

			y += 1;
			for (t; t < counterByte; t++){
				out[y + t] = currentByte;
			}

			y += counterByte;
		} else {
			y += 1;
			printf("y single: %i\n", y);
			out[y] = currentByte;
			status += 1; /* just one, not repeating */
		}

		lastByte = currentByte;

		x += 1;
	}

	return status;
}

int rleEncodeBytes(char* in, char* out, int length) {
	char firstByte;
	char secondByte;
	int x;
	int y;
	int status;

	status = 0;

	x = 0;
	y = 0;
	while (x < length){
		firstByte = in[x];
		secondByte = in[x+1];

		if (firstByte == secondByte){
			/*output the two same bytes*/
			/*get repeats immediately after these two bytes*/
			int repeats = findRepeatsInBuffer(firstByte, in, x+2, length);
			
			out[y] = firstByte;
			out[y+1] = secondByte;
			out[y+2] = repeats;
			printf("'%c%c',%i\n", firstByte, secondByte, repeats);
			y = y + 3;
			x = x + 2 + repeats; /*skip to next two bytes, accounting for repeated bytes*/
			status += 3;
		} else {
			out[y] = firstByte;
			out[y+1] = secondByte;
			y = y + 2;

			printf("'%c%c'\n", firstByte, secondByte);
			x = x + 2; /*skip to next two bytes */
			status += 2;
		}
	}

	return status;
}

int findRepeatsInBuffer(char c, char* tBuffer, int startIndex, int endIndex) {
	int x;
	int repeatCount;

	x = startIndex;
	repeatCount = 0;

	for (x; x < endIndex; x++) {
		if (c == tBuffer[x]) {
			repeatCount++;
		} else {
			return repeatCount;
		}
	}
	return repeatCount;
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


