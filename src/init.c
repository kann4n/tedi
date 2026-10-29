#include "tedi.h"

void initTheme()
{
	// TODO: add rgb support
	E.theme.comment = FG_CYAN;
	E.theme.mlcomment = FG_CYAN;
	E.theme.keyword1 = FG_YELLOW;
	E.theme.keyword2 = FG_MAGENTA;
	E.theme.keyword3 = FG_MAGENTA;
	E.theme.string = FG_GREEN;
	E.theme.number = FG_RED;
	E.theme.match = FG_BLUE;
	E.theme.default_fg = FG_WHITE;
	E.theme.default_bg = BG_BLACK;
	strcpy(E.theme.name, "default");
}

void initEditor()
{
	E.cx = E.cy = E.rx = 0;
	E.rowoff = E.coloff = 0;
	E.numrows = 0;
	E.row = NULL;
	E.dirty = 0;
	E.filename = NULL;
	E.statusmsg[0] = '\0';
	E.statusmsg_time = 0;
	E.syntax = NULL;
	initTheme();

	if (getWindowSize(&E.screenrows, &E.screencols) == -1)
		die("getWindowSize");
	E.screenrows -= S_LINES;  // space for statusline and other stuffss
}
