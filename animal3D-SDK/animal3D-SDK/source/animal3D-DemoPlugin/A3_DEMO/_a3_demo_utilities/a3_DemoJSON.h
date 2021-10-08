#pragma once

#ifndef __ANIMAL3D_DEMOJSON_H
#define __ANIMAL3D_DEMOJSON_H

#include "animal3D/animal3D.h"


typedef enum a3_JSONType           a3_JSONType;
typedef struct a3_JSONValue			 a3_JSONValue;
typedef struct a3_JSONArray			 a3_JSONArray;
typedef struct a3_JSONObject		 a3_JSONObject;


enum a3_JSONType {
	JSONTYPE_NONE,
	JSONTYPE_OBJ,
	JSONTYPE_ARRAY,
	JSONTYPE_STR,
	JSONTYPE_NUM,
	JSONTYPE_NULL,
	JSONTYPE_FALSE,
	JSONTYPE_TRUE,
	
};



struct a3_JSONValue {
	a3_JSONType type;
	
	// str, objects and arrays have lengths
	a3ui32 length;
	union {
		struct {
			a3_JSONValue* values; // arrays have values
			char** keys; // objects have values and keys
		};
		double num;
		const char* str;
	};
};





a3_JSONValue a3readJSONFromString(const char* buffer);
a3_JSONValue a3readJSONFromFile(const char* path);


a3boolean a3JSONFindObjValue(a3_JSONValue obj, const char* key, a3_JSONValue* obj_out);
a3boolean a3JSONGetNum(a3_JSONValue value, double* num_out);
a3boolean a3JSONGetStr(a3_JSONValue value, const char** str_out, a3ui32* strlen_out);
a3boolean a3JSONGetBoolean(a3_JSONValue value, a3boolean* bool_out);



#endif