#include "utils.h"
#include <string.h>

int getfilename(char *dst, const char *src, int maxlen) {
	char *slash = strrchr(src, '/');
	const char *name_start = slash ? slash : src;
	char *name_end = strrchr(src, '.');
	
	int name_len = name_end ? (int)(name_end - name_start) : strlen(name_start);
	if (name_len >= maxlen) name_len = maxlen - 1;

	strncpy(dst, name_start, name_len);
	dst[name_len] = '\0';

	return name_len;
}
