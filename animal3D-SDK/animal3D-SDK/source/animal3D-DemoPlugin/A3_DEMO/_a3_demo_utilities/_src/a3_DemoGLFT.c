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
}





































