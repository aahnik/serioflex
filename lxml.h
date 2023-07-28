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
  free(node);
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

  doc->root = XMLNode_new(NULL);

  XMLNode *current_node = NULL;

  char lex[256];
  int lexi = 0;
  int i = 0;
  while (buf[i] != '\0') {
    if (buf[i] == '<') {
      // inner text
      lex[lexi] = '\0';
      if (lexi > 0) {
        if (!current_node) {
          fprintf(stderr, "Text outside of document\n");
          return false;
        }
        current_node->inner_text = strdup(lex);
        printf("inner text: %s\n", current_node->inner_text);
      }

      // end of node
      if (buf[i + 1] == '/') {
        lexi = 0;
        i += 2;

        while (buf[i] != '>') lex[lexi++] = buf[i++];
        lex[lexi] = '\0';
        if (strcmp(current_node->tag, lex)) {
          fprintf(stderr, "Mismatched tags %s != %s\n", current_node->tag, lex);
          return false;
        }
        current_node = current_node->parent;
        i++;
        continue;
      }

      // set current node
      if (!current_node)
        current_node = doc->root;
      else
        current_node = XMLNode_new(current_node);

      // get current node tag
      // lexi = 0;
      i++;
      while (buf[i] != '>') lex[lexi++] = buf[i++];
      lex[lexi] = '\0';

      // strcpy(current_node->tag, lex); // this is a terrible mistake
      current_node->tag = strdup(lex);

      // reset lex
      lexi = 0;
      lex[lexi] = '\0';
      i++;
      continue;
    } else {
      lex[lexi++] = buf[i++];
      continue;
    }
    i++;

    // if (buf[i] == '>') {
    // }
    // lex[lexi] = buf[i];
    // i++;
    // lexi++;
  }

  return true;
}
void XMLDocument_free(XMLDocument *doc) {
  // free the heap allocation
  XMLNode_free(doc->root);
}

#endif
