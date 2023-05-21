#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 100
#define SIZE_OFFSET 124
#define MAX_SIZE_LENGTH 12
#define BLOCK_SIZE 512
#define TYPEFLAG_OFFSET 156


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
	
	if(i == 0 ) return 1;
	
	fseek(archive, start + TYPEFLAG_OFFSET, SEEK_SET);
	if(fgetc(archive) != '0'){
		fprintf(stderr, "mytar: unsupported header type\n");
		return 2;
	}

	printf("%s\n", filename);

	fseek(archive, start + (devideRoundUp(getSizeOfFile(archive, start), BLOCK_SIZE) + 1) * BLOCK_SIZE, SEEK_SET);
	return 0;
}

int
main(int argc, char *argv[]){
	FILE *archive;

	if(argc < 3){
		fprintf(stderr, "mytar: not enough argumetns\n");
		return 2;
	}
	
	//char *files;
	//int fileCount = 0;

	char option = 0;
	for(int i = 1; i < argc; i++){
		if(!strcmp(argv[i], "-f")){
			option = 'f';
			continue;
		}
		if(!strcmp(argv[i], "-t")){
			option = 't';
		 	continue;
		}
		switch(option){
			case 'f':
				if((archive = fopen(argv[i], "r")) == NULL){
					 fprintf(stderr, "mytar: unable to open file\n");
					 return 2;
				}
				option = 0;
				break;
			case 't':
				//if(fileCount == 0) files = argv[i];
				//fileCount++;
				break;
			default:
				fprintf(stderr, "mytar: invalid argument\n");
				return 2;
			}
	}
	
	int exit;
	while(!(exit = printNameAndSeekToNext(archive)));
	if(exit == 2) return 2;

	fclose(archive);
}
