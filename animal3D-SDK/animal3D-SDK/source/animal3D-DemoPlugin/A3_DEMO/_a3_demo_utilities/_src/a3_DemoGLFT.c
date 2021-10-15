#include "../a3_DemoGLFT.h"
#include "../a3_DemoJSON.h"
#include "../a3_DemoUtils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void a3GLTF_ReadName(a3_JSONValue object, char dst[100]) {
	a3_JSONValue json_name;
	if (a3JSONFindObjValue(object, "name", &json_name) != 1) {
		printf("error: name missing"); return;
	}

	char* src;
	a3ui32 len;
	if (a3JSONGetStr(json_name, &src, &len) != 1) {
		printf("error: name was not of type string"); return;
	}

	if (len > 100) {
		len = 100;
		printf("warning: name %s was truncated\n", src);
	}
	memcpy(dst, src, len);
	dst[len] = 0;
	
}

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
	if (!a3JSONFindObjValue(value, "nodes", &json_nodes) || json_nodes.type != JSONTYPE_ARRAY) {
		printf("file missing nodes\n");
	}

	/*
	LOAD NODES
	*/
	glft.nodes_count = json_nodes.length;
	a3AllocArray(glft.nodes, glft.nodes_count, a3_GLFT_Node);
	memset(glft.nodes, 0, glft.nodes_count * sizeof(a3_GLFT_Node));

	for (a3ui32 i = 0; i < glft.nodes_count; i++) {
		glft.nodes[i].parent = -1;
	}

	for (a3ui32 i = 0; i < glft.nodes_count; i++) {
		a3_GLFT_Node* node = glft.nodes + i;
		a3_JSONValue json_node = json_nodes.values[i];
		a3_JSONValue json_val;

		node->index = i;

		if (a3JSONFindObjValue(json_node, "children", &json_val) && json_val.type == JSONTYPE_ARRAY) {
			a3AllocArray(node->children, json_val.length, a3ui32);
			node->children_count = json_val.length;

			for (a3ui32 j = 0; j < json_val.length; j++) {
				a3ui32 childIndex = (a3ui32)json_val.values[j].num;
				node->children[j] = childIndex;
				glft.nodes[childIndex].parent = node->index;
			}
		}

		a3GLTF_ReadName(json_node, node->name);

		//node->matrix = a3mat4_identity;
		
		if (a3JSONFindObjValue(json_node, "matrix", &json_val)) {
			node->usesMatrix = 1;
			for (a3ui8 i = 0; i < 16; i++) {
				(&node->matrix.m00)[i] = (float)json_val.values[i].num;
			}

		}

		node->translation = a3vec3_zero;
		node->rotation = a3vec4_w;
		node->scale = a3vec3_one;

		
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


	/*
		LOAD SKINS
	*/

	a3_JSONValue json_skins;
	if (a3JSONFindObjValue(value, "skins", &json_skins) && json_skins.type == JSONTYPE_ARRAY) {
		
		glft.skins_count = json_skins.length;
		a3AllocArray(glft.skins, glft.skins_count, a3_GLFT_Skin);

		for (a3ui32 i = 0; i < glft.skins_count; i++) {

			a3_JSONValue json_val;
			a3_GLFT_Skin* skin = glft.skins + i;


			a3GLTF_ReadName(json_skins.values[i], skin->name);

			if (a3JSONFindObjValue(json_skins.values[i], "joints", &json_val)) {
				skin->joints_count = json_val.length;
				a3AllocArray(skin->joints, skin->joints_count, a3ui32);

				for (a3ui32 j = 0; j < skin->joints_count; j++) {
					skin->joints[j] = (a3ui32)json_val.values[i].num;
				}

			}
			
			if (a3JSONFindObjValue(json_skins.values[i], "skeleton", &json_val)) {
				skin->skeleton = (a3ui32)json_val.num;
			}

		}


	}
	else {
		printf("couldnt find skins\n");
	}


























	// start anim loading
	/*
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
				byteLength = (a3ui32)json_uri.num;
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

	char* tempBuffer;
	a3ReadFileIntoMemory(buffer_path, &tempBuffer);
	*/

	*out_glft = glft;
	return -1;

}





































