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
    TK_RETURN,
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
char *strndup(char *p, int len);
Token *consume_ident();
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
    ND_ASSIGN,
    ND_VAR,  // 変数
    ND_RETURN,
} NodeKind;

typedef struct Node Node;

typedef struct Var Var;

struct Var {
    Var *next;   // 次の変数orNULL
    char *name;  // 変数名
    int len;     // 名前の長さ
    int offset;  // RBPからのオフセット
};

struct Node {
    NodeKind kind;
    Node *next;
    Node *lhs;
    Node *rhs;
    int val;
    Var *var;
};

Var *find_lvar(Token *tok);

typedef struct {
    Node *node;
    Var *locals;
    int stack_size;
} Program;

Program *program();

void codegen(Program *prog);

#endif