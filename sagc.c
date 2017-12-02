

#ifndef garbagecollector_C
#define garbagecollector_C

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>
#include<math.h>
#include<stdarg.h>
#include<time.h>
#include<stdbool.h>
#include<stdint.h>

#include "sagc.h"

typedef unsigned int UINT;

typedef struct ads_gcpointer { /*stack node struct declaration*/
    struct ads_gcpointer* prev ;
	struct ads_gcpointer* next ;
	short size;
	int NumberOfItems;
} ads_gcpointer ;

uint64_t ADS_CurrentMemoryActive = 0;
uint64_t ADS_GC_MaxMemoryActive = 0;
uint64_t ads_maxGCalocateinlist__ = 0; /*maximum number of dynamically alocated data active in occupied ads_stack at some point */
uint64_t ads_LenGCcalls__ = 0; /*current no of active dynamically allocated data in occupied ads_stack */
uint64_t ads_TotalGCcalls__ = 0; /*total no of call made to mycalloc*/
ads_gcpointer * ads_gcobjhead__ = NULL; /*ads_stack head for occupied ads_stack */

void * ads_getGcAllocatedBlock(ads_gcpointer* gcblock )
{
    return ((char*)gcblock) + sizeof(ads_gcpointer);
}

ads_gcpointer* ads_getGcBlock(void*allocated)
{
    return (ads_gcpointer*)(((char*)allocated) - sizeof(ads_gcpointer));
}

void* sagc_calloc(int n,int size) /*allocate dynamic memory in a safe way maintaing a stack of occupied element */
{
	ads_gcpointer*p = (ads_gcpointer*)calloc(n,sizeof(ads_gcpointer)+size);
	checkMemory(p);
    void * allocated = ads_getGcAllocatedBlock(p);
	p->NumberOfItems = n;
	p->size = size;
	p->prev = NULL;
	p->next = ads_gcobjhead__;
	if(ads_gcobjhead__)ads_gcobjhead__->prev = p;
	ads_gcobjhead__ = p;
	ADS_CurrentMemoryActive += n*size;
	ads_TotalGCcalls__++;
	ads_LenGCcalls__++;
	if(ads_LenGCcalls__ > ads_maxGCalocateinlist__)
	ads_maxGCalocateinlist__ = ads_LenGCcalls__ ;
	if(ADS_CurrentMemoryActive > ADS_GC_MaxMemoryActive ) ADS_GC_MaxMemoryActive = ADS_CurrentMemoryActive;
	sagc_flush(&n);
	return allocated;
}

void* sagc_malloc(UINT size) /*allocate dynamic memory in a safe way maintaing a stack of occupied element */
{
	return sagc_calloc(1,size);
}

void __sagc_Dump(void * add) /*prints the occupied stack */
{
	sagc_flush(add);
	ads_gcpointer * trav ,*prev ;
	trav = ads_gcobjhead__;
	prev = NULL;
	printf("\n\n\n Allocated [ ");
	while(trav != NULL)
	{
		printf("\n%p, blocks = %d , size = %d ",ads_getGcAllocatedBlock(trav),trav->NumberOfItems,trav->size);
		prev = trav;
		trav = trav->next;
	}
	trav = prev;
	printf(" ]\n\n MaxGC = %I64d ,ActiveAllocated = %I64d ,Totalcalls = %I64d\n",ads_maxGCalocateinlist__,ads_LenGCcalls__,ads_TotalGCcalls__);
	printf(" Current Dynamic Memory = %I64d MiB %I64d KiB %I64d Bytes.\n",ADS_CurrentMemoryActive/1048576,(ADS_CurrentMemoryActive/1024)%1048576,ADS_CurrentMemoryActive%1024);
	printf(" Max Dynamic Memory = %I64d MiB %I64d KiB %I64d Bytes.\n",ADS_GC_MaxMemoryActive/1048576,(ADS_GC_MaxMemoryActive/1024)%1048576,ADS_GC_MaxMemoryActive%1024);
}

void sagc_memCheck()
{
	printf("\n\n MaxGC = %I64d ,ActiveAllocated = %I64d ,Totalcalls = %I64d\n",ads_maxGCalocateinlist__,ads_LenGCcalls__,ads_TotalGCcalls__);
	printf(" Current Dynamic Memory = %I64d MiB %I64d KiB %I64d Bytes.\n",ADS_CurrentMemoryActive/1048576,(ADS_CurrentMemoryActive/1024)%1048576,ADS_CurrentMemoryActive%1024);
	printf(" Max Dynamic Memory = %I64d MiB %I64d KiB %I64d Bytes.\n",ADS_GC_MaxMemoryActive/1048576,(ADS_GC_MaxMemoryActive/1024)%1048576,ADS_GC_MaxMemoryActive%1024);
}

void sagc_free(void* a) /*frees the memory from occupied stack */
{
	ads_gcpointer* gcblock = ads_getGcBlock(a);
	if(a == NULL)
	{
		printf("MemoryError : Cannot Free a NULL pointer.");
	}
	ADS_CurrentMemoryActive -= gcblock->NumberOfItems*gcblock->size;
    if(!gcblock->prev)
    {
        ads_gcobjhead__ = gcblock->next;
        if(ads_gcobjhead__)ads_gcobjhead__->prev = NULL;
    }
    else
    {
        gcblock->prev->next = gcblock->next;
        if(gcblock->next) gcblock->next->prev = gcblock->prev;
    }
    free(gcblock);
    ads_LenGCcalls__--;
}

ads_gcpointer* get_Gc_Obj_Head(void*allocated)
{
	return ads_getGcBlock(allocated);
}

typedef struct ads_Gc_variable {
	void ** address;
	void * value ;
	void (*destructor)(void*a);
	struct ads_Gc_variable * next;
 	int direction;
} ads_Gc_variable ;

ads_Gc_variable * ads_gc_var_head = NULL;

ads_Gc_variable * sagc_Get_GcVarHead(void * value)
{
	ads_Gc_variable * trav = ads_gc_var_head;
	while( trav != NULL )
	{
		if(trav->value == value) return trav;
		trav = trav->next;
	}
	return NULL;
}

void sagc_inner_RegisterVar(void ** add,void (*destructor)(void*))
{
	ads_Gc_variable * trav = sagc_Get_GcVarHead(*add);
	if(trav != NULL)
	{
		if(trav->address < add && trav->direction == 1)trav->address = add;
		else if( trav->address > add && trav->direction == -1)trav->address = add;
		return;
	}
	ads_Gc_variable * p = (ads_Gc_variable*)calloc(1,sizeof(ads_Gc_variable));
	p->address = add;
	p->value = *add;
	p->next = ads_gc_var_head ;
	p->destructor = destructor;
 	p->direction = (&add < (void***)add)? 1:-1;
	ads_gc_var_head = p;
}

bool sagc_ResolveScope(void * currentadd,void*add,int cmp)
{
	if( currentadd - add > 0xffffff ) return false;
	if(cmp == 1)
	{
		if( currentadd > add ) return true;
		return false;
	}
	else if(cmp == -1)
	{
		if( currentadd < add ) return true;
		return false;
	}
	return false;
}

void sagc_flush(void* currentadd)
{
	ads_Gc_variable * trav = ads_gc_var_head,*prev = NULL;
	void**add ;
	while( trav != NULL )
	{
		add = trav->address;
		if( *add != trav->value || sagc_ResolveScope(currentadd,add,trav->direction) )
		{
			if( get_Gc_Obj_Head(trav->value) != NULL )
			{
				if(trav->destructor) trav->destructor(trav->value);
				else sagc_free(trav->value);
			}
			if(prev == NULL) ads_gc_var_head = trav->next;
			else prev->next = trav->next;
			free(trav);
		}
		prev = trav;
		trav = trav->next;
	}
}

void sagc_print_GcVar()
{
	ads_Gc_variable * trav = ads_gc_var_head;
	printf("\n\nGcVar[ ");
	while( trav != NULL )
	{
		printf("%p:%p,",trav->address,trav->value);
		trav = trav->next;
	}
	printf(" ]");
}

#endif

