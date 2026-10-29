#include "../include/tedi.h"

char *editorPrompt(char *prompt, void (*callback)(char *, int))
{
	size_t bufsize = 128;
	char *buf = malloc(bufsize);

	size_t buflen = 0;
	buf[0] = '\0';

	while (1) {
		editorSetStatusMsg(prompt, buf);
		editorRefreshScreen();

		int c = editorReadkey();
		if (c == DELETE_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
			if (buflen != 0)
				buf[--buflen] = '\0';
		} else if (c == '\x1b') {
			editorSetStatusMsg("");
			if (callback)
				callback(buf, c);
			free(buf);
			return NULL;
		} else if (c == '\r' && buflen != 0) {
			editorSetStatusMsg("");
			if (callback)
				callback(buf, c);
			return buf;
		} else if (!iscntrl(c) && c < 128) {
			if (buflen == bufsize - 1) {
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}

		if (callback)
			callback(buf, c);
	}
}

void editorMoveCursor(int key)
{
	erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
	switch (key) {
	case ARROW_LEFT:
		if (E.cx != 0) {
			E.cx--;
		} else if (E.cy > 0) {
			E.cy--;
			E.cx = E.row[E.cy].size;
		}
		break;
	case ARROW_UP:
		if (E.cy != 0)
			E.cy--;
		break;
	case ARROW_RIGHT:
		if (row && E.cx < row->size) {
			E.cx++;
		} else if (row && E.cx == row->size) {
			E.cx = 0;
			E.cy++;
		}
		break;
	case ARROW_DOWN:
		if (E.cy < E.numrows)
			E.cy++;
		break;
	}
	row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
	int rowlen = row ? row->size : 0;
	if (E.cx > rowlen)
		E.cx = rowlen;
}

void editorProcessKeypress()
{
	int c = editorReadkey();
	switch (c) {
	case '\r':  // Enter Key
		editorInsertNewline();
		break;

	case CTRL_KEY('q'):
		if (E.dirty) {
			editorSetStatusMsg("Warning!!! File has unsaved changes. (press '!' to override)");
			editorRefreshScreen();
			if (editorReadkey() != '!') {
				editorSetStatusMsg("");
				return;
			}
		}
		write(STDOUT_FILENO, ALT_SCR_BUF_OFF, C_LEN(ALT_SCR_BUF_OFF));
		exit(0);
		break;

	case CTRL_KEY('s'):
		editorSave();
		break;

	case CTRL_KEY('f'):
		editorFind();
		break;

	case CTRL_KEY('t'):
		editorLoadColorScheme("test.colorscheme");
		break;

	case HOME_KEY:
		E.cx = 0;
		break;
	case END_KEY:
		if (E.cy < E.numrows)
			E.cx = E.row[E.cy].size;
		break;

	case BACKSPACE:      // in old days backspace(BS) has CC-8 (Control Code)
	case CTRL_KEY('h'):  // this also returns CC-8. (Ctrl-A = 1 & Ctrl-Z = 26)
	case DELETE_KEY:     // But nowdays BS is 127 and Delete is <esc>[3~
		if (c == DELETE_KEY)
			editorMoveCursor(ARROW_RIGHT);
		editorDelChar();
		break;

	case PAGE_UP:
	case PAGE_DOWN: {
		if (c == PAGE_UP) {
			E.cy = E.rowoff;
		} else if (c == PAGE_DOWN) {
			E.cy = E.rowoff + E.screenrows - 1;
			if (E.cy > E.numrows)
				E.cy = E.numrows;
		}
		int times = E.screenrows;
		while (times--) {
			editorMoveCursor((c == PAGE_UP) ? ARROW_UP : ARROW_DOWN);
		}
		break;
	}

	case ARROW_LEFT:
	case ARROW_UP:
	case ARROW_RIGHT:
	case ARROW_DOWN:
		editorMoveCursor(c);
		break;

	case CTRL_KEY('l'):
	case '\x1b':  // esc key
		break;

	default:
		editorInsertChar(c);
		break;
	}
}
