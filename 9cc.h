#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef INCLUDE_9CC
#define INCLUDE_9cc

typedef struct Type Type;

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
void error_tok(Token *tok, char *fmt, ...);
Token *peek(char *s);
Token *consume(char *op);
char *strndup(char *p, int len);
Token *consume_ident();
void expect(char *op);
int expect_number();
char *expect_ident();
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
    ND_EQ,       // ==
    ND_NE,       // !=
    ND_LT,       // <
    ND_LE,       // <=
    ND_ASSIGN,   // =
    ND_ADDR,     // unary &
    ND_DEREF,    // unary *
    ND_VAR,      // 変数
    ND_RETURN,   // return
    ND_IF,       // if()
    ND_WHILE,    // while()
    ND_FOR,      // for (;;)
    ND_BLOCK,    // { ... }
    ND_FUNCALL,  // Function call
    ND_NULL,     // Empty statement
} NodeKind;

typedef struct Node Node;

typedef struct Var Var;

struct Var {
    char *name;  // 変数名
    int len;     // 名前の長さ
    int offset;  // RBPからのオフセット
    Type *ty;
};

typedef struct VarList VarList;
struct VarList {
    VarList *next;
    Var *var;
};

struct Node {
    NodeKind kind;
    Node *next;
    Node *lhs;
    Node *rhs;

    Type *ty;
    Token *tok;

    // "if" statement
    Node *cond;
    Node *then;
    Node *els;

    // "for" statement
    Node *init;
    Node *inc;

    // "block"
    Node *body;

    // function call
    char *funcname;
    Node *args;

    int val;
    Var *var;
};

Var *find_lvar(Token *tok);

typedef struct Function Function;
struct Function {
    Function *next;
    char *name;
    Node *node;
    VarList *params;
    VarList *locals;
    int stack_size;
};

typedef enum { TY_INT, TY_PTR, TY_ARRAY } TypeKind;

struct Type {
    TypeKind kind;
    Type *base;
    int array_size;
};

Type *int_type();
Type *pointer_to();
Type *array_of(Type *base, int size);
int size_of(Type *ty);

void add_type(Function *prog);

Function *program();

void codegen(Function *prog);

#endif