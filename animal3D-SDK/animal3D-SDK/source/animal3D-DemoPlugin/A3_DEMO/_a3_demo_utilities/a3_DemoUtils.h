
/*

	a3_DemoUtil.h
	Declarations of helpful macros.

	********************************************
	*** Declarations of utility macros.      ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOUTIL_H
#define __ANIMAL3D_DEMOUTIL_H


#include "animal3D/a3/a3types_integer.h"


//-----------------------------------------------------------------------------
// helpers for allocating and freeing memory

void __a3AssertThrow(const char* msg, const char* filename, unsigned int line);

void* __a3SafeMalloc(size_t count, size_t type_size, const char* var_name, const char* filename, unsigned int line);
void* __a3SafeRealloc(void* array_ptr, size_t count, size_t type_size, const char* var_name, const char* filename, unsigned int line);

#define a3Assert(value, msg) if ( !(value) ) __a3AssertThrow(msg, __FILE__, __LINE__)
#define a3ResizeArray(array_ptr, count, type) array_ptr = (type*)__a3SafeRealloc(array_ptr, count, sizeof(type), #array_ptr, __FILE__, __LINE__); 
#define a3AllocArray(array_ptr, count, type) array_ptr = (type*)__a3SafeMalloc(count, sizeof(type), #array_ptr, __FILE__, __LINE__);

/*
	splits a string by a separator in place (reuses the memory, replacing split character with NULL)
	outputs start of split strings
	returns: number of lines output
 */
a3i32 a3SplitString(char* str, char split_char, char** strs_out, a3i32 max_strs, a3boolean skip_empty);


/*
	reads whole file into memory and splits by line
	returns: number of lines output
	the lines are put in the same buffer so lines_out[0] must be freed
 */
a3i32 a3ReadLinesFromFile(const char* path, char** lines_out, a3i32 max_lines);


a3i32 a3ReadFileIntoMemory(const char* path, char** buf_out);

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_DEMOMACROS_H