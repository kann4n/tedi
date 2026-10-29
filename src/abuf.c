#include "abuf.h"
#include <stdlib.h>
#include <string.h>

void abufAppend(struct abuf *ab, const char *s, int len)
{
	char *new = realloc(ab->buf, ab->len + len);
	if (new == NULL)
		return;
	memcpy(&new[ab->len], s, len);
	ab->buf = new;
	ab->len += len;
}

void abufFree(struct abuf *ab) { free(ab->buf); }

void abufSetColor(struct abuf *ab, char *fg, char *bg)
{
	if (!fg || !bg)
		return;
	abufAppend(ab, fg, strlen(fg));
	abufAppend(ab, bg, strlen(bg));
}
