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


int
getSizeOfFile(FILE *archive, long int start){
	fseek(archive, start + SIZE_OFFSET, SEEK_SET);
	char filesize[MAX_SIZE_LENGTH];
	fread(filesize, sizeof(filesize[0]), MAX_SIZE_LENGTH, archive);
	return atoi(filesize);
}


int
printNameAndSeekToNext(FILE *archive){
	long int start = ftell(archive);
	char filename[MAX_NAME_LENGTH];
	int i = 0;

	while((filename[i] = fgetc(archive)) != '\0') i++;
	
	if(i == 0) return 0;

	printf("%s\n", filename);

	fseek(archive, start + (devideRoundUp(getSizeOfFile(archive, start), BLOCK_SIZE) + 1) * BLOCK_SIZE, SEEK_SET);
	return 1;
}

int
main(int argc, char *argv[]){
	FILE *archive;

	if((archive = fopen(argv[1], "r")) == NULL){
		 printf("neotevrel jsem se");
		 return 1;
	}
	
	while(printNameAndSeekToNext(archive));	

	fclose(archive);
}
