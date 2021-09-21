#pragma once

#ifndef __ANIMAL3D_DEMOJSON_H
#define __ANIMAL3D_DEMOJSON_H

#include "animal3D/animal3D.h"


typedef struct a3_JSONType           a3_JSONType;
typedef struct a3_JSONValue			 a3_JSONValue;
typedef struct a3_JSONArray			 a3_JSONArray;
typedef struct a3_JSONObject		 a3_JSONObject;



/*
 Tokenize:
 outputs nice buffer of meaning values with a type to make it easier to parse,
 converts strings to numbers
 deals with escape characters

 Parser:
 validates json structure and builds values, objects and arrays from tokens

*/
struct a3_JSONToken {

};


// holds all the data taken from a file
// stores pool of other json items
struct a3_JSONReader {
	
	a3_JSONValue* values;

	a3_JSONObject* objects;

	a3_JSONArray* arrays;

	const char* keys;
	const char* strings;
};


struct a3_JSONValue {
	a3_JSONType type;

	union {
		Object* obj;
		Array* array;
		long i_num;
		double f_num;
	};
}


struct a3_JSONArray {

	a3ui32 length;
	// 
	a3ui32 items_index;

}


struct a3_JSONObject {
	uint32 items;
	a3ui32 keys_index;
	a3ui32 values_index;
}







#endif