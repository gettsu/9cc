#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        error("%s: invalid number of arguments", argv[0]);
    }
    user_input = argv[1];
    token = tokenize(argv[1]);
    Program *prog = program();

    int offset = 0;
    for (Var *var = prog->locals; var; var = var->next) {
        offset += 8;
        var->offset = offset;
    }
    prog->stack_size = offset;
    codegen(prog);
    return 0;
}