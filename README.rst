Semi-Automatic-Garbage-Collector
==================================

Semi Automatic Garbage Collector for c. 
Helps in freeing the dynamic allocated memory semi automatically. 

Function like malloc, calloc realloc, etc are used to allocate dynamic memory in C Programming language, This library is designed to be used for and with such allocation. It frees the dynamic memory allocated without the need of calling free on each one of them. In many programs it has been found that many dynamic allocation are left un freed, Such a situation leads to memory leakage in the program OR It is pain to free all the memory individually knowing exactly when we don't need them. Memory allocated Dynamically Remain as it is even after the pointer to it is long gone leading to memory overhead and leakage.

This library has been created with that In mind, Aiming at the above problem specifically.

All you need to do is allocate dynamic memory using our sagc_malloc or sagc_calloc function and send the pointer which holds the address to such allocated blocks to a function and we will make sure that the block is freed whenever the pointer is destroyed.

For Example : example.c   ::

	#include <stdio.h>
	#include "sagc.h"

	int any_function()
	{
		int * r = sagc_malloc(sizeof(int));
		sagc_RegisterVar(r);
		//Your code
		//The allocated memory will be freed when the function exits.
		//OR more specifically when the variable 'r' is destroyed.
		return 0;
	}

No Need to free the variable.

There are a lot of Fully-Automatic Garbage Collector out there which frees the dynamic memory allocated without doing anything, but there is a cost to those kind of garbage collector creates a lot of overhead for your program. This is a Semi-Automatic Garbage Collector It lets you have a bit of control over your dynamic allocations while doing most of the hard work.

It lets you choose what to free automatically and what not to free.
It lets you assign destructor function which reduces a ton of overheads.
It lets you maintain the local scope of variables for dynamic memory also.
It lets you do what could not be done before like returning array from function and creating c object/class.
It is designed to be efficient and compatible with modern Abstract Data Structure and Custom made C object the latest versions of the library contain specific code where destructor functions can be called upon group of dynamically allocated memory.

For Example. :: 

	sagc_RegisterVar( variable , destructor_function );
	
	
A specific data structure can have several other dynamically allocated memories like a linked list can contain a many nodes. Now we no longer need to assign or register all those nodes all we need to do is register the starting node and a function that will free all the other nodes. Thus the overhead of freeing all of them will be reduce to one.


Documentation
-------------
 <http://dasemporium.esy.es/sagc>
 
