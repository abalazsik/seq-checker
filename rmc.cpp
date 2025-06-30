
#define S_NORMAL 0
#define S_IN_LINE_COMMENT 1
#define S_IN_BLOCK_COMMENT 2

#include <string.h>

extern "C" void remove_comments(char* buffer) {
		unsigned int length = strlen(buffer);
		
		int state = S_NORMAL;
		unsigned int src_idx = 0, dest_idx = 0;
		
		while(src_idx < length) {
			if (state == S_NORMAL && buffer[src_idx] == '/' && buffer[src_idx + 1] == '/') { // S_NORMAL -> S_IN_LINE_COMMENT
				state = S_IN_LINE_COMMENT;
				src_idx++;
			} else if (state == S_NORMAL && buffer[src_idx] == '/' && buffer[src_idx + 1] == '*') { // S_NORMAL -> S_IN_BLOCK_COMMENT
				state = S_IN_BLOCK_COMMENT;
				src_idx++;
			} else if (state == S_IN_BLOCK_COMMENT && buffer[src_idx] == '*' && buffer[src_idx + 1] == '/') { // S_IN_LINE_COMMENT -> S_NORMAL
				state = S_NORMAL;
				src_idx++;
			} else if (state == S_IN_LINE_COMMENT && buffer[src_idx] == '\n') { // S_IN_LINE_COMMENT -> S_NORMAL
				buffer[dest_idx] = buffer[src_idx];
				state = S_NORMAL;
			} else if (state == S_NORMAL) { // S_NORMAL -> S_NORMAL
				if (src_idx != dest_idx) {
					buffer[dest_idx] = buffer[src_idx];
				}
				dest_idx++;
			}
			src_idx++;
		}

		while(dest_idx < length) {
			buffer[dest_idx++] = (char)0;
		}
	}

/*

// for manual testing

#include <stdio.h>
#include <stdlib.h>

#define SIZE 8192

int main (int argc, char** argv) {
	
	if(argc < 2) {
		perror("Provide an input file!");
		return 1;
	}
	
	FILE *inputFile = fopen(argv[1], "r");
	
	if (!inputFile) {
		perror("Failed to open the file!");
		return 2;
	}
	
	char buffer[SIZE];
	
	size_t length = fread(buffer, sizeof(char), SIZE, inputFile);
	
	fclose(inputFile);

	remove_comments(buffer);
	
	printf("%s", buffer);
	
	return 0;
}

*/