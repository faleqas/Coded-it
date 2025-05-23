/*GENERAL PURPOSE STACK*/
#pragma once
#include <stdbool.h>

typedef struct
{
    char* arr;
    int len;
    size_t element_size;
} Stack;



void Stack_init(Stack* stack, size_t element_size);
void Stack_push(Stack* stack, char* element);

//NOTE(omar): THIS FUNCTION RETURNS A COPY OF THE LAST ELEMENT.
//            MAKE SURE TO FREE() AFTER YOU ARE DONE
char* Stack_pop(Stack* stack, bool return_element); 

//Get element at specified index
//returns the element itself, not a copy
char* Stack_get(Stack* stack, int index);

void Stack_clear(Stack* stack);