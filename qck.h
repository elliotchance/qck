#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char u8;

typedef unsigned char qck_bool;

#define qck_expr_type_call 1
#define qck_expr_type_property 2
#define qck_expr_type_binary 3
#define qck_expr_type_rtn 4
#define qck_expr_type_instantiate 5
#define qck_expr_type_f64 6
#define qck_expr_type_assign 7
#define qck_expr_type_object 8
#define qck_expr_type_string 9
#define qck_expr_type_bool 10
#define qck_expr_type_unary 11

struct qck_expr;

typedef struct {
  const char *func_name;
  u8 arg_count;
  struct qck_expr **args;
  void (*func)(int, struct qck_expr **); // linked
} qck_expr_call;

typedef struct {
  const char *var_name;
  const char *property_name;
  int index; // linked
} qck_expr_property;

typedef struct {
  struct qck_expr *left;
  u8 operator; // Uses token kinds.
  struct qck_expr *right;
  struct qck_expr *(*func)(struct qck_expr *left,
                           struct qck_expr *right); // Linked
} qck_expr_binary;

typedef struct {
  struct qck_expr *expr;
} qck_expr_rtn;

typedef struct {
  const char *type;
  size_t var_count;
  struct qck_var **vars;
} qck_expr_instantiate;

typedef struct {
  const char *var_name;
  struct qck_expr *value;
  int index; // linked
} qck_expr_assign;

typedef struct {
  size_t len;
  struct qck_expr **values;
} qck_expr_object;

struct qck_expr {
  u8 type;
  union {
    double f64;
    char *string;
    qck_bool bool;
    qck_expr_property *property;
    qck_expr_call *call;
    qck_expr_binary *binary;
    qck_expr_rtn *rtn;
    qck_expr_instantiate *instantiate;
    qck_expr_assign *assign;
    qck_expr_object *object;
  };
};

typedef struct qck_expr qck_expr;

qck_expr *qck_expr_call_new(const char *func_name, u8 arg_count,
                            qck_expr **args);
qck_expr *qck_expr_property_new(const char *var_name,
                                const char *property_name);
qck_expr *qck_expr_binary_new(qck_expr *left, u8 operator, qck_expr * right);
qck_expr *qck_expr_rtn_new(qck_expr *expr);
qck_expr *qck_expr_instantiate_new(const char *type, size_t var_count,
                                   struct qck_var **vars);
qck_expr *qck_expr_assign_new(const char *var_name, qck_expr *value);
qck_expr *qck_expr_f64_new(double value);
qck_expr *qck_expr_object_new(int property_count);
qck_expr *qck_expr_string_new(char *value);
qck_expr *qck_expr_bool_new(qck_bool value);
qck_expr *qck_expr_unary_new(u8 operator, qck_expr * value);
char *qck_expr_call_fmt(qck_expr_call *call);
char *qck_expr_property_fmt(qck_expr_property *property);
char *qck_expr_binary_fmt(qck_expr_binary *binary);
char *qck_expr_rtn_fmt(qck_expr_rtn *rtn);
char *qck_expr_instantiate_fmt(qck_expr_instantiate *instantiate);
char *qck_expr_assign_fmt(qck_expr_assign *assign);
char *qck_expr_fmt(qck_expr *expr);
char *qck_expr_unary_fmt(qck_expr_binary *binary);
qck_expr **qck_exprs(int count, ...);

#define qck_type_kind_f64 1
#define qck_type_kind_name 2
#define qck_type_kind_func 3
#define qck_type_kind_interface 4

struct qck_var;

struct qck_type {
  u8 kind;
  const char *name;
  struct qck_type *result;
  size_t var_count;
  struct qck_var **vars;
};

typedef struct qck_type qck_type;

qck_type *qck_type_new_f64();
qck_type *qck_type_new_name(const char *name);
qck_type *qck_type_new_func(const char *name, qck_type *returns);
qck_type *qck_type_new_interface(const char *name, size_t var_count,
                                 struct qck_var **vars);
char *qck_type_fmt(qck_type *type);
int qck_type_find_index(qck_type *type, const char *name);

struct qck_var {
  const char *name;
  qck_type *type;
  struct qck_expr *value;
};

typedef struct qck_var qck_var;

qck_var *qck_var_new(const char *name, qck_type *type, struct qck_expr *value);
char *qck_var_fmt(qck_var *var);

typedef struct {
  const char *name;
  u8 arg_count;
  qck_var **args;
  qck_type *returns;
  size_t expr_count;
  qck_expr **exprs;

  // Linking
  // int register_count;
  // qck_expr **registers;
} qck_func;

qck_func *qck_func_new(const char *name, size_t arg_count, qck_var **args,
                       qck_type *returns, size_t expr_count, qck_expr **exprs);
char *qck_func_fmt(qck_func *func);

typedef struct {
  size_t type_count;
  qck_type **types;
  size_t func_count;
  qck_func **funcs;
} qck_program;

qck_program *qck_program_new();
char *qck_program_fmt(qck_program *program);
void qck_program_push_type(qck_program *program, qck_type *type);
void qck_program_push_func(qck_program *program, qck_func *func);
qck_type *qck_program_find_type(qck_program *program, const char *type_name);
qck_program *qck_program_parse(const char *s);

#define qck_token_eof 0         // EOF
#define qck_token_identifier 1  // identifier
#define qck_token_string 2      // "string"
#define qck_token_open_paren 3  // (
#define qck_token_close_paren 4 // )
#define qck_token_open_curly 5  // {
#define qck_token_close_curly 6 // }
#define qck_token_plus 7        // +
#define qck_token_func 8        // func
#define qck_token_true 9        // true
#define qck_token_false 10      // false
#define qck_token_and 11        // and
#define qck_token_or 12         // or
#define qck_token_not 13        // not

typedef struct {
  u8 kind;
  char *value; // Only used for identifier and string.
} qck_token;

int qck_tokenize(const char *s, qck_token *tokens);
const char *qck_token_str(u8 kind);
