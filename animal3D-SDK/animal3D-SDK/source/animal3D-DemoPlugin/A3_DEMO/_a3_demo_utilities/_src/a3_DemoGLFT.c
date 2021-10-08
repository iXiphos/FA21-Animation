#include "../a3_DemoGLFT.h"
#include "../a3_DemoJSON.h"
#include "../a3_DemoUtils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

a3i32 a3GLFTRead(a3_GLFTFile* out_glft, const char* dirname, const char* filename) {

	a3ui32 dirname_size = strlen(dirname);
	a3ui32 filename_size = strlen(filename);

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
		a3_JSONValue json_children;
		if (a3JSONFindObjValue(json_nodes.values[i], "children", &json_children) && json_children.type == JSONTYPE_ARRAY) {
			a3AllocArray(node->children, json_children.length, a3ui32);
			node->children_count = json_children.length;
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



}





































