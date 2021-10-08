#include "../a3_DemoGLFT.h"
#include "../a3_DemoJSON.h"
#include "../a3_DemoUtils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

a3i32 a3GLFTRead(a3_GLFTFile* out_glft, const char* dirname, const char* filename) {

	a3ui32 dirname_size = (a3ui32)strlen(dirname);
	a3ui32 filename_size = (a3ui32)strlen(filename);

	char* full_path = (char*)malloc(dirname_size + filename_size + 1);
	memcpy(full_path, dirname, dirname_size);
	memcpy(full_path + dirname_size, filename, filename_size);
	full_path[dirname_size + filename_size] = 0;

	a3_JSONValue value = a3readJSONFromFile(full_path);

	a3_GLFTFile glft;

	a3_JSONValue json_nodes;
	if (a3JSONFindObjValue(value, "nodes", &json_nodes) && json_nodes.type == JSONTYPE_ARRAY) {
		printf("file missing nodes\n");
	}


	glft.nodes_count = json_nodes.length;
	a3AllocArray(glft.nodes, glft.nodes_count, a3_GLFT_Node);

	for (a3ui32 i = 0; i < glft.nodes_count; i++) {
		a3_GLFT_Node* node = glft.nodes + i;
		a3_JSONValue json_node = json_nodes.values[i];
		a3_JSONValue json_val;
		if (a3JSONFindObjValue(json_node, "children", &json_val) && json_val.type == JSONTYPE_ARRAY) {
			a3AllocArray(node->children, json_val.length, a3ui32);
			node->children_count = json_val.length;
		}


		if (a3JSONFindObjValue(json_node, "name", &json_val)) {
			a3ui32 len = json_val.length;
			if (len > 100) len = 100;
			memcpy(node->name, json_val.str, len);
			node->name[len] = 0;
		}

		node->matrix = a3mat4_identity;

		if (a3JSONFindObjValue(json_node, "matrix", &json_val)) {
			node->usesMatrix = 1;
			for (a3ui8 i = 0; i < 16; i++) {
				(&node->matrix.m00)[i] = (float)json_val.values[i].num;
			}

		}
		
		if (a3JSONFindObjValue(json_node, "translation", &json_val)) {
			for (a3ui8 i = 0; i < 3; i++) {
				(&node->translation.x)[i] = (float)json_val.values[i].num;
			}
		}

		if (a3JSONFindObjValue(json_node, "rotation", &json_val)) {
			for (a3ui8 i = 0; i < 4; i++) {
				(&node->rotation.x)[i] = (float)json_val.values[i].num;
			}
		}

		if (a3JSONFindObjValue(json_node, "scale", &json_val)) {
			for (a3ui8 i = 0; i < 3; i++) {
				(&node->scale.x)[i] = (float)json_val.values[i].num;
			}
		}



	}





























	// start anim loading
	a3ui32 filename_size = strlen(filename);
	const char* uri;
	a3ui32 byteLength;

	a3_JSONValue json_buffer;
	if (a3JSONFindObjValue(value, "buffers", &json_buffer) && json_nodes.type == JSONTYPE_ARRAY) 
	{
		a3_JSONValue json_uri;
		for (a3ui32 j = 0; j < json_buffer.length; j++)
		{
			if (a3JSONFindObjValue(json_buffer.values[j], "byteLength", &json_uri) && json_nodes.type == JSONTYPE_NUM)
			{
				byteLength = json_uri.num;
			}
			if (a3JSONFindObjValue(json_buffer.values[j], "uri", &json_uri) && json_nodes.type == JSONTYPE_ARRAY) 
			{
				uri = json_uri.str;
			}
		}
	}
	char* buffer_path = (char*)malloc(dirname_size + strlen(uri) + 1);
	memcpy(buffer_path, dirname, dirname_size);
	memcpy(buffer_path + dirname_size, filename, filename_size);
	buffer_path[dirname_size + filename_size] = 0;

	char** tempBuffer;
	a3ReadFileIntoMemory(buffer_path, tempBuffer);


	*out_glft = glft;
	return -1;

}





































