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


struct item{
	char *value;
	struct item *next;
};

struct item *head;


void
findPrintDelete(struct item **phead, char *filename){
	struct item *temp = *phead, *prev;

	if(temp != NULL && !strcmp(temp->value, filename)){
		*phead = temp->next;
		free(temp);
		return;
	}

	while(temp != NULL && strcmp(temp->value, filename)){
		prev = temp;
		temp = temp->next;
	}

	if(temp == NULL) return;
	
	printf("%s\n", filename);
	prev->next = temp->next;

	free(temp);
}


int
printNameAndSeekToNext(FILE *archive, struct item *head, int listAll){
	long int start = ftell(archive);
	char filename[MAX_NAME_LENGTH];
	int i = 0;

	while((filename[i] = fgetc(archive)) != '\0') i++;
	
	if(i == 0) return 1;
	
	fseek(archive, start + TYPEFLAG_OFFSET, SEEK_SET);
	char c = fgetc(archive);
	if(c != '0'){
		fprintf(stderr, "mytar: Unsupported header type: %d\n", c);
		return 2;
	}
	
	if(listAll) printf("%s\n", filename);
	else findPrintDelete(&head, filename);

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
	char option = 0;
	
	int listAll = 1;
	struct item *current;

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
				listAll = 0;
				struct item *p = (struct item *)malloc(sizeof(struct item));
				if(p == NULL) {
					fprintf(stderr, "mytar: failed to malloc\n");
					return 2;
				}
				p->value = argv[i];
				p->next = NULL;

				if(head == NULL){
					head = p;
					current = p;
				}
				else{
					current->next = p;
					current = p;
				}
				break;
			default:
				fprintf(stderr, "mytar: Unknown option: -%c\n", option);
				return 2;
			}
	}
	free(current);
	
	printf("dostal jsem se aspon sem");
	int exit;
	while(!(exit = printNameAndSeekToNext(archive, head, listAll)));
	if(exit == 2) return 2;
	
	if(head != NULL){
		for(struct item *p = head; p != NULL; p = p->next) fprintf(stderr, "mytar: %s: Not found in archive\n", p->value);
		fprintf(stderr, "mytar: Exiting with failure status due to previous errors\n");
		return 2;
	}

	fclose(archive);
}
