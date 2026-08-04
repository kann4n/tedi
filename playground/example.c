/*
 * Theme Test File
 * Open this in tedi to see the syntax highlighting
 */

#include <stdio.h> // This is a single-line comment

// A simple struct definition to test keywords
struct Point {
  int x;
  int y;
  float weight;
};

int main() {
  int count = 100;
  double pi = 3.14159;

  // Testing strings
  char letter    = 'f';
  char *greeting = "Hello, kannan!";

  // Testing control flow keywords
  if (count > 0) {
    for (int i = 0; i < 5; i++) {
      continue;
    }
  } else {
    return -1;
  }

  switch (letter) {
  case 'G': break;
    default:
        printf("wtf\n");
        break;
  }

  return 0;
}
