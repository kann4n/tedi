#include "../include/tedi.h"

struct editorConfig E;

int main(int argc, char *argv[])
{
	enableRawMode();
	initEditor();
	if (argc >= 2)
		editorOpen(argv[1]);
	editorSetStatusMsg(" HELP: <C-s> = Save | <C-q> = quit | <C-f> = find ");

	while (1) {
		editorRefreshScreen();
		editorProcessKeypress();
	}
	return 0;
}
