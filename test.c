#include <stdio.h>

#include "lxml.h"

int main() {
  XMLDocument doc;

  XMLDocument_load(&doc, "test.xml");
  printf("\n\n*******\n%s||\n*******\n\n\n", doc.source);
  XMLDocument_free(&doc);
}