#include <stdio.h>

#include "lxml.h"

int main() {
  XMLDocument doc;

  XMLDocument_load(&doc, "test.xml");
  XMLNode* node = doc.root;
  printf("Tag: %s Inner Text: %s\n", node->tag, node->inner_text);

  printf("Attributes:\n");

  for (int i = 0; i < node->attrs.size; i++) {
    XMLAttribute attr = node->attrs.data[i];

    printf("%s : %s\t", attr.key, attr.value);
  }
  printf("\n");
  if (node->attrs.size == 0) printf("No attributes on this node\n");
  XMLDocument_free(&doc);
}