#pragma once

#ifndef __ANIMAL3D_DEMOJSON_H
#define __ANIMAL3D_DEMOJSON_H

#include "animal3D/animal3D.h"


typedef struct a3_JSONType           a3_JSONType;
typedef struct a3_JSONValue			 a3_JSONValue;
typedef struct a3_JSONArray			 a3_JSONArray;
typedef struct a3_JSONObject		 a3_JSONObject;





struct a3_JSONValue {
	a3ui8 type;

	union {
		a3_JSONObject* obj;
		//Array* array;
		long i_num;
		double f_num;
	};

};


struct a3_JSONArray {

	a3ui32 length;
	a3_JSONValue* items; // maybe should be pool

};


struct a3_JSONObject {
	a3ui32 item_count;
	char* keys;
	a3_JSONValue* values;
};



a3i32 a3readJSONFromString(const char* buffer);
a3i32 a3readJSONFromFile(const char* path);


#endif