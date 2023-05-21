#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME_LENGTH 100
#define SIZE_OFFSET 124
#define MAX_SIZE_LENGTH 12
#define BLOCK_SIZE 512


int
devideRoundUp(int dividend, int divisor){
	int quotient = dividend / divisor;
	return (dividend % divisor > 0) ? quotient + 1 : quotient;
}

void
printNameAndSeekToNext(FILE *archive){
	long int start = ftell(archive);
	char filename[MAX_NAME_LENGTH];
	int i = 0;

	while((filename[i] = fgetc(archive)) != '\0') i++;

	printf("%s\n", filename);

	fseek(archive, start + SIZE_OFFSET, SEEK_SET);
	char filesize[MAX_SIZE_LENGTH];
	fread(filesize, sizeof(filesize[0]), MAX_SIZE_LENGTH, archive);
	int size = atoi(filesize);

	fseek(archive, start + (devideRoundUp(size, BLOCK_SIZE) + 1) * BLOCK_SIZE, SEEK_SET);
}

int
main(int argc, char *argv[]){
	FILE *archive;

	if((archive = fopen(argv[1], "r")) == NULL){
		 printf("neotevrel jsem se");
		 return 1;
	}
	
	for(int j = 0; j < 5; j++){
		printNameAndSeekToNext(archive);	
	}

	fclose(archive);
}
