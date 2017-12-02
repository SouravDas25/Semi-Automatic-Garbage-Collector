


#ifndef garbagecollector_H
#define garbagecollector_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>
#include<math.h>
#include<stdarg.h>
#include<time.h>
#include<stdbool.h>

#define MemoryError(...)   printf("\nError Captured In file %s In line %d \n\nMemoryError : ",__FILE__,__LINE__);\
printf(__VA_ARGS__);printf("\n\n");\
system("pause");exit(EXIT_FAILURE)

#define checkMemory(n) if(n == NULL){MemoryError("Out of Memory");}

typedef unsigned int UINT;

/*allocate dynamic memory in a safe way maintaing a stack of occupied element .*/
void* sagc_calloc(int n,int size);

/*allocate dynamic memory in a safe way maintaing a stack of occupied element .*/
void* sagc_malloc(UINT size);

/*prints the occupied stack .*/
#define sagc_Dump(n) __sagc_Dump((void*)&n)
void __sagc_Dump(void * add);

void sagc_memCheck();

/*frees the memory from occupied stack .*/
void sagc_free(void* a);

#define sagc_ExtendVar sagc_RegisterVar
#define sagc_RegisterVarFunc(n,func) sagc_inner_RegisterVar((void**)&n,func)
#define sagc_RegisterVar(n) sagc_inner_RegisterVar((void**)&n,NULL)
void sagc_inner_RegisterVar(void ** add,void (*destructor)(void*));

void sagc_flush(void * add);

#endif
