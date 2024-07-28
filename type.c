#include "qck.h"

qck_type *qck_type_new_f64() {
  qck_type *type = malloc(sizeof(qck_type));
  type->kind = qck_type_kind_f64;
  return type;
}

qck_type *qck_type_new_func(const char *name, qck_type *returns) {
  qck_type *type = malloc(sizeof(qck_type));
  type->kind = qck_type_kind_func;
  type->name = name;
  type->result = returns;
  return type;
}

qck_type *qck_type_new_name(const char *name) {
  qck_type *type = malloc(sizeof(qck_type));
  type->kind = qck_type_kind_name;
  type->name = name;
  return type;
}

qck_type *qck_type_new_interface(const char *name, size_t var_count,
                                 qck_var **vars) {
  qck_type *type = malloc(sizeof(qck_type));
  type->kind = qck_type_kind_interface;
  type->name = name;
  type->var_count = var_count;
  type->vars = vars;
  return type;
}

char *qck_type_fmt(qck_type *type) {
  char *s = malloc(100);
  s[0] = 0;

  switch (type->kind) {
  case qck_type_kind_f64: {
    sprintf(s, "f64");
    break;
  }

  case qck_type_kind_name: {
    strcat(s, type->name);
    break;
  }

  case qck_type_kind_func: {
    char *returns = qck_type_fmt(type->result);
    sprintf(s, "%s() %s", type->name, returns);
    free(returns);
    break;
  }

  case qck_type_kind_interface: {
    strcat(s, "type ");
    strcat(s, type->name);
    strcat(s, " {\n");
    for (size_t i = 0; i < type->var_count; i++) {
      strcat(s, "\t");
      char *func_fmt = qck_var_fmt(type->vars[i]);
      strcat(s, func_fmt);
      free(func_fmt);
      strcat(s, "\n");
    }
    strcat(s, "}\n\n");
    break;
  }
  }

  return s;
}

int qck_type_find_index(qck_type *type, const char *name) {
  for (int i = 0; i < type->var_count; i++) {
    if (strcmp(name, type->vars[i]->name) == 0) {
      return i;
    }
  }

  return -1;
}
