#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef INCLUDE_9CC
#define INCLUDE_9cc

//トークンの種類
typedef enum {
    TK_RESERVED,  // 記号
    TK_NUM,       // 整数
    TK_EOF,       // 入力終わり
    TK_IDENT,     // 識別子
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind;  // トークンの型
    Token *next;     //次の入力トークン
    int val;         // kindがTK_NUMの場合，その数値
    char *str;       // トークンの文字列
    int len;
};

void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
bool consume(char *op);
void expect(char *op);
int expect_number();
bool at_eof();
Token *new_token(TokenKind kind, Token *cur, char *str, int len);
Token *tokenize();

extern char *user_input;
extern Token *token;

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
} NodeKind;

typedef struct Node Node;

struct Node {
    NodeKind kind;
    Node *next;
    Node *lhs;
    Node *rhs;
    int val;
};

Node *program();

void codegen(Node *node);

#endif