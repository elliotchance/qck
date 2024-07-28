#include "qck.h"

qck_program *qck_program_new() {
  qck_program *program = malloc(sizeof(qck_program));
  program->type_count = 0;
  program->types = calloc(sizeof(qck_type *), 100);
  program->func_count = 0;
  program->funcs = calloc(sizeof(qck_func *), 100);
  return program;
}

void qck_program_push_type(qck_program *program, qck_type *type) {
  program->types[program->type_count] = type;
  ++program->type_count;
}

void qck_program_push_func(qck_program *program, qck_func *func) {
  program->funcs[program->func_count] = func;
  ++program->func_count;
}

qck_type *qck_program_find_type(qck_program *program, const char *type_name) {
  for (int i = 0; i < program->type_count; i++) {
    if (strcmp(type_name, program->types[i]->name) == 0) {
      return program->types[i];
    }
  }

  return NULL;
}

char *qck_program_fmt(qck_program *program) {
  char *s = malloc(10000);
  s[0] = 0;
  for (int i = 0; i < program->type_count; i++) {
    char *type = qck_type_fmt(program->types[i]);
    strcat(s, type);
    free(type);
  }
  for (int i = 0; i < program->func_count; i++) {
    char *func = qck_func_fmt(program->funcs[i]);
    strcat(s, func);
    free(func);
  }
  return s;
}
