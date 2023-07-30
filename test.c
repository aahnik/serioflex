#include <assert.h>
#include <stdio.h>

#include "lxml.h"

void show_node_attrs(XMLNode* node) {
  if (node->attrs.size) printf("Attributes for node[%s]:\n", node->tag);

  for (int i = 0; i < node->attrs.size; i++) {
    XMLAttribute attr = node->attrs.data[i];
    printf("%s : %s\t", attr.key, attr.value);
  }
  if (node->attrs.size) printf("\n");
  if (node->attrs.size == 0)
    printf("No attributes on this node[%s]\n", node->tag);
}

void show_node(XMLNode* node) {
  printf("Tag: %s \tInner Text: %s++\n", node->tag, node->inner_text);
}

void show_children(XMLNode* node) {
  printf("Children of %s\n", node->tag);
  if (!node->tag) printf("This node is probably the document root\n");
  if (node->children.size == 0) {
    printf("This node has no children\n");
    return;
  }
  for (int i = 0; i < node->children.size; i++) {
    printf("%s\n", node->children.data[i]->tag);
  }
}

void test_ends_with() {
  assert(ends_with("hello", "hello"));
  assert(ends_with("hello", "llo"));
  assert(ends_with("hello", "lo"));

  assert(!ends_with("hello", "cllo"));
  assert(!ends_with("hello", "llj"));

  printf("✅ ends_with passed\n\n");
}
int main() {
  test_ends_with();

  XMLDocument doc;
  if (XMLDocument_load(&doc, "test.xml")) {
    printf("\n\n*********test.c********\n\n");
    printf("Document root\n");
    show_node(doc.root);
    show_children(doc.root);

    printf("\n===========\n");
    XMLNode* mynode = XMLNode_child(doc.root, 0);

    show_node(mynode);
    show_node_attrs(mynode);
    show_children(mynode);

    XMLNode* more_node = XMLNode_child(XMLNode_child(mynode, 0), 0);
    show_node(more_node);

    XMLNode* another_node = XMLNode_child(mynode, 1);
    show_node(another_node);
    show_node_attrs(another_node);
    XMLDocument_free(&doc);
  }
}
