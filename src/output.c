#include "tedi.h"

void editorScroll()
{
	E.rx = 0;
	if (E.cy < E.numrows) {
		E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);
	}

	if (E.cy < E.rowoff) {
		E.rowoff = E.cy;
	}
	if (E.cy >= E.rowoff + E.screenrows) {
		E.rowoff = E.cy - E.screenrows + 1;
	}
	if (E.rx < E.coloff) {
		E.coloff = E.rx;
	}
	if (E.rx >= E.coloff + E.screencols) {
		E.coloff = E.rx - E.screencols + 1;
	}
}

// TODO: redo the work
void editorDrawRows(struct abuf *ab)
{
	char default_color[32];
	int dlen = snprintf(default_color, sizeof(default_color), "\x1b[%d;%dm", E.theme.default_fg, E.theme.default_bg);
	abufAppend(ab, default_color, dlen);

	for (int y = 0; y < E.screenrows; y++) {
		int filerow = y + E.rowoff;
		if (filerow >= E.numrows) {
			if (y == E.screenrows / 3 && E.numrows == 0) {
				char welcome[80];
				int welcomelen = snprintf(welcome, sizeof(welcome), "Tedi - Text Editor version %s", TEDI_VERSION);
				if (welcomelen > E.screencols)
					welcomelen = E.screencols;
				int padding = (E.screencols - welcomelen) / 2;
				if (padding) {
					abufAppend(ab, "~", 1);
					padding--;
				}
				while (padding--)
					abufAppend(ab, " ", 1);
				abufAppend(ab, welcome, welcomelen);
			} else {
				abufAppend(ab, "~", 1);
			}
		} else {
			int len = E.row[filerow].rsize - E.coloff;
			if (len < 0)
				len = 0;
			if (len > E.screencols)
				len = E.screencols;
			char *c = &E.row[filerow].render[E.coloff];
			unsigned char *hl = &E.row[filerow].hl[E.coloff];
			int current_color = -1;

			for (int j = 0; j < len; j++) {
				if (iscntrl(c[j])) {
					char sym = SYM_CAPPED_CTRL(c[j]);
					abufAppend(ab, "\x1b[7m", 4);  // set inverse colorcode
					abufAppend(ab, &sym, 1);
					abufAppend(ab, "\x1b[m", 3);  // reset colorcode

					// Must re-apply after a full reset \x1b[m
					abufAppend(ab, default_color, dlen);

					if (current_color != -1) {
						char buf[24];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
						abufAppend(ab, buf, clen);
					}
				} else if (hl[j] == HL_NORMAL) {
					if (current_color != -1) {
						abufAppend(ab, default_color, dlen);
						current_color = -1;
					}
					abufAppend(ab, &c[j], 1);
				} else {
					int color = editorSyntaxToColor(hl[j]);
					if (color != current_color) {
						current_color = color;
						char buf[24];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
						abufAppend(ab, buf, clen);
					}
					abufAppend(ab, &c[j], 1);
				}
			}
			abufAppend(ab, default_color, dlen);
		}

		abufAppend(ab, "\x1b[K", 3);
		abufAppend(ab, "\r\n", 2);
	}
}

void editorDrawStatusBar(struct abuf *ab)
{
	char fg[] = "\x1b[38;2;255;255;255m";
	char bg[] = "\x1b[48;2;10;10;10m";
	abufSetColor(ab, fg, bg);

	char status[80], rstatus[80];
	char *status_msg = E.filename ? E.filename : "[No Name]";
	char *file_status = E.dirty ? "(modified)" : "";
	char *filetype = E.syntax ? E.syntax->filetype : "idk";
	int len = snprintf(status, sizeof(status), " %.24s %.16s", status_msg, file_status);
	int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d ", filetype, E.cy + 1, E.numrows);
	if (len > E.screencols)
		len = E.screencols;
	abufAppend(ab, status, len);

	while (len < E.screencols) {
		if (E.screencols - len == rlen) {
			abufAppend(ab, rstatus, rlen);
			break;
		} else {
			abufAppend(ab, " ", 1);
			len++;
		}
	}
	abufAppend(ab, "\x1b[m", 3);
	abufAppend(ab, "\r\n", 2);
}

void editorDrawMsgBar(struct abuf *ab)
{
	abufAppend(ab, "\x1b[K", 3);
	char fg[] = "\x1b[38;2;255;255;255m";
	char bg[] = "\x1b[48;2;15;15;15m";
	abufSetColor(ab, fg, bg);

	int msglen = strlen(E.statusmsg);
	if (msglen > E.screencols)
		msglen = E.screencols;

	if (msglen && time(NULL) - E.statusmsg_time < MSG_TTL)
		abufAppend(ab, E.statusmsg, msglen);
	else
		msglen = 0;

	while (msglen < E.screencols) {
		abufAppend(ab, " ", 1);
		msglen++;
	}
	abufAppend(ab, "\x1b[m", 3);
}

void editorRefreshScreen()
{
	editorScroll();
	struct abuf ab = {NULL, 0};

	abufAppend(&ab, "\x1b[?25l", 6);  // hide cursor
	abufAppend(&ab, "\x1b[H", 3);     // set cursor top-left

	editorDrawRows(&ab);
	editorDrawStatusBar(&ab);
	editorDrawMsgBar(&ab);

	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1, (E.rx - E.coloff) + 1);
	abufAppend(&ab, buf, strlen(buf));

	abufAppend(&ab, "\x1b[?25h", 6);  // set cursor

	write(STDOUT_FILENO, ab.buf, ab.len);
	abufFree(&ab);
}

void editorSetStatusMsg(const char *fmt, ...)
{
	va_list ap;  // argument pointer
	va_start(ap, fmt);
	vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
	va_end(ap);
	E.statusmsg_time = time(NULL);
}
