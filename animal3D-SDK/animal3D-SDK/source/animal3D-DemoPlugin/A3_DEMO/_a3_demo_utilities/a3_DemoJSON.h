#pragma once

#ifndef __ANIMAL3D_DEMOJSON_H
#define __ANIMAL3D_DEMOJSON_H

#include "animal3D/animal3D.h"


typedef struct a3_JSONType           a3_JSONType;
typedef struct a3_JSONValue			 a3_JSONValue;
typedef struct a3_JSONArray			 a3_JSONArray;
typedef struct a3_JSONObject		 a3_JSONObject;





struct a3_JSONValue {
	uint8 type;

	union {
		Object* obj;
		Array* array;
		long i_num;
		double f_num;
	};

}


struct a3_JSONArray {

	uint32 length;
	JSONValue* items; // maybe should be pool

}


struct a3_JSONObject {
	uint32 items;
	char* keys;
	JSONValue* values;
}







#endif