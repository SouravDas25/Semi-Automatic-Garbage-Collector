

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

#include "gc.h"

typedef unsigned int UINT;

typedef struct ads_gcpointer { /*stack node struct declaration*/
	void * allocated ;
	struct ads_gcpointer* next ;
	short size;
	short NumberOfItems;
} ads_gcpointer ;

int64_t ADS_CurrentMemoryActive = 0;
int64_t ADS_GC_MaxMemoryActive = 0;
static int __maxGCalocateinlist__ = 0; /*maximum number of dynamically alocated data active in occupied stack at some point */
static int __LenGCcalls__ = 0; /*current no of active dynamically allocated data in occupied stack */
static int __TotalGCcalls__ = 0; /*total no of call made to mycalloc*/
static ads_gcpointer * __gcobjhead__ = NULL; /*stack head for occupied stack */

void* sagc_calloc(int n,int size) /*allocate dynamic memory in a safe way maintaing a stack of occupied element */
{
	void * b = calloc(n,size);
	ads_gcpointer* p = (ads_gcpointer*)calloc(1,sizeof(ads_gcpointer));
	checkMemory(b);checkMemory(p);
	p->allocated = b;
	p->NumberOfItems = n;
	p->size = size;
	p->next = __gcobjhead__;
	__gcobjhead__ = p;
	ADS_CurrentMemoryActive += n*size;
	__TotalGCcalls__++;
	__LenGCcalls__++;
	if(__LenGCcalls__ > __maxGCalocateinlist__)
	__maxGCalocateinlist__ = __LenGCcalls__ ;
	if(ADS_CurrentMemoryActive > ADS_GC_MaxMemoryActive ) ADS_GC_MaxMemoryActive = ADS_CurrentMemoryActive;
	sagc_flush(&n);
	return b;
}

void* sagc_malloc(UINT size) /*allocate dynamic memory in a safe way maintaing a stack of occupied element */
{
	return sagc_calloc(1,size);
}

void sagc_inner_Garbagecheck(void * add) /*prints the occupied stack */
{
	sagc_flush(add);
	ads_gcpointer * trav ,*prev ;
	trav = __gcobjhead__;
 	prev = NULL;
	
	printf("\n\n\n Allocated [ ");
	while(trav != NULL)
	{
		printf("\naddress = %p, blocks = %d , size = %d ",trav->allocated,trav->NumberOfItems,trav->size);
		prev = trav;
		trav = trav->next;
	}
	trav = prev;
	printf(" ]\n\n MaxGC = %d ,ActiveAllocated = %d ,Totalcalls = %d\n",__maxGCalocateinlist__,__LenGCcalls__,__TotalGCcalls__);
	printf(" Current Dynamic Memory(in use) = %I64d KiB %I64d Bytes.\n",ADS_CurrentMemoryActive/1024,ADS_CurrentMemoryActive%1024);
	printf(" Max Dynamic Memory = %I64d KiB %I64d Bytes.\n",ADS_GC_MaxMemoryActive/1024,ADS_GC_MaxMemoryActive%1024);
}

void sagc_GC_MemoryCheck()
{
	printf(" MaxGC = %d ,ActiveAllocated = %d ,Totalcalls = %d\n",__maxGCalocateinlist__,__LenGCcalls__,__TotalGCcalls__);
	printf(" Current Dynamic Memory(in use) = %I64d KiB %I64d Bytes.\n",ADS_CurrentMemoryActive/1024,ADS_CurrentMemoryActive%1024);
	printf(" Max Dynamic Memory = %I64d KiB %I64d Bytes.\n",ADS_GC_MaxMemoryActive/1024,ADS_GC_MaxMemoryActive%1024);
}

void sagc_free(void* a) /*frees the memory from occupied stack */
{
	ads_gcpointer* trav,*prev;
	if(a == NULL)
	{
		MemoryError("Cannot Free a NULL pointer");
	}
	trav = __gcobjhead__;
	prev = NULL;
	while(trav != NULL)
	{
		if(trav->allocated == a)
		{
			ADS_CurrentMemoryActive -= trav->NumberOfItems*trav->size;
			if(prev == NULL) __gcobjhead__ = trav->next;
			else prev->next = trav->next;
			free(a);
			free(trav);
			__LenGCcalls__--;
			return;
		}
		prev = trav;
		trav = trav->next;
	}
	MemoryError("Memory address %p provided is a dangling pointer",a);
}

ads_gcpointer* get_Gc_Obj_Head(void*allocated)
{
	ads_gcpointer* trav;
	trav = __gcobjhead__;
	while(trav != NULL)
	{
		if(trav->allocated == allocated)
		{
			return trav;
		}
		trav = trav->next;
	}
	return NULL;
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

