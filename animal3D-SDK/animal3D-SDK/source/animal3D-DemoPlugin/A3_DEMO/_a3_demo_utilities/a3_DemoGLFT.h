
#ifndef __ANIMAL3D_DEMOGLFT_H
#define __ANIMAL3D_DEMOGLFT_H

#include "animal3D/animal3D.h"
#include "animal3D-A3DM/animal3D-A3DM.h"

typedef struct a3_GLFTFile a3_GLFTFile;
typedef struct a3_GLFT_Skin a3_GLFT_Skin;
typedef struct a3_GLFT_Node a3_GLFT_Node;

#define GLFT_NAME_MAX_SIZE 100

struct a3_GLFTFile {
	a3_GLFT_Skin* skins;
	a3ui32 skins_count;


	a3_GLFT_Node* nodes;
	a3ui32 nodes_count;
};

/*
struct a3_GLFT_Accessor {
	a3ui32 bufferView;
	a3ui32 byteOffset;
	a3ui32 componentType; // for now just floats
	a3ui32 count;
	a3f32 min[16];
	a3f32 max[16];
	a3ui8 type_size;
};
*/


struct a3_GLFT_Skin {
	
	a3ui32 joints_count;
	a3ui32* joint_indices;
	a3ui32 invBindMat_index;
	a3ui32 skeleton_index;
	char name[GLFT_NAME_MAX_SIZE];
};

struct a3_GLFT_Node {
	char name[GLFT_NAME_MAX_SIZE];
	a3ui32* children;
	a3ui32 children_count;

	a3ui8 usesMatrix;
	union {
		a3mat4 matrix;
		struct {
			a3vec4 rotation;
			a3vec3 translation;
			a3vec3 scale;
		};
	};
};

a3i32 a3GLFTRead(a3_GLFTFile* out_glft, const char* directory, const char* filename);

#endif