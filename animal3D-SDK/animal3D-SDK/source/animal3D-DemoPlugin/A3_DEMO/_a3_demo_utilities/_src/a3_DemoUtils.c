
#include "../a3_DemoUtil.h"
#include <stdio.h>


void __a3AssertThrow(const char* msg, const char* filename, unsigned int line) {
	printf("ASSERT FAILED: %s at %s:%u\n", msg, filename, line);
}
