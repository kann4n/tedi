#include <stdio.h>
#include <stdlib.h>

#define MAX_FLEX    256
#define TERM_WIDTH  80
#define TERM_HEIGHT 24

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
    if (step < base)
      step += flex;
  }

  return rects;
}
void printFlexRule(FlexRule *rule) {
  printf("FlexRule: n=%d, dir=%d, flex=[", rule->n, rule->dir);
  for (int i = 0; i < rule->n; i++)
    printf(" %d%c", rule->flex[i], (i < rule->n - 1) ? ',' : ' ');
  printf("]\n");
}
void printRect(Rect rect) {
  printf("Rect: x=%d, y=%d, w=%d, h=%d\n", rect.x, rect.y, rect.w, rect.h);
}
void printRects(Rect *rects, int n) {
  for (int i = 0; i < n; i++) {
    printf("[%d] ", i);
    printRect(rects[i]);
  }
}
// Impl of a this lib.
int main() {
  Area area = {0, 0, TERM_WIDTH, TERM_HEIGHT};

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

  free(bodyRects);
  free(mainRects);
  return 0;
}
