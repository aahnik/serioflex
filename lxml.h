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

struct _XMLAttribute {
  char *key;
  char *value;
};

typedef struct _XMLAttribute XMLAttribute;

void XMLAttribute_free(XMLAttribute *attr);

struct _XMLAttributeList {
  int heap_size;  // how many elements we can put in
  int size;       // how many elements we have
  XMLAttribute *data;
};

typedef struct _XMLAttributeList XMLAttributeList;

void XMLAttributeList_init(XMLAttributeList *list);
void XMLAttributeList_add(XMLAttributeList *list, XMLAttribute *attr);

struct _XMLNodeList {
  int heap_size;
  int size;
  struct _XMLNode **data;
};

typedef struct _XMLNodeList XMLNodeList;

void XMLNodeList_init(XMLNodeList *list);

struct _XMLNode {
  char *tag;
  char *inner_text;
  struct _XMLNode *parent;
  XMLAttributeList attrs;
  XMLNodeList children;
};

typedef struct _XMLNode XMLNode;

XMLNode *XMLNode_new(XMLNode *parent);
void XMLNode_free(XMLNode *node);

void XMLNodeList_add(XMLNodeList *list, XMLNode *node);
XMLNode *XMLNode_child(XMLNode *parent, int index);

struct _XMLDocument {
  XMLNode *root;
};

typedef struct _XMLDocument XMLDocument;

bool XMLDocument_load(XMLDocument *doc, const char *path);
void XMLDocument_free(XMLDocument *doc);

/*
implementation
*/

void XMLAttribute_free(XMLAttribute *attr) {
  if (!attr) return;
  free(attr->key);
  free(attr->value);
}

void XMLAttributeList_init(XMLAttributeList *list) {
  list->heap_size = 1;
  list->size = 0;
  list->data = (XMLAttribute *)malloc(sizeof(XMLAttribute) * list->heap_size);
}
void XMLAttributeList_add(XMLAttributeList *list, XMLAttribute *attr) {
  if (list->size >= list->heap_size) {
    list->heap_size = list->heap_size * 2;
    list->data = (XMLAttribute *)realloc(
        list->data, sizeof(XMLAttribute) * list->heap_size);
  }
  list->data[list->size++] = *attr;
}

XMLNode *XMLNode_new(XMLNode *parent) {
  XMLNode *node = (XMLNode *)malloc(sizeof(XMLNode));
  node->parent = parent;
  node->tag = NULL;
  node->inner_text = NULL;
  XMLAttributeList_init(&node->attrs);
  XMLNodeList_init(&node->children);

  if (parent) XMLNodeList_add(&parent->children, node);

  return node;
}
void XMLNode_free(XMLNode *node) {
  if (node->tag) free(node->tag);
  if (node->inner_text) free(node->inner_text);
  for (int i = 0; i < node->attrs.size; i++) {
    XMLAttribute_free(&node->attrs.data[i]);
  }
  free(node);
  // free(node); !! Danger: we need to free its children before freeing it
  // if its children exist, then they will be pointing to something that does
  // not exist seems like a dangling pointer danger
  // now i realize what manual
  // memory handling in c means
}

XMLNode *XMLNode_child(XMLNode *parent, int index) {
  return parent->children.data[index];
}

void XMLNodeList_init(XMLNodeList *list) {
  list->heap_size = 1;
  list->size = 0;
  list->data = (XMLNode **)malloc(sizeof(XMLNode *) * list->heap_size);
}
void XMLNodeList_add(XMLNodeList *list, XMLNode *node) {
  if (list->size >= list->heap_size) {
    list->heap_size = list->heap_size * 2;
    list->data =
        (XMLNode **)realloc(list->data, sizeof(XMLNode *) * list->heap_size);
  }
  list->data[list->size++] = node;
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

  XMLNode *current_node = doc->root;

  char lex[256];
  int lexi = 0;
  int i = 0;
  while (buf[i] != '\0') {
    if (buf[i] == '<') {
      // inner text
      lex[lexi] = '\0';
      if (lexi > 0) {
        if (!current_node->parent) {
          fprintf(stderr, ":::ERROR::: Text outside of any node\n");
          lexi = 0;
          lex[lexi] = '\0';
          // return false;
        } else {
          if (!current_node->inner_text)
            current_node->inner_text = strdup(lex);
          else
            strcat(current_node->inner_text, (lex));
          printf(":::DEBUG::: inner text: %s for tag %s\n",
                 current_node->inner_text, current_node->tag);
          lexi = 0;
        }
      }

      // end of node
      if (buf[i + 1] == '/') {
        lexi = 0;
        i += 2;

        while (buf[i] != '>') lex[lexi++] = buf[i++];
        lex[lexi] = '\0';
        lexi = 0;

        if (!current_node) {
          fprintf(stderr, "Already at the root\n");
          return false;
        }

        if (strcmp(current_node->tag, lex)) {
          fprintf(stderr, "Mismatched tags %s != %s\n", current_node->tag, lex);
          return false;
        }
        current_node = current_node->parent;
        i++;
        continue;
      }

      // set current node

      current_node = XMLNode_new(current_node);

      // start tag
      lexi = 0;
      i++;
      XMLAttribute curr_attr = {0, 0};
      while (buf[i] != '>') {
        lex[lexi++] = buf[i++];

        // tag name
        if (buf[i] == ' ' && !current_node->tag) {
          lex[lexi] = '\0';
          current_node->tag = strdup(lex);

          // reset lex
          lexi = 0;
          lex[lexi] = '\0';
          i++;
          continue;
        }

        // usually ignores spaces
        if (lex[lexi - 1] == ' ') {
          lexi--;
          continue;
        }

        // attribute key
        if (buf[i] == '=') {
          lex[lexi] = '\0';
          lexi = 0;
          curr_attr.key = strdup(lex);
          continue;
        }

        // attribute value
        if (buf[i] == '"') {
          if (!curr_attr.key) {
            fprintf(stderr, "Value has no key\n");
            return false;
          }
          lexi = 0;
          i++;
          while (buf[i] != '"') lex[lexi++] = buf[i++];

          lex[lexi] = '\0';
          curr_attr.value = strdup(lex);
          XMLAttributeList_add(&current_node->attrs, &curr_attr);
          curr_attr.key = NULL, curr_attr.value = NULL;  // reset current attr
          lexi = 0;
          i++;
          continue;
        }
      }

      lex[lexi] = '\0';
      // case when no attributes
      if (!current_node->tag) {
        current_node->tag = strdup(lex);
      }
      // reset lex
      lexi = 0;
      lex[lexi] = '\0';
      i++;
      continue;
    } else {
      lex[lexi++] = buf[i++];
      continue;
    }
    // i++;

    // if (buf[i] == '>') {
    // }
    // lex[lexi] = buf[i];
    // i++;
    // lexi++;
  }
  lex[lexi] = '\0';
  if (lexi > 0) {
    printf(":::ERROR::: Text found at end (outside any node)\n++%s++\n", lex);
  }
  return true;
}
void XMLDocument_free(XMLDocument *doc) {
  // free the heap allocation
  XMLNode_free(doc->root);
}

#endif
