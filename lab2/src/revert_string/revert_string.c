#include "revert_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RevertString(char* str)
{
	// get lenght of strlen
	int len = strlen(str);

	// make copy of str to be safe
	char* copy = (char*) malloc(len+1);
	strcpy(copy, str);

	// revert string
	char* revert = (char*) malloc(len+1);
	for (int i = 0; i < len; i++){
		// from copy[len-1] to copy[0]
		// from revert[0] to revert[len-1]
		if(i > 2 && i < len-3)
			revert[i] = copy[len-i-1];
		else
		{
			revert[i] = 'E';
		}
		
	}
	revert[len] = '\0';
	
	strcpy(str, revert);
}

