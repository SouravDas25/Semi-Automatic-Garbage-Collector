
#include <stdio.h>
#include "Gc.h"

typedef struct node {
	struct node * next; //8-4 bytes depending on the OS
	int data ; // 8-4 bytes depending on the OS
}node; // Total 16-8 bytes

typedef struct {
	node * head; //8-4 bytes depending on the OS
	int size; // 8-4 bytes depending on the OS
}stack; // Total 16-8 bytes

stack * newStack() //Creates a Stack Head
{
	stack * s = (stack*)sagc_malloc(sizeof(stack));// allocate memory for stack head
	s->head = NULL;
	s->size = 0;
	return s;
}

void push(stack * s,int data) //Push a integer into the stack
{
	node * ns = (node*)sagc_malloc(sizeof(stack));
	ns->next = s->head;
	ns->data = data;
	s->head = ns;
	s->size++;
}

void printstack(stack *s ) // print the stack
{
	node * tr = s->head;
	printf("\nstack[ ");
	while(tr!=NULL)
	{
		printf(" %d, ",tr->data);
		tr = tr->next;
	}
	printf(" ]");
}

void freestack(void * a ) //free the entire stack
/*the function is a destructor function for stack to use it with SaGc
  it must take the a single void * pointer to the stack like
  sagc_free function or else it won't work */
{
	stack * s = (stack*)a; // type cast the void* pointer to stack pointer
	node * tr = s->head,*p;
	while(tr!=NULL) 
	{
		p = tr;
		tr = tr->next;
		sagc_free(p); //loop and free all nodes
	}
	sagc_free(s); //free the stack head
}

void stack_program() //A Stack Program
{
	stack * s = newStack(); // create a stack
	sagc_RegisterVarFunc(s,freestack); //register the stack variable and destructor function
	push(s,23); // do wat ever you want with the  stack
	push(s,35);
	push(s,56);
	printstack(s);
	printf("\n\n Inside the stack function.");
	sagc_Garbagecheck(s); //Check for dynamic memory allocation and total menory usage
}

int main()
{
	int i;
	
	stack_program();
	printf("\n\n Outside the stack function.");
	sagc_Garbagecheck(i); // check the garbage section to see weather all memory has been freed or not
	return 0;
}
