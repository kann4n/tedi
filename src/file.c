#include "../include/tedi.h"

char *editorRowsToStr(int *buflen)
{
	int totlen = 0;
	for (int j = 0; j < E.numrows; j++)
		totlen += E.row[j].size + 1;  // +1 for '\n'
	*buflen = totlen;

	char *buf = malloc(totlen);  // starting point
	char *p = buf;               // will be pointing to end when func finshes.
	                             // so buf must not change. and only return that.
	for (int j = 0; j < E.numrows; j++) {
		memcpy(p, E.row[j].chars, E.row[j].size);
		p += E.row[j].size;
		*p = '\n';
		p++;
	}
	return buf;
}

void editorOpen(char *filename)
{
	free(E.filename);
	E.filename = strdup(filename);

	editorSelectSyntaxHighlight();

	FILE *fp = fopen(filename, "r");
	if (!fp)
		die("fopen");
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	// getline returns -1 when End of File
	while ((linelen = getline(&line, &linecap, fp)) != -1) {
		while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
			linelen--;
		editorInsertRow(E.numrows, line, linelen);
	}
	free(line);
	fclose(fp);
	E.dirty = 0;
}

void editorSave()
{
	if (E.filename == NULL) {
		E.filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
		if (E.filename == NULL) {
			editorSetStatusMsg("escaped");
			return;
		}
		editorSelectSyntaxHighlight();
	}
	int len;
	char *buf = editorRowsToStr(&len);
	// O_RDWR = Open with Read and Write mode,
	// O_CREAT = Create if doesn't exists.
	// 0644 = -rw-r--r--
	int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
	if (fd != -1) {
		// sets filesize to len bytes
		if (ftruncate(fd, len) != -1) {
			if (write(fd, buf, len) == len) {
				close(fd);
				free(buf);
				editorSetStatusMsg("%d bytes written to disk", len);
				E.dirty = 0;
				return;
			}
		}
		close(fd);
	}
	free(buf);
	editorSetStatusMsg("Can't save! I/O error: %s", strerror(errno));
}
