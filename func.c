#include "qck.h"

qck_func *qck_func_new(const char *name, size_t arg_count, qck_var **args,
                       qck_type *returns, size_t expr_count, qck_expr **exprs) {
  qck_func *func = malloc(sizeof(qck_func));
  func->name = name;
  func->arg_count = arg_count;
  func->args = args;
  func->returns = returns;
  func->expr_count = expr_count;
  func->exprs = exprs;
  return func;
}

char *qck_func_fmt(qck_func *func) {
  char *s = malloc(1000);
  s[0] = 0;
  strcat(s, "func ");
  strcat(s, func->name);
  strcat(s, "(");
  for (int i = 0; i < func->arg_count; i++) {
    if (i > 0) {
      strcat(s, ", ");
    }

    char *var = qck_var_fmt(func->args[i]);
    strcat(s, var);
    free(var);
  }
  strcat(s, ")");

  if (func->returns != NULL) {
    strcat(s, " ");
    char *returns = qck_type_fmt(func->returns);
    strcat(s, returns);
    free(returns);
  }

  strcat(s, " {\n");
  for (int i = 0; i < func->expr_count; i++) {
    strcat(s, "\t");
    char *expr = qck_expr_fmt(func->exprs[i]);
    strcat(s, expr);
    free(expr);
    strcat(s, "\n");
  }
  strcat(s, "}\n\n");
  return s;
}