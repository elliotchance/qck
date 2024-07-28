#include "qck.h"

void qck_func_print(int arg_count, qck_expr **args) {
  if (args[0]->type == qck_expr_type_f64) {
    printf("%g\n", args[0]->f64);
  }
  if (args[0]->type == qck_expr_type_string) {
    printf("%s\n", args[0]->string);
  }
  if (args[0]->type == qck_expr_type_bool) {
    if (args[0]->bool) {
      printf("%s\n", "true");
    } else {
      printf("%s\n", "false");
    }
  }
}

struct qck_expr *qck_binary_string_plus(struct qck_expr *left,
                                        struct qck_expr *right) {
  int len = strlen(left->string) + strlen(right->string);
  char *s = malloc(len + 1);
  s[0] = 0;
  strcat(s, left->string);
  strcat(s, right->string);
  s[len] = 0;
  return qck_expr_string_new(s);
}

struct qck_expr *qck_binary_bool_and(struct qck_expr *left,
                                     struct qck_expr *right) {
  return qck_expr_bool_new(left->bool && right->bool);
}

struct qck_expr *qck_binary_bool_or(struct qck_expr *left,
                                    struct qck_expr *right) {
  return qck_expr_bool_new(left->bool || right->bool);
}

struct qck_expr *qck_binary_bool_not(struct qck_expr *left,
                                     struct qck_expr *right) {
  return qck_expr_bool_new(!right->bool);
}

int qck_link_func_find_var(qck_func *func, const char *var_name) {
  for (int i = 0; i < func->arg_count; i++) {
    if (strcmp(var_name, func->args[i]->name) == 0) {
      return i;
    }
  }

  for (int i = 0; i < func->expr_count; i++) {
    if (func->exprs[i]->type == qck_expr_type_assign &&
        strcmp(var_name, func->exprs[i]->assign->var_name) == 0) {
      return func->arg_count + i;
    }
  }

  return -1;
}

void qck_link_expr(qck_program *program, qck_func *func, qck_expr *expr) {
  char *s = qck_expr_fmt(expr);

  switch (expr->type) {
  case qck_expr_type_rtn: {
    qck_link_expr(program, func, expr->rtn->expr);
    break;
  }
  case qck_expr_type_unary: {
    qck_link_expr(program, func, expr->binary->right);
    if (expr->binary->operator== qck_token_not) {
      expr->binary->func = qck_binary_bool_not;
    }
    break;
  }
  case qck_expr_type_binary: {
    qck_link_expr(program, func, expr->binary->left);
    qck_link_expr(program, func, expr->binary->right);
    if (expr->binary->operator== qck_token_and) {
      expr->binary->func = qck_binary_bool_and;
    } else if (expr->binary->operator== qck_token_or) {
      expr->binary->func = qck_binary_bool_or;
    } else {
      expr->binary->func = qck_binary_string_plus;
    }
    break;
  }
  case qck_expr_type_property: {
    int var_idx = qck_link_func_find_var(func, expr->property->var_name);

    qck_type *type =
        qck_program_find_type(program, func->args[var_idx]->type->name);
    expr->property->index =
        qck_type_find_index(type, expr->property->property_name);
    break;
  }
  case qck_expr_type_assign: {
    expr->assign->index = qck_link_func_find_var(func, expr->assign->var_name);
    qck_link_expr(program, func, expr->assign->value);
    break;
  }
  case qck_expr_type_call: {
    for (int i = 0; i < expr->call->arg_count; ++i) {
      qck_link_expr(program, func, expr->call->args[i]);
    }
    expr->call->func = qck_func_print;
  }
  }
}

void qck_link_func(qck_program *program, qck_func *func) {
  for (int i = 0; i < func->expr_count; i++) {
    qck_link_expr(program, func, func->exprs[i]);
  }
}

void qck_link_program(qck_program *program) {
  for (int i = 0; i < program->func_count; i++) {
    qck_link_func(program, program->funcs[i]);
  }
}

qck_expr *qck_run_expr(qck_expr *expr, qck_expr **stack) {
  char *s = qck_expr_fmt(expr);

  switch (expr->type) {
  case qck_expr_type_assign: {
    stack[0] = qck_run_expr(expr->assign->value, stack);
    break;
  }
  case qck_expr_type_unary: {
    qck_expr *right = qck_run_expr(expr->binary->right, stack);
    return expr->binary->func(NULL, right);
  }
  case qck_expr_type_binary: {
    qck_expr *left = qck_run_expr(expr->binary->left, stack);
    qck_expr *right = qck_run_expr(expr->binary->right, stack);
    return expr->binary->func(left, right);
  }
  case qck_expr_type_instantiate: {
    qck_expr *obj = qck_expr_object_new(2);
    obj->object->values[0] = qck_expr_f64_new(5);
    obj->object->values[1] = qck_expr_f64_new(3);
    return obj;
  }
  case qck_expr_type_call: {
    expr->call->func(1, qck_exprs(1, qck_run_expr(expr->call->args[0], stack)));
    break;
  }
  case qck_expr_type_string: {
    return qck_expr_string_new(expr->string);
  }
  case qck_expr_type_bool: {
    return qck_expr_bool_new(expr->bool);
  }
  }

  return NULL;
}

void qck_run_func(qck_func *func) {
  qck_expr **stack = calloc(sizeof(qck_expr *), 100);
  for (int i = 0; i < func->expr_count; i++) {
    qck_run_expr(func->exprs[i], stack);
  }

  // printf("MEMORY:\n");
  // for (int i = 0; i < 1; i++) {
  //   char *mem = qck_expr_fmt(stack[i]);
  //   printf("%d %s\n", i, mem);
  // }
}

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    FILE *f = fopen(argv[i], "r");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;

    qck_program *program = qck_program_parse(string);

    printf("\n%s", qck_program_fmt(program));

    qck_link_program(program);

    qck_run_func(program->funcs[0]);

    free(string);
  }

  return 0;
}
