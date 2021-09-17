
#include "../a3_DemoUtils.h"
#include <animal3D/a3/a3macros.h>
#include <stdio.h>
#include <stdlib.h>


void __a3AssertThrow(const char* msg, const char* filename, unsigned int line) {
	printf("ASSERT FAILED: %s at %s:%u\n", msg, filename, line);
}

void* __a3SafeMalloc(size_t count, size_t type_size, const char* var_name, const char* filename, unsigned int line) {
	// make sure type_size * count won't overflow
	if ((count && type_size && SIZE_MAX / count < type_size)) {
		exit(1);
	}

	void* ptr = malloc(count * type_size);
	if (ptr == NULL) {
		printf("ERROR: failed to malloc %s \n\t at %s:%u", var_name, filename, line);
		exit(1);
	}

	return ptr;
}


void* __a3SafeRealloc(void* array_ptr, size_t count, size_t type_size, const char* var_name, const char* filename, unsigned int line) {

	// make sure type_size * count won't overflow
	if ((count && type_size && SIZE_MAX / count < type_size)) {
		exit(1);
	}

	void* ptr = realloc(array_ptr, count * type_size);
	if (ptr == NULL) {
		printf("ERROR: failed to realloc %s \n\t at %s:%u", var_name, filename, line);
		exit(1);
	}

	return ptr;
}

a3i32 a3SplitString(char* str, char split_char, char** strs_out, a3i32 max_strs, a3boolean skip_empty) {
	a3i32 count = 0;
	char* start = str;
	for (char* itr = str; true; itr++) {
		if (*itr != split_char && *itr != 0) continue; // keep walking through column
		if (skip_empty && itr == start) { *itr = 0; start++; continue; } // skip repeat skip characters
		if (count >= max_strs) return -1; // read to many columns

		
		strs_out[count] = start;
		start = itr + 1;
		count++;
		if (*itr == 0) break;
		*itr = 0;
	}
	return count;
}


a3i32 a3ReadLinesFromFile(const char* path, char** lines_out, a3i32 max_lines) {
	FILE* fp = fopen(path, "r");
	if (fp == NULL) {
		printf("Couldn't open %s\n", path);
		return -1;
	}

	// get the length of the file
	fseek(fp, 0L, SEEK_END);
	a3ui64 size = ftell(fp);
	fseek(fp, 0, SEEK_SET);


	// read the whole file into memory
	char* buf;
	a3AllocArray(buf, size+1, char);
	fread(buf, 1, size, fp);
	buf[size] = 0;
	fclose(fp);

	return a3SplitString(buf, '\n', lines_out, max_lines, a3false);
}