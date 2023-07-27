/*
Thanks to https://youtu.be/kPFYfTvMRs8
*/

#ifndef LITTLE_XML
#define LITTLE_XML

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
definitions
*/

struct _XMLNode {
  char *tag;
  char *inner_text;
  struct _XMLNode *parent;
};

typedef struct _XMLNode XMLNode;

XMLNode *XMLNode_new(XMLNode *parent);
void XMLNode_free(XMLNode *node);

struct _XMLDocument {
  XMLNode *root;
};

typedef struct _XMLDocument XMLDocument;

bool XMLDocument_load(XMLDocument *doc, const char *path);
void XMLDocument_free(XMLDocument *doc);

/*
implementation
*/

XMLNode *XMLNode_new(XMLNode *parent) {
  XMLNode *node = (XMLNode *)malloc(sizeof(XMLNode));
  node->parent = parent;
  node->tag = NULL;
  node->inner_text = NULL;
  return node;
}
void XMLNode_free(XMLNode *node) {
  if (node->tag) free(node->tag);
  if (node->inner_text) free(node->inner_text);
  // free(node); !! Danger: we need to free its children before freeing it
  // if its children exist, then they will be pointing to something that does
  // not exist seems like a dangling pointer danger
  // now i realize what manual
  // memory handling in c means
}

bool XMLDocument_load(XMLDocument *doc, const char *path) {
  FILE *file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "'%s' File load failed!\n", path);
    return false;
  }

  // move position indicator of file stream to the end of file
  fseek(file, 0, SEEK_END);
  int size = ftell(file);    // size in no of bytes
  fseek(file, 0, SEEK_SET);  // rewind
  printf("The size of file '%s' in bytes is %d\n", path, size);

  char *buf = (char *)malloc(size * sizeof(char) + 1);
  fread(buf, size, 1, file);
  fclose(file);
  buf[size] = '\0';

  return true;
}
void XMLDocument_free(XMLDocument *doc) {
  // free the heap allocation
  XMLNode_free(doc->root);
}

#endif
