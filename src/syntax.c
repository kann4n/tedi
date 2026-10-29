#include "tedi.h"

char *C_HL_extensions[] = {".c", ".cpp", ".h", NULL};
char *Python_HL_extensions[] = {".py", NULL};

char *C_HL_keywords[] = {"switch", "if",      "while",   "for",    "break",     "continue", "return", "else",
                         "struct", "union",   "typedef", "static", "enum",      "class",    "case",   "int|",
                         "long|",  "double|", "float|",  "char|",  "unsigned|", "signed|",  "void|",  NULL};

char *Python_HL_keywords[] = {"False",   "None",     "True",     "and",      "as",     "assert", "async",  "await",
                              "break",   "class",    "continue", "def",      "del",    "elif",   "else",   "except",
                              "finally", "for",      "from",     "global",   "if",     "import", "in",     "is",
                              "lambda",  "nonlocal", "not",      "or",       "pass",   "raise",  "return", "try",
                              "while",   "with",     "yield",    "int|",     "float|", "str|",   "bool|",  "list|",
                              "dict|",   "set|",     "tuple|",   "complex|", "bytes|", NULL};

struct editorSyntax HLDB[] = {
    {"c", C_HL_extensions, C_HL_keywords, "//", "/*", "*/", HL_HL_NUMBERS | HL_HL_STRINGS},
    {"python", Python_HL_extensions, Python_HL_keywords, "#", "", "", HL_HL_NUMBERS | HL_HL_STRINGS},
};

#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

int is_separator(int c) { return isspace(c) || c == '\0' || strchr(",.()+-=~%<>[]{};", c) != NULL; }

void editorUpdateSyntax(erow *row)
{
	row->hl = realloc(row->hl, row->rsize);
	memset(row->hl, HL_NORMAL, row->rsize);

	if (E.syntax == NULL)
		return;

	char **keywords = E.syntax->keywords;

	char *scs = E.syntax->singleline_comment_start;
	char *mcs = E.syntax->multiline_comment_start;
	char *mce = E.syntax->multiline_comment_end;

	int scs_len = scs ? strlen(scs) : 0;
	int mcs_len = mcs ? strlen(mcs) : 0;
	int mce_len = mce ? strlen(mce) : 0;

	int prev_sep = 1;
	int in_string = 0;
	int in_comment = (row->idx > 0 && E.row[row->idx - 1].hl_open_comment);
	int i = 0;
	while (i < row->rsize) {
		char c = row->render[i];
		unsigned char prev_hl = (i > 0) ? row->hl[i - 1] : HL_NORMAL;

		if (scs_len && !in_string && !in_comment) {
			if (!strncmp(&row->render[i], scs, scs_len)) {
				memset(&row->hl[i], HL_COMMENT, row->rsize - i);
				break;
			}
		}

		if (mcs_len && mce_len && !in_string) {
			if (in_comment) {
				row->hl[i] = HL_MLCOMMENT;
				if (!strncmp(&row->render[i], mce, mce_len)) {
					memset(&row->hl[i], HL_MLCOMMENT, mce_len);
					i += mce_len;
					in_comment = 0;
					prev_sep = 1;
					continue;
				} else {
					i++;
					continue;
				}
			} else if (!strncmp(&row->render[i], mcs, mcs_len)) {
				memset(&row->hl[i], HL_MLCOMMENT, mcs_len);
				i += mcs_len;
				in_comment = 1;
				continue;
			}
		}
		if (E.syntax->flags & HL_HL_STRINGS) {
			if (in_string) {
				row->hl[i] = HL_STRING;
				if (c == '\\' && i + 1 < row->rsize) {
					row->hl[i + 1] = HL_STRING;
					i += 2;
					continue;
				}
				if (c == in_string)
					in_string = 0;
				i++;
				prev_sep = 1;
				continue;
			} else {
				if (c == '"' || c == '\'') {
					in_string = c;
					row->hl[i] = HL_STRING;
					i++;
					continue;
				}
			}
		}

		if (E.syntax->flags & HL_HL_NUMBERS) {
			if ((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)) {
				row->hl[i] = HL_NUMBER;
				i++;
				prev_sep = 0;
				continue;
			}
		}

		if (prev_sep) {
			int j;
			for (j = 0; keywords[j]; j++) {
				int klen = strlen(keywords[j]);
				int kw2 = keywords[j][klen - 1] == '|';  // -1 cause end is '\0'
				if (kw2)
					klen--;  // remove secondary keywords identifier '|'
				if (!strncmp(&row->render[i], keywords[j], klen) && is_separator(row->render[i + klen])) {
					memset(&row->hl[i], kw2 ? HL_KEYWORD_2 : HL_KEYWORD_1, klen);
					i += klen;
					break;
				}
			}
			if (keywords[j] != NULL) {
				prev_sep = 0;
				continue;
			}
		}
		prev_sep = is_separator(c);
		i++;
	}
	int changed = (row->hl_open_comment != in_comment);
	row->hl_open_comment = in_comment;
	if (changed && row->idx + 1 < E.numrows)
		editorUpdateSyntax(&E.row[row->idx + 1]);
}

int editorSyntaxToColor(int hl)
{
	switch (hl) {
	case HL_COMMENT:
		return E.theme.comment;
	case HL_MLCOMMENT:
		return E.theme.mlcomment;
	case HL_KEYWORD_1:
		return E.theme.keyword1;
	case HL_KEYWORD_2:
		return E.theme.keyword2;
	case HL_KEYWORD_3:
		return E.theme.keyword3;
	case HL_STRING:
		return E.theme.string;
	case HL_NUMBER:
		return E.theme.number;
	case HL_MATCH:
		return E.theme.match;
	default:
		return E.theme.default_fg;
	}
}

void editorSelectSyntaxHighlight()
{
	E.syntax = NULL;
	if (E.filename == NULL)
		return;

	char *ext = strrchr(E.filename, '.');

	for (unsigned int j = 0; j < HLDB_ENTRIES; j++) {
		struct editorSyntax *s = &HLDB[j];
		unsigned int i = 0;
		while (s->filematch[i]) {
			int is_ext = (s->filematch[i][0] == '.');
			int match =
			    ((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(E.filename, s->filematch[i])));

			if (match) {
				E.syntax = s;

				// update syntax color
				for (int frow = 0; frow < E.numrows; frow++) {
					editorUpdateSyntax(&E.row[frow]);
				}
				return;
			}
			i++;
		}
	}
}

void editorLoadColorScheme(const char *path)
{
	FILE *fp = fopen(path, "r");
	if (!fp) {
		editorSetStatusMsg("failed to load theme");
		return;
	}

	char *line = NULL;
	ssize_t linelen;
	size_t linecap = 0;

	getfilename(E.theme.name, path, sizeof(E.theme.name));
	// TODO: separate this to another function
	while ((linelen = getline(&line, &linecap, fp)) != -1) {
		char key[32];
		int val;
		if (sscanf(line, "%[^=]=%d", key, &val) == 2) {
			if (strcmp(key, "comment") == 0)
				E.theme.comment = val;
			else if (strcmp(key, "default_bg") == 0)
				E.theme.default_bg = val;
			else if (strcmp(key, "default_fg") == 0)
				E.theme.default_fg = val;
			else if (strcmp(key, "keyword1") == 0)
				E.theme.keyword1 = val;
			else if (strcmp(key, "keyword2") == 0)
				E.theme.keyword2 = val;
			else if (strcmp(key, "keyword3") == 0)
				E.theme.keyword3 = val;
			else if (strcmp(key, "match") == 0)
				E.theme.match = val;
			else if (strcmp(key, "mlcomment") == 0)
				E.theme.mlcomment = val;
			else if (strcmp(key, "number") == 0)
				E.theme.number = val;
			else if (strcmp(key, "string") == 0)
				E.theme.string = val;
		}
	}

	free(line);
	fclose(fp);
	editorSetStatusMsg("Colorscheme changed to %s", E.theme.name);
}
