#include "qck.h"

qck_var *qck_var_new(const char *name, qck_type *type, struct qck_expr *value) {
  qck_var *var = malloc(sizeof(qck_var));
  var->name = name;
  var->type = type;
  var->value = value;
  return var;
}

char *qck_var_fmt(qck_var *var) {
  char *s = malloc(1000);
  char *type = qck_type_fmt(var->type);
  sprintf(s, "%s %s", var->name, type);
  free(type);
  // TODO: missing value
  return s;
}
