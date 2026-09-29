#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLEX    256
#define MAX_WIDTH   120
#define MAX_HEIGHT  40
#define TERM_WIDTH  80
#define TERM_HEIGHT 24

// must be ASCII
#define RECT_CORNER     '+'
#define RECT_HORIZONTAL '-'
#define RECT_VERTICAL   '|'
#define RECT_FILL       ' '

/* Struct Definitions */

typedef struct {
  int x, y, w, h;
} Rect;

typedef Rect Area;

typedef enum {
  horizontal,
  vertical,
} Direction;

typedef int Flex;

typedef struct {
  int n;
  Direction dir;
  Flex flex[MAX_FLEX];
} FlexRule;

typedef struct {
  int w, h;
  char data[MAX_HEIGHT][MAX_WIDTH];
} Buffer;

/* utils */

int isCorner(int x, int y, int sx, int sy, int ex, int ey) {
  char isTopLeft     = (x == sx && y == sy);
  char isTopRight    = (x == ex && y == sy);
  char isBottomLeft  = (x == sx && y == ey);
  char isBottomRight = (x == ex && y == ey);
  return isTopLeft || isTopRight || isBottomLeft || isBottomRight;
}

int isHoriEdge(int y, int sy, int ey) {
  return y == sy || y == ey;
}

int isVertEdge(int x, int sx, int ex) {
  return x == sx || x == ex;
}

/*
 * Creates a set of rectangles based on a flex rule on given area.
 * Returns:
 *  - pointer to first rectangle
 *  - NULL if the rule is invalid or something else goes wrong
 */
Rect *createFlexRects(Area area, FlexRule *rule) {
  if (rule->n == 0 || rule->n > MAX_FLEX)
    return NULL;

  float sum = 0;

  for (int i = 0; i < rule->n; i++)
    sum += rule->flex[i];

  Rect *rects = malloc(rule->n * sizeof(Rect));

  float base = (rule->dir == horizontal) ? area.w : area.h;
  float step = (rule->dir == horizontal) ? area.x : area.y;
  for (int i = 0; i < rule->n; i++) {
    float flex = (rule->flex[i] / sum) * base;
    if (rule->dir == horizontal) {
      rects[i].x = step;
      rects[i].y = area.y;
      rects[i].w = flex;
      rects[i].h = area.h;
    } else {
      rects[i].x = area.x;
      rects[i].y = step;
      rects[i].w = area.w;
      rects[i].h = flex;
    }
    step += flex;
  }

  return rects;
}

/* Printing Functions */

/* Prints given flex rule to the console.*/
void printFlexRule(FlexRule *rule) {
  printf("FlexRule: n=%d, dir=%d, flex=[", rule->n, rule->dir);
  for (int i = 0; i < rule->n; i++)
    printf(" %d%c", rule->flex[i], (i < rule->n - 1) ? ',' : ' ');
  printf("]\n");
}

/* Prints given rectangle to the console.*/
void printRect(Rect rect) {
  printf("Rect: x=%d, y=%d, w=%d, h=%d\n", rect.x, rect.y, rect.w, rect.h);
}

/* Prints given set of rectangles to the console.*/
void printRects(Rect *rects, int n) {
  for (int i = 0; i < n; i++) {
    printf("[%d] ", i);
    printRect(rects[i]);
  }
}

/* Display functions */

void putChar(Buffer *buffer, char c, int x, int y) {
  if (x < 0 || x >= MAX_WIDTH || y < 0 || y >= MAX_HEIGHT)
    return;
  buffer->data[y][x] = c;
}

void clearBuffer(Buffer *buffer) {
  for (int y = 0; y < MAX_HEIGHT; y++) {
    for (int x = 0; x < MAX_WIDTH; x++) {
      buffer->data[y][x] = ' ';
    }
  }
}

void putRect(Buffer *buffer, Rect rect) {
  int sx = rect.x, sy = rect.y, ex = rect.x + rect.w, ey = rect.y + rect.h;
  for (int y = sy; y <= ey; y++) {
    for (int x = sx; x <= ex; x++) {
      if (isCorner(x, y, sx, sy, ex, ey))
        putChar(buffer, RECT_CORNER, x, y);
      else if (isHoriEdge(y, sy, ey))
        putChar(buffer, RECT_HORIZONTAL, x, y);
      else if (isVertEdge(x, sx, ex))
        putChar(buffer, RECT_VERTICAL, x, y);
      else
        putChar(buffer, RECT_FILL, x, y);
    }
  }
}

void putText(Buffer *buffer, char *text, int x, int y, int maxWidth) {
  int len = strlen(text);
  for (int i = 0; i < len; i++) {
    putChar(buffer, text[i], x + i, y);
    if (i >= maxWidth)
      break;
  }
}

void putRects(Buffer *buffer, Rect *rects, int n) {
  for (int i = 0; i < n; i++) {
    putRect(buffer, rects[i]);
  }
}

void renderBuffer(Buffer *buffer) {
  for (int y = 0; y <= buffer->h; y++) {
    for (int x = 0; x <= buffer->w; x++) {
      printf("%c", buffer->data[y][x]);
    }
    printf("\n");
  }
}

// Impl of a this lib.
int main() {
  Area area = {0, 0, 80, 24};

  FlexRule bodyRule = {
    3, vertical, {1, 8, 1}};                   // space for header, Main, footer
  FlexRule mainRule = {2, horizontal, {2, 8}}; // space for sidebar, content

  Rect *bodyRects, *mainRects;

  bodyRects = createFlexRects(area, &bodyRule);
  printf("Header, Main, Footer\n");
  printRects(bodyRects, bodyRule.n);

  printf("\n");

  mainRects = createFlexRects(bodyRects[1], &mainRule);
  printf("Sidebar, Content\n");
  printRects(mainRects, mainRule.n);

  printf("\nWith rules\n");
  printFlexRule(&bodyRule);
  printFlexRule(&mainRule);

  Buffer buffer = {TERM_WIDTH, TERM_HEIGHT};
  clearBuffer(&buffer);
  putRects(&buffer, bodyRects, bodyRule.n);
  putRects(&buffer, mainRects, mainRule.n);

  putText(&buffer, "Header", 20, 1, 80);
  putText(&buffer, "Sidebar", 4, 12, 8);
  putText(&buffer, "Content", 30, 12, 80);
  putText(&buffer, "Footer", 20, 22, 80);

  renderBuffer(&buffer);

  free(bodyRects);
  free(mainRects);
  return 0;
}
