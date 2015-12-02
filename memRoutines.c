#include <math.h>
#include "memStruct.h"

extern struct fb* headFreeB;
extern struct ab* headAllocatedB;
extern char* memoryAdress;
extern mem_fit_function_t* strategy;
extern int instant;
extern int dataFile;


int shouldSplitFreeBlock(struct fb* fb, int askedSize){
	int paddingForAlignement;
	int totalSizeOfNewBlock = askedSize ;
	
	paddingForAlignement = (__BIGGEST_ALIGNMENT__ - ((totalSizeOfNewBlock + sizeof(struct fb)) % __BIGGEST_ALIGNMENT__)) % __BIGGEST_ALIGNMENT__ ;
	totalSizeOfNewBlock += paddingForAlignement;
	if ( fb->size > totalSizeOfNewBlock +1  ) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}


void addPadding( struct fb** fb, int askedSize){
	int paddingForAlignement;
	int totalSizeOfNewBlock;
		
	totalSizeOfNewBlock = askedSize + sizeof(struct ab);		
	paddingForAlignement = (__BIGGEST_ALIGNMENT__ - (totalSizeOfNewBlock % __BIGGEST_ALIGNMENT__)) % __BIGGEST_ALIGNMENT__ ;
	(*fb)->size = askedSize + paddingForAlignement;
	
	return;
}

void splitFreeBlock( struct fb** fb, int askedSize){
	int paddingForAlignement;
	int totalSizeOfNewBlock;
	struct fb* pointerToLeftPart = *fb;
	struct fb* pointerToRightPart;

	totalSizeOfNewBlock = askedSize + sizeof(struct ab);		
	paddingForAlignement = (__BIGGEST_ALIGNMENT__ - (totalSizeOfNewBlock % __BIGGEST_ALIGNMENT__)) % __BIGGEST_ALIGNMENT__ ;
	totalSizeOfNewBlock += paddingForAlignement;
	
	pointerToRightPart =(struct fb*) ((char*)pointerToLeftPart + totalSizeOfNewBlock); //adresse du nouveau block libre a inserer 
	pointerToRightPart->size = pointerToLeftPart->size + sizeof(struct fb) - (totalSizeOfNewBlock + sizeof(struct fb));
	pointerToRightPart->next = pointerToLeftPart->next ;
	
	pointerToLeftPart->size = askedSize + paddingForAlignement;
	pointerToLeftPart->next = pointerToRightPart;
	
	return;
} 

void extractFB( struct fb** fb){

	if ((*fb)->next!=NULL){
		(*fb)= (*fb)->next;
		}
	else{
		(*fb) =NULL;
	}

	return;
}

void insertFB(struct fb* toInsert){
	struct fb** pointerFromPreviousBlock = &headFreeB;
	struct fb* nextBlock = *pointerFromPreviousBlock;
	struct fb* previousBlock = headFreeB ;
	int flag = FALSE;

	while ( ((long long unsigned)toInsert > (long long unsigned)nextBlock) && (nextBlock!=NULL) ){
		if (flag){ previousBlock = previousBlock->next;}	
		pointerFromPreviousBlock = &(nextBlock->next);
		nextBlock = nextBlock->next;
		flag = TRUE;
		}
	
	toInsert->next = nextBlock;
	*pointerFromPreviousBlock = toInsert;
	if ((char*)toInsert + sizeof (struct fb) + toInsert->size ==  (char*)nextBlock){ //Fusion aval
		fusionFB(toInsert,nextBlock);
		}
	
	if (!flag)return;
		if ((char*)previousBlock + sizeof (struct fb) + previousBlock->size ==  (char*)toInsert){ //Fusion amont
			fusionFB(previousBlock,toInsert);
			}
	return;
}
	
void fusionFB(struct fb* leftBlock, struct fb* rightBlock){
	leftBlock->size = leftBlock->size + rightBlock->size + sizeof (struct fb);
	extractFB(&(leftBlock->next));
	return;	
}


int sumMyMemory(){
	int sum = 0;
	struct fb* f = headFreeB;
	struct ab* a = headAllocatedB;
	
	while (f!=NULL){
		sum += f->size;
		sum += sizeof(struct fb);
		f = f->next;
		}
	
	while (a!=NULL){
		sum += a->size;
		sum += sizeof(struct ab);
		a = a->next;
		}

	return sum;
}




#include <stdio.h>
void writeInt(int fd, long long unsigned val){
  int nbDigits(long long unsigned int x){
    if ( x == 0){
      return 1;
    }else{
      return (unsigned int)log10(val)+1;
    }
  }	
  int n = nbDigits(val);
  char string[n];
  unsigned int i = n;
  

  string[0]='0';
  while ( i > 0 ) {
    i--;
    string[i] = '0' + (val % 10);
    val = val / 10;
  }

  write(fd,&string,n);
  return;
}

extern int memInitialSize;
#include <assert.h>

void memorySnapshot(){
  
  int userMemoryUsage = 0;
  int allocMemoryUsage = 0;
  void* lastUsedByte = memoryAdress;
  struct ab* a = headAllocatedB;
  int fd = dataFile;

  assert(sumMyMemory()==memInitialSize);
  while (a!=NULL){
    userMemoryUsage += a->size;
    lastUsedByte = (void*)((char*)a + (int)(a->size) + (int)sizeof(struct ab) - (unsigned long long)memoryAdress);
    if (allocMemoryUsage<=(long long unsigned)lastUsedByte){
      allocMemoryUsage=(long long unsigned)lastUsedByte;
      }
    a = a->next;
  }

  writeInt(fd, instant);
  write(fd," ",1);
  writeInt(fd, userMemoryUsage);
  write(fd," ",1);
  writeInt(fd, allocMemoryUsage);
  write(fd,"\n",1);

  instant++;
  return ;

}



