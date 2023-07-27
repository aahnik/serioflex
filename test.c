#include <stdio.h>

#include "lxml.h"

int main() {
  XMLDocument doc;

  XMLDocument_load(&doc, "test.xml");
  printf("%s\n", doc.root->tag);
  XMLDocument_free(&doc);
}