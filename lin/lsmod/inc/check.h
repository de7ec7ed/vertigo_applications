#ifndef __CHECK_H__
#define __CHECK_H__

#include <defines.h>

#define CHECK_SUCCESS(a, b) \
	if(a != SUCCESS) { printf("%s", b); return FAILURE;}

#define CHECK_NOT_NULL(a, b) \
		if(a == NULL) { printf("%s", b); return FAILURE;}

#endif //__CHECK_H__
