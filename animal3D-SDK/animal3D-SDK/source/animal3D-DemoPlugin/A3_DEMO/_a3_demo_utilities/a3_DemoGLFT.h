
#ifndef __ANIMAL3D_DEMOGLFT_H
#define __ANIMAL3D_DEMOGLFT_H

#include "animal3D/animal3D.h"

typedef struct a3_GLFTFile a3_GLFTFile;



struct a3_GLFTFile {

};


a3i32 a3GLFTRead(a3_GLFTFile* out_glft, const char* directory, const char* filename);

#endif