#include<ctype.h>
#include<stdarg.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数
    TK_EOF,      // 入力終わり
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next; //次の入力トークン
    int val;   // kindがTK_NUMの場合，その数値
    char *str; // トークンの文字列
};
// 現在着目しているトークン
Token *token;

// エラー報告をする関数
void error (char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

char *user_input;
char error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap,fmt);
    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " "); // pos個の空白を出力
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 次のトークンが期待している記号のときは，トークンを一つ読み進めて真を返す．
bool consume(char op){
    if (token->kind != TK_RESERVED || token->str[0] != op) return false;
    token = token -> next;
    return true;
}

// 次のトークンが期待している記号のときは，トークンを一つ読み進める．
void expect(char op) {
    if (token->kind != TK_RESERVED || token->str[0] != op){
        error("'%c',ではありません", op);
    }
    token = token -> next;
}

int expect_number() {
    if (token -> kind != TK_NUM){
        error_at(token->str,"数ではありません");
    }
    int val = token -> val;
    token = token -> next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char* str){
    Token *tok = calloc(1,sizeof(Token));
    tok -> kind = kind;
    tok -> str = str;
    cur -> next = tok;
    return tok;
}

//入力文字pをトークナイズしてそれを返す
Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;
    while (*p) {
        if (isspace(*p)){
            p++;
            continue;
        }

        if (*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }

        if (isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur -> val = strtol(p, &p ,10);
            continue;
        }
        error("トークナイズできません");
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc, char ** argv){
    if (argc != 2){
        fprintf(stderr, "引数の個数が足りません");
        return 1;
    }
    user_input = argv[1];
    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax, %d\n", expect_number());
            continue;
        }
        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }
    printf("    ret\n");
    return 0;
}