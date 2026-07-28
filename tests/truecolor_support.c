#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int supports_truecolor() {
  const char *colorterm = getenv("COLORTERM");
  if (colorterm != NULL) {
    // most terminals support truecolor sets COLORTERM to "truecolor" or "24bit"
    if (strcmp(colorterm, "truecolor") == 0 ||
        strcmp(colorterm, "24bit") == 0) {
      return 1;
    }
  }
  // extra check for 24-bit color support using TERM environment variable
  const char *term = getenv("TERM");
  if (term != NULL) {
    if (strstr(term, "24bit") != NULL || strstr(term, "truecolor") != NULL ||
        strstr(term, "direct") != NULL) {
      return 1;
    }
  }
  return 0;
}

void setColorFG(char *fg, int r, int g, int b) {
  snprintf(fg, 32, "\x1b[38;2;%d;%d;%dm", r, g, b);
}

void setColorBG(char *bg, int r, int g, int b) {
  snprintf(bg, 32, "\x1b[48;2;%d;%d;%dm", r, g, b);
}

int main() {
  char fg[24], bg[24], reset[] = "\x1b[0m";
  setColorFG(fg, 255, 0, 0);
  setColorBG(bg, 0, 0, 0);
  if (supports_truecolor()) {
    printf("truecolor supported\n");
    printf("%s%s", fg, bg);
    printf("This is a RGB colored output\n");
    printf("%s", reset);
  } else {
    printf("truecolor not supported\n");
  }
  return 0;
}
