#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "input_tool.h"

int inputReadInt(const char* prompt, int min, int max) {
	char intbuf[16];
	
	int result;
	do {
		printf("%s ", prompt);
		errno = 0;
		
		// - 16 chars is more than enough to read a max sized int (strtol() is undefined for so large anyway)
		// - After reading string, check if the last char is a line break
		// - If it is not, then we need to keep reading to clear the pending input until we find a line break
		if (fgets(intbuf, 16, stdin) == NULL) {
			fprintf(stderr, "Input error!\n");
			exit(1);
		}
		
		int len = strlen(intbuf);
		if (len > 0 && intbuf[len-1] != '\n') {
			int end;
			do {
				end = fgetc(stdin);
			} while (end != EOF && end != '\n');
		}
		
		result = strtol(intbuf, NULL, 10);
	} while (result < min || result > max || errno != 0);
	
	return result;
}

uint32_t inputReadU32Hex(const char* prompt) {
	char hexbuf[9];
	
	uint32_t result;
	do {
		printf("%s ", prompt);
		errno = 0;
		
		// - 9 chars is enough to read a max sized int + terminator
		// - After reading string, check if the last char is a line break
		// - If it is not, then we need to keep reading to clear the pending input until we find a line break
		if (fgets(hexbuf, 9, stdin) == NULL) {
			fprintf(stderr, "Input error!\n");
			exit(1);
		}
		
		int len = strlen(hexbuf);
		if (len > 0 && hexbuf[len-1] != '\n') {
			int end;
			do {
				end = fgetc(stdin);
			} while (end != EOF && end != '\n');
		}
		
		//result = strtoul(hexbuf, &outPtr, 16);
		
		// Use manual traversal instead to not silently ignore non-hex. stupid strtoul
		result = 0;
		for (int i = 0; i < len; i++) {
			int nybble;
			
			if (hexbuf[i] >= '0' && hexbuf[i] <= '9') {
				nybble = hexbuf[i] - '0';
			} else if (hexbuf[i] >= 'a' && hexbuf[i] <= 'f') {
				nybble = hexbuf[i] - 'a' + 0xa;
			} else if (hexbuf[i] >= 'A' && hexbuf[i] <= 'F') {
				nybble = hexbuf[i] - 'A' + 0xa;
			} else if (hexbuf[i] == '\n') {
				break;
			} else {
				errno = 1;
				break;
			}
			
			result = (result << 4) | nybble;
		}
		
	} while (errno != 0);
	
	return result;
}
