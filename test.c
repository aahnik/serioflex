#include <stdio.h>

#include "lxml.h"

int main() {
  XMLDocument doc;

  XMLDocument_load(&doc, "test.xml");
  printf("Tag: %s Inner Text: %s\n", doc.root->tag, doc.root->inner_text);
  XMLDocument_free(&doc);
}