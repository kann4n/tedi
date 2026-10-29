#ifndef TEDI_H
#define TEDI_H

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "abuf.h"
#include "utils.h"
#include "ansi.h"

/* Definitions and constants */

#define TEDI_VERSION "0.0.16"
#define TAB_STOP 8
#define S_LINES 2
#define MSG_TTL 3
#define DEBUG 1

#define CTRL_KEY(k) ((k) & 0x1f)
#define SYM_CAPPED_CTRL(k) (((k) < 32) ? ((k) + 64) : '?')
#define HL_HL_NUMBERS (1 << 0)
#define HL_HL_STRINGS (1 << 1)

/* Enums */
enum editorKey {
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_UP,
	ARROW_RIGHT,
	ARROW_DOWN,
	PAGE_UP,
	PAGE_DOWN,
	HOME_KEY,
	END_KEY,
	DELETE_KEY,
};
enum editorHighlight {
	HL_NORMAL = 0,
	HL_COMMENT,
	HL_MLCOMMENT,
	HL_KEYWORD_1,
	HL_KEYWORD_2,
	HL_KEYWORD_3,
	HL_STRING,
	HL_NUMBER,
	HL_MATCH,
	HL_ENTRIES
};
enum COLOR {
	FG_BLACK = 30,
	FG_RED,
	FG_GREEN,
	FG_YELLOW,
	FG_BLUE,
	FG_MAGENTA,
	FG_CYAN,
	FG_WHITE,
	BG_BLACK = 40,
	BG_RED,
	BG_GREEN,
	BG_YELLOW,
	BG_BLUE,
	BG_MAGENTA,
	BG_CYAN,
	BG_WHITE,
};

/* Structs */

typedef struct erow {
	int idx;
	int size;
	int rsize;
	char *chars;
	char *render;
	unsigned char *hl;
	int hl_open_comment;
} erow;

struct editorColorScheme {
	char name[16];
	int comment;
	int mlcomment;
	int keyword1;
	int keyword2;
	int keyword3;
	int string;
	int number;
	int match;
	int default_fg;
	int default_bg;
};
struct editorSyntax {
	char *filetype;
	char **filematch;
	char **keywords;
	char *singleline_comment_start;
	char *multiline_comment_start;
	char *multiline_comment_end;
	int flags;
};
struct editorConfig {
	int cy, cx, rx;
	int rowoff, coloff;
	int screenrows, screencols;
	int numrows;
	erow *row;
	int dirty;
	char *filename;
	char statusmsg[80];
	time_t statusmsg_time;
	struct editorSyntax *syntax;
	struct termios orig_termios;
	struct editorColorScheme theme;
};

extern struct editorConfig E;
extern struct editorSyntax HLDB[];

/* Function declarations */

// terminal
void die(const char *msg);
void disableRawMode();
void enableRawMode();
int editorReadkey();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);

// syntax
int isSeparator(int c);
void editorUpdateSyntax(erow *row);
int editorSyntaxToColor(int hl);
void editorSelectSyntaxHighlight();
void editorLoadColorScheme(const char *path);

// operations
void editorInsertChar(int c);
void editorInsertNewline();
void editorDelChar();

// row
int editorRowCxToRx(erow *row, int cx);
int editorRowRxToCx(erow *row, int rx);
void editorUpdateRow(erow *row);
void editorInsertRow(int at, char *s, size_t len);
void editorFreeRow(erow *row);
void editorDelRow(int at);
void editorRowInsertChar(erow *row, int at, int c);
void editorRowAppendString(erow *row, char *s, size_t len);
void editorRowDelChar(erow *row, int at);

// file
char *editorRowsToStr(int *buflen);
void editorOpen(char *filename);
void editorSave();

// find
void editorFindCallback(char *query, int key);
void editorFind();

// input
char *editorPrompt(char *prompt, void (*callback)(char *, int));
void editorMoveCursor(int key);
void editorProcessKeypress();

// output
void editorScroll();
void editorDrawRows(struct abuf *ab);
void editorDrawStatusBar(struct abuf *ab);
void editorDrawMsgBar(struct abuf *ab);
void editorRefreshScreen();
void editorSetStatusMsg(const char *fmt, ...);

// init
void initTheme();
void initEditor();

#endif
