#ifndef ABUF_H
#define ABUF_H

struct abuf {
	char *buf;
	int len;
};

void abufAppend(struct abuf *ab, const char *s, int len);

void abufFree(struct abuf *ab);

void abufSetColor(struct abuf *ab, char *fg, char *bg);

#endif
