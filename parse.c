#include "9cc.h"

Var *locals;

Var *find_var(Token *tok) {
    for (Var *var = locals; var; var = var->next) {
        if (strlen(var->name) == tok->len &&
            !memcmp(tok->str, var->name, tok->len))
            return var;
    }
    return NULL;
}

Node *new_node(NodeKind kind) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = new_node(kind);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_num(int val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

Node *new_var(Var *var) {
    Node *node = new_node(ND_VAR);
    node->var = var;
    return node;
}

Var *push_var(char *name) {
    Var *var = calloc(1, sizeof(Var));
    var->next = locals;
    var->name = name;
    locals = var;
    return var;
}

Function *function();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

// program = function*
Function *program() {
    Function head;
    head.next = NULL;
    Function *cur = &head;
    while (!at_eof()) {
        cur->next = function();
        cur = cur->next;
    }
    return head.next;
}

// function = ident "(" ")" "{" stmt* "}"

Function *function() {
    locals = NULL;
    char *name = expect_ident();
    expect("(");
    expect(")");
    expect("{");

    Node head;
    head.next = NULL;
    Node *cur = &head;

    while (!consume("}")) {
        cur->next = stmt();
        cur = cur->next;
    }
    Function *fn = calloc(1, sizeof(Function));
    fn->name = name;
    fn->node = head.next;
    fn->locals = locals;
    return fn;
}

/* stmt = expr";"
        |"if""("expr")" stmt ("else" stmt)?
        |"while" "(" expr ")" stmt
        |"for" "(" expr? ";" expr? ";" expr?")" stmt
        |"{" stmt* "}"
        |"return" expr ";"
*/

Node *stmt() {
    Node *node;
    if (consume("return")) {
        node = new_node(ND_RETURN);
        node->lhs = expr();
    } else if (consume("if")) {
        node = new_node(ND_IF);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume("else")) node->els = stmt();
        return node;
    } else if (consume("while")) {
        node = new_node(ND_WHILE);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;
    } else if (consume("for")) {
        node = new_node(ND_FOR);
        expect("(");
        if (!consume(";")) {
            node->init = expr();
            expect(";");
        }
        if (!consume(";")) {
            node->cond = expr();
            expect(";");
        }
        if (!consume(")")) {
            node->inc = expr();
            expect(")");
        }
        node->then = stmt();
        return node;
    } else if (consume("{")) {
        Node head;
        head.next = NULL;
        Node *cur = &head;

        while (!consume("}")) {
            cur->next = stmt();
            cur = cur->next;
        }
        Node *node = new_node(ND_BLOCK);
        node->body = head.next;
        return node;
    } else {
        node = expr();
    }
    if (!consume(";")) error_at(token->str, "';ではないトークンです");
    return node;
}

// expr = assign
Node *expr() { return assign(); }

// assign = equeality ("=" assign)?
Node *assign() {
    Node *node = equality();
    if (consume("=")) node = new_binary(ND_ASSIGN, node, assign());
    return node;
}

// equality = relational ("==" relaional | "!=" relational)*
Node *equality() {
    Node *node = relational();
    for (;;) {
        if (consume("=="))
            node = new_binary(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_binary(ND_NE, node, relational());
        else
            return node;
    }
}
// relational = add ("<" add | " <= " add | ">" add | ">=" add)*
Node *relational() {
    Node *node = add();
    for (;;) {
        if (consume("<"))
            node = new_binary(ND_LT, node, add());
        else if (consume("<="))
            node = new_binary(ND_LE, node, add());
        else if (consume(">"))
            node = new_binary(ND_LT, add(), node);
        else if (consume(">="))
            node = new_binary(ND_LE, add(), node);
        else
            return node;
    }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_binary(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_binary(ND_DIV, node, unary());
        else
            return node;
    }
}

// add = mul("+" mul | "-" mul)*
Node *add() {
    Node *node = mul();
    for (;;) {
        if (consume("+"))
            node = new_binary(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_binary(ND_SUB, node, mul());
        else
            return node;
    }
}

// unary = ("+|-")? unary
Node *unary() {
    if (consume("+")) return unary();
    if (consume("-")) return new_binary(ND_SUB, new_num(0), unary());
    return primary();
}

/* primary = "(" expr")""
            | num
            | ident args?
    args = "("(assign ("," assign)*)?")"
*/
Node *func_args() {
    if (consume(")")) return NULL;

    Node *head = assign();
    Node *cur = head;
    while (consume(",")) {
        cur->next = assign();
        cur = cur->next;
    }
    expect(")");
    return head;
}
Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume_ident();
    if (tok) {
        if (consume("(")) {
            Node *node = new_node(ND_FUNCALL);
            node->funcname = strndup(tok->str, tok->len);
            node->args = func_args();
            return node;
        }
        Var *var = find_var(tok);
        if (!var) {
            var = push_var(strndup(tok->str, tok->len));
        }
        return new_var(var);
    }

    return new_num(expect_number());
}
