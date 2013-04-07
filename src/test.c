#include <stdlib.h>
#include <stdio.h>

// C90 doesn't have bool definition
typedef int bool;
#define true 1
#define false 0

#define BUFFER_SIZE 35

bool rleEncode(char* in, char* out);
int findRepeatsInBuffer(char c, char* tBuffer, int startIndex, int endIndex);
void printBuffer(char* b, int x);

int main() {

	int compressedSize = 0;
	int decompressedSize = 0;

	char buffer[BUFFER_SIZE] = {'h','f','u','h','i','u','g','h','e','r','i','u','g','i','i','i','e','e','e','r','r','r','r','r','r','r','r','f','f','f','f','f','f','w','e'};
	char encoded[BUFFER_SIZE];
	char decoded[BUFFER_SIZE];

	printBuffer(buffer, BUFFER_SIZE);
	compressedSize = rleEncode(buffer, encoded);

	printf("Encoded: ");
	printBuffer(encoded, BUFFER_SIZE);
	decompressedSize = rleDecode(encoded, decoded);

	printf("Compressed size: %i\n", compressedSize);

	printf("Decoded: size - %i\n", decompressedSize);
	printBuffer(decoded, BUFFER_SIZE);

	return 0;
}

void printBuffer(char* b, int x){
	int y;

	for (y = 0; y < x; y++) {
		printf("%c",b[y]);
	}
	printf("\n");
}

int rleDecode(char* in, char *out) {
	char lastByte;
	char currentByte;
	int counterByte;
	int x;
	int y;
	int status;

	status = 0;
	counterByte = 0;
	x = 0;
	y = 0;

	lastByte = in[x];

	out[y] = lastByte;
	x += 1;
		
	while (x < 3) {
		
		currentByte = in[x];

		if (currentByte == lastByte) {
			y += 1;
			out[y] = currentByte;

			x += 1;
			counterByte = in[x];

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
			out[y] = currentByte;
			status += 1; /* just one, not repeating */
		}

		lastByte = currentByte;

		x += 1;
	}

	return status;
}

int rleEncode(char* in, char* out) {
	char firstByte;
	char secondByte;
	int x;
	int y;
	int status;

	status = 0;

	x = 0;
	y = 0;
	while (x < BUFFER_SIZE){
		firstByte = in[x];
		secondByte = in[x+1];

		if (firstByte == secondByte){
			/*output the two same bytes*/
			/*get repeats immediately after these two bytes*/
			int repeats = findRepeatsInBuffer(firstByte, in, x+1, BUFFER_SIZE);
			
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
