#include "../a3_DemoGLFT.h"
#include "../a3_DemoJSON.h"
#include <string.h>
#include <stdlib.h>

a3i32 a3GLFTRead(a3_GLFTFile* out_glft, const char* dirname, const char* filename) {

	a3ui32 dirname_size = strlen(dirname);
	a3ui32 filename_size = strlen(filename);

	char* full_path = (char*)malloc(dirname_size + filename_size + 1);
	memcpy(full_path, dirname, dirname_size);
	memcpy(full_path + dirname_size, filename, filename_size);
	full_path[dirname_size + filename_size] = 0;

	a3_JSONValue value = a3readJSONFromFile(full_path);


}





































