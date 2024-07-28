#include "qck.h"

qck_expr *qck_expr_call_new(const char *func_name, u8 arg_count,
                            qck_expr **args) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_call;

  expr->call = malloc(sizeof(qck_expr_call));
  expr->call->func = NULL;
  expr->call->func_name = func_name;
  expr->call->arg_count = arg_count;
  expr->call->args = args;

  return expr;
}

char *qck_expr_call_fmt(qck_expr_call *call) {
  char *s = malloc(100);
  s[0] = 0;
  strcat(s, call->func_name);
  strcat(s, "(");
  for (int i = 0; i < call->arg_count; i++) {
    if (i > 0) {
      strcat(s, ", ");
    }

    char *var = qck_expr_fmt(call->args[i]);
    strcat(s, var);
    free(var);
  }
  strcat(s, ")");
  return s;
}

qck_expr *qck_expr_property_new(const char *var_name,
                                const char *property_name) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_property;

  expr->property = malloc(sizeof(qck_expr_property));
  expr->property->var_name = var_name;
  expr->property->property_name = property_name;

  return expr;
}

char *qck_expr_property_fmt(qck_expr_property *property) {
  char *s = malloc(100);
  sprintf(s, "%s.%s", property->var_name, property->property_name);
  return s;
}

qck_expr *qck_expr_binary_new(qck_expr *left, u8 operator, qck_expr * right) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_binary;

  expr->binary = malloc(sizeof(qck_expr_binary));
  expr->binary->left = left;
  expr->binary->operator= operator;
  expr->binary->right = right;
  expr->binary->func = NULL;

  return expr;
}

char *qck_expr_binary_fmt(qck_expr_binary *binary) {
  char *left = qck_expr_fmt(binary->left);
  char *right = qck_expr_fmt(binary->right);

  char *s = malloc(100);
  sprintf(s, "(%s %s %s)", left, qck_token_str(binary->operator), right);
  free(left);
  free(right);
  return s;
}

qck_expr *qck_expr_unary_new(u8 operator, qck_expr * value) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_unary;

  expr->binary = malloc(sizeof(qck_expr_binary));
  expr->binary->left = NULL;
  expr->binary->operator= operator;
  expr->binary->right = value;

  return expr;
}

char *qck_expr_unary_fmt(qck_expr_binary *binary) {
  char *value = qck_expr_fmt(binary->right);
  char *s = malloc(100);
  sprintf(s, "%s %s", qck_token_str(binary->operator), value);
  free(value);
  return s;
}

qck_expr *qck_expr_rtn_new(qck_expr *e) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_rtn;

  expr->rtn = malloc(sizeof(qck_expr_rtn));
  expr->rtn->expr = e;

  return expr;
}

char *qck_expr_rtn_fmt(qck_expr_rtn *rtn) {
  char *expr = qck_expr_fmt(rtn->expr);

  char *s = malloc(100);
  sprintf(s, "return %s", expr);
  free(expr);
  return s;
}

qck_expr *qck_expr_instantiate_new(const char *type, size_t var_count,
                                   qck_var **vars) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_instantiate;

  expr->instantiate = malloc(sizeof(qck_expr_instantiate));
  expr->instantiate->type = type;
  expr->instantiate->var_count = var_count;
  expr->instantiate->vars = vars;

  return expr;
}

char *qck_expr_instantiate_fmt(qck_expr_instantiate *instantiate) {
  char *s = malloc(100);
  s[0] = 0;
  strcat(s, instantiate->type);
  strcat(s, "{");
  for (int i = 0; i < instantiate->var_count; i++) {
    if (i > 0) {
      strcat(s, ", ");
    }
    strcat(s, instantiate->vars[i]->name);
    strcat(s, ": ");
    char *value = qck_expr_fmt(instantiate->vars[i]->value);
    strcat(s, value);
    free(value);
  }
  strcat(s, "}");
  return s;
}

qck_expr *qck_expr_f64_new(double value) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_f64;
  expr->f64 = value;
  return expr;
}

qck_expr *qck_expr_bool_new(qck_bool value) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_bool;
  expr->bool = value;
  return expr;
}

qck_expr *qck_expr_string_new(char *value) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_string;
  expr->string = value;
  return expr;
}

qck_expr *qck_expr_assign_new(const char *var_name, qck_expr *value) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_assign;

  expr->assign = malloc(sizeof(qck_expr_assign));
  expr->assign->var_name = var_name;
  expr->assign->value = value;

  return expr;
}

char *qck_expr_assign_fmt(qck_expr_assign *assign) {
  char *s = malloc(100);
  s[0] = 0;
  strcat(s, "var ");
  strcat(s, assign->var_name);
  strcat(s, " = ");
  char *value = qck_expr_fmt(assign->value);
  strcat(s, value);
  free(value);
  return s;
}

qck_expr *qck_expr_object_new(int property_count) {
  qck_expr *expr = malloc(sizeof(qck_expr));
  expr->type = qck_expr_type_object;

  expr->object = malloc(sizeof(qck_expr_object));
  expr->object->len = property_count;
  expr->object->values = calloc(sizeof(qck_expr *), property_count);

  return expr;
}

char *qck_expr_fmt(qck_expr *expr) {
  switch (expr->type) {
  case qck_expr_type_call:
    return qck_expr_call_fmt(expr->call);
  case qck_expr_type_property:
    return qck_expr_property_fmt(expr->property);
  case qck_expr_type_binary:
    return qck_expr_binary_fmt(expr->binary);
  case qck_expr_type_unary: {
    char *s = malloc(100);
    sprintf(s, "%s %s", qck_token_str(expr->binary->operator),
            qck_expr_fmt(expr->binary->right));
    return s;
  }
  case qck_expr_type_rtn:
    return qck_expr_rtn_fmt(expr->rtn);
  case qck_expr_type_instantiate:
    return qck_expr_instantiate_fmt(expr->instantiate);
  case qck_expr_type_assign:
    return qck_expr_assign_fmt(expr->assign);
  case qck_expr_type_f64: {
    char *s = malloc(100);
    sprintf(s, "%g", expr->f64);
    return s;
  }
  case qck_expr_type_string: {
    char *s = malloc(100);
    sprintf(s, "\"%s\"", expr->string);
    return s;
  }
  case qck_expr_type_bool: {
    char *s = malloc(100);
    if (expr->bool) {
      strcpy(s, "true");
    } else {
      strcpy(s, "false");
    }
    return s;
  }
  case qck_expr_type_object: {
    char *s = malloc(100);
    s[0] = 0;
    strcat(s, "[");

    for (int i = 0; i < expr->object->len; i++) {
      if (i > 0) {
        strcat(s, ", ");
      }
      char *value = qck_expr_fmt(expr->object->values[i]);
      strcat(s, value);
      free(value);
    }

    strcat(s, "]");
    return s;
  }
  }
  return NULL;
}

qck_expr **qck_exprs(int count, ...) {
  va_list ap;
  qck_expr **exprs = calloc(sizeof(qck_expr *), count);
  va_start(ap, count);
  for (int i = 0; i < count; i++) {
    exprs[i] = va_arg(ap, qck_expr *);
  }
  va_end(ap);
  return exprs;
}
