
/*

	a3_DemoUtil.h
	Declarations of helpful macros.

	********************************************
	*** Declarations of utility macros.      ***
	********************************************
*/

#ifndef __ANIMAL3D_DEMOUTIL_H
#define __ANIMAL3D_DEMOUTIL_H


//-----------------------------------------------------------------------------
// helpers for allocating and freeing memory

void __a3AssertThrow(const char* msg, const char* filename, unsigned int line);

#define a3Assert(value, msg) if ( !(value) ) __a3AssertThrow(msg, __FILE__, __LINE__)
#define a3ResizeArray(ptr, count, type) ptr = (type*)realloc(ptr, count * sizeof(type)); a3Assert(ptr, "unable to realloc " #ptr)
#define a3AllocArray(ptr, count, type) ptr = (type*)malloc(count * sizeof(type)); a3Assert(ptr, "unable to malloc " #ptr);

//-----------------------------------------------------------------------------


#endif	// !__ANIMAL3D_DEMOMACROS_H