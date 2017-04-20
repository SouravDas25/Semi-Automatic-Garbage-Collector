Semi-Automatic-Garbage-Collector
==================================

Semi Automatic Garbage Collector for c. 
Helps in freeing the dynamic allocated memory semi automatically. 

Function like malloc, calloc realloc, etc are used to allocate dynamic memory in C Programming language, This library is designed to be used for and with such allocation. It frees the dynamic memory allocated without the need of calling free on each one of them. In many programs it has been found that many dynamic allocation are left un freed, Such a situation leads to memory leakage in the program OR It is pain to free all the memory individually knowing exactly when we don't need them. Memory allocated Dynamically Remain as it is even after the pointer to it is long gone leading to memory overhead and leakage.

This library has been created with that In mind, Aiming at the above problem specifically.

All you need to do is allocate dynamic memory using our sagc_malloc or sagc_calloc function and send the pointer which holds the address to such allocated blocks to a function and we will make sure that the block is freed whenever the pointer is destroyed.

For Example : example.c   ::

	#include <stdio.h>
	#include "Gc.h"

	int any_function()
	{
		int * r = sagc_malloc(sizeof(int));
		sagc_RegisterVar(r);
		//Your code
		//The allocated memory will be freed when the function exits.
		//OR more specifically when the variable 'r' is destroyed.
		return 0;
	}
