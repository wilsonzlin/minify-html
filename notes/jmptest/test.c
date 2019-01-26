#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>

typedef void destructor_t(void*);

typedef struct runtime_s {
  char* error;
  void** instances;
  destructor_t** destructors;
} *runtime_t;

static runtime_t runtime;

void runtime_init(void) {
  runtime = calloc(1, sizeof(struct runtime_s));
  runtime->instances = calloc(10, sizeof(void*));
  runtime->destructors = calloc(10, sizeof(destructor_t));
}

typedef struct buffer_s {
  size_t length;
  size_t size;
  char* data;
} *buffer_t;

buffer_t buffer_create(void) {
  buffer_t buffer = calloc(1, sizeof(struct buffer_s));
  char* data = calloc(10, sizeof(char));
  buffer->size = 10;
  buffer->data = data;
  return buffer;
}

void buffer_destroy(buffer_t buffer) {
  free(buffer->data);
  free(buffer);
  printf("Buffer destroyed\n");
}

static jmp_buf env;

void failing_function(void) {
  printf("Entered failing_function\n");
  longjmp(env, 1);
}

int main(void) {
  runtime_init();

  if (setjmp(env) == 0) {
    buffer_t buffer = buffer_create();
    runtime->instances[0] = buffer;
    runtime->destructors[0] = (destructor_t *) &buffer_destroy;
    memcpy(buffer->data, "Hello", 5);
    failing_function();
    printf("End of setjmp == 0\n");
  } else {
    // Error handling code
    printf("%p: %s\n", &runtime->instances[0], ((buffer_t) runtime->instances[0])->data);
    runtime->destructors[0](runtime->instances[0]);
    printf("End of error handling code\n");
  }

  return EXIT_SUCCESS;
}
