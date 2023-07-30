#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 4029 * 32
#define MAX_KEY 256
#define MAX_VALUE 1024
#define COLON ":"

struct _kv {
  char key[MAX_KEY];
  union value {
    char *string;
    struct _kv *kv_ptr;
  };
  bool is_string;
};

typedef struct _kv kv;

void parseYaml(const char *filename) {
  FILE *file = fopen(file, "r");

  if (!file) {
    printf("There is some error in opening the file %s", filename);
    return;
  }

  char buffer[MAX_BUFFER];
  char *colon;

  while (fgets(buffer, MAX_BUFFER, file)) {
    colon = strrchr(buffer, COLON);
    kv item;
    strncpy(item.key, buffer, colon - buffer);
    item.key[colon - buffer] = '\0';

    // strncpy(item.value, colon, )
  }

  fclose(file);
}

int main() {
  return 0;
}