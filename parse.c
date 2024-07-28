#include "qck.h"

qck_expr *consume_expr(qck_token *tokens, int *pos);

qck_token consume_token(qck_token *tokens, int *pos, u8 kind) {
  qck_token token = tokens[*pos];
  if (token.kind != kind) {
    printf("pos %d: expecting %d, but found %d\n", *pos, kind, token.kind);
    exit(1);
  }
  (*pos)++;
  return token;
}

qck_expr *consume_binary(qck_token *tokens, int *pos) {
  consume_token(tokens, pos, qck_token_open_paren);
  qck_expr *left = consume_expr(tokens, pos);

  qck_token op;
  switch (tokens[*pos].kind) {
  case qck_token_plus:
  case qck_token_and:
  case qck_token_or:
    op = consume_token(tokens, pos, tokens[*pos].kind);
    break;
  }
  qck_expr *right = consume_expr(tokens, pos);
  consume_token(tokens, pos, qck_token_close_paren);

  return qck_expr_binary_new(left, op.kind, right);
}

qck_expr *consume_expr(qck_token *tokens, int *pos) {
  switch (tokens[*pos].kind) {
  case qck_token_true:
  case qck_token_false:
    return qck_expr_bool_new(tokens[(*pos)++].kind == qck_token_true);
  case qck_token_not: {
    (*pos)++; // skip not
    return qck_expr_unary_new(qck_token_not, consume_expr(tokens, pos));
  }
  case qck_token_string:
    return qck_expr_string_new(
        consume_token(tokens, pos, qck_token_string).value);
  case qck_token_open_paren:
    return consume_binary(tokens, pos);
  case qck_token_identifier: {
    qck_token call = consume_token(tokens, pos, qck_token_identifier);
    consume_token(tokens, pos, qck_token_open_paren);
    qck_expr *arg = consume_expr(tokens, pos);
    consume_token(tokens, pos, qck_token_close_paren);

    return qck_expr_call_new(call.value, 1, qck_exprs(1, arg));
  }
  }

  return NULL;
}

qck_func *consume_func(qck_token *tokens, int *pos) {
  consume_token(tokens, pos, qck_token_func);
  qck_token name = consume_token(tokens, pos, qck_token_identifier);
  consume_token(tokens, pos, qck_token_open_paren);
  consume_token(tokens, pos, qck_token_close_paren);
  consume_token(tokens, pos, qck_token_open_curly);

  qck_expr **exprs = calloc(sizeof(qck_expr *), 10);
  int expr_count = 0;
  while (tokens[*pos].kind != qck_token_close_curly) {
    exprs[expr_count++] = consume_expr(tokens, pos);
  }
  consume_token(tokens, pos, qck_token_close_curly);

  return qck_func_new(name.value, 0, NULL, NULL, expr_count, exprs);
}

qck_program *qck_program_parse(const char *s) {
  qck_token *tokens = calloc(sizeof(qck_token), 100);
  int token_count = qck_tokenize(s, tokens);

  // printf("found %d\n", token_count);
  // for (int i = 0; i < token_count; i++) {
  //   printf("%d [%d %s]\n", i, tokens[i].kind, tokens[i].value);
  // }

  qck_program *program = qck_program_new();
  int pos = 0;
  qck_program_push_func(program, consume_func(tokens, &pos));

  return program;
}
