#include "qck.h"

int qck_tokenize(const char *s, qck_token *tokens) {
  int token_count = 0;
  while (*s) {
    switch (*s) {
    case '(':
      tokens[token_count].kind = qck_token_open_paren;
      goto next;
    case ')':
      tokens[token_count].kind = qck_token_close_paren;
      goto next;
    case '{':
      tokens[token_count].kind = qck_token_open_curly;
      goto next;
    case '}':
      tokens[token_count].kind = qck_token_close_curly;
      goto next;
    case '+':
      tokens[token_count].kind = qck_token_plus;
      goto next;
    case '"':
      s++;
      int len = 0;
      while (s[len] != '"') {
        ++len;
      }
      tokens[token_count].kind = qck_token_string;
      tokens[token_count].value = malloc(len + 1);
      memcpy(tokens[token_count].value, s, len);
      s += len;
      goto next;
    case '/':
      s++;
      while (*s != '\n' && *s != 0) {
        ++s;
      }
      --token_count;
      goto next;
    case ' ':
    case '\n':
    case '\t':
    case '\r':
      --token_count;
      goto next;
    }

    int len = 0;
    while (s[len] >= 'a' && s[len] <= 'z' && s[len] != 0) {
      ++len;
    }

    if (len == 0) {
      printf("failure at: %s\n", s);
      exit(1);
    }

    if (strncmp("func", s, len) == 0) {
      tokens[token_count].kind = qck_token_func;
    } else if (strncmp("true", s, len) == 0) {
      tokens[token_count].kind = qck_token_true;
    } else if (strncmp("false", s, len) == 0) {
      tokens[token_count].kind = qck_token_false;
    } else if (strncmp("and", s, len) == 0) {
      tokens[token_count].kind = qck_token_and;
    } else if (strncmp("or", s, len) == 0) {
      tokens[token_count].kind = qck_token_or;
    } else if (strncmp("not", s, len) == 0) {
      tokens[token_count].kind = qck_token_not;
    } else {
      tokens[token_count].kind = qck_token_identifier;
      tokens[token_count].value = malloc(len + 1);
      memcpy(tokens[token_count].value, s, len);
    }
    s += len - 1;

  next:
    token_count++;
    s++;
  }

  return token_count;
}

const char *qck_token_str(u8 kind) {
  switch (kind) {
  case qck_token_plus:
    return "+";
  case qck_token_and:
    return "and";
  case qck_token_or:
    return "or";
  case qck_token_not:
    return "not";
  }

  return NULL;
}