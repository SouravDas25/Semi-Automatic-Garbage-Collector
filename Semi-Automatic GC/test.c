
#include <stdio.h>
#include "Gc.h"

int do_nothing()
{
	int * r = sagc_malloc(sizeof(int));
	sagc_RegisterVar(r);
	printf("\nDoNothing.r = %p ",r);
	return 0;
}

int main()
{
	do_nothing();
	int * r = sagc_malloc(sizeof(int));
	printf("\nmain.r = %p ",r);

	//Leak Detection Function
	sagc_Garbagecheck(r);
	return 0;
}

