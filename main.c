#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

_Bool err_exit;

_Noreturn void help() {
    printf("calc使用方法：\n");
    printf("1. 交互模式：直接在控制台运行calc;\n");
    printf("2. 文件模式：在控制台输入calc <文件名>，解释文件中的语句。\n\n");

    printf("calc语法：\n");
    printf("赋值：v = E\n");
    printf("屏幕输出表达式：E?\n");
    printf("四则运算：E + E; E - E; E * E; E / E，E可以是变量、常量或表达式。\n");
    printf("括号()中的表达式优先运算。\n");
    printf("clear 清空变量\n");
    exit(EXIT_SUCCESS);
}

void scan(Lexer *lexer, TokenList *token_list) {

    do {
        lexer->buf[MAX_BUF - 2] = 0;
        if (!err_exit) printf(">>> ");
        fgets(lexer->buf, MAX_BUF, lexer->in);
        if (lexer->buf[MAX_BUF - 2]) {
            error_handle("一条语句字符长度不得超过%d(含回车符)\n", MAX_BUF);
            while (getc(lexer->in) != '\n');
        } else {
            if ((!lex(lexer, token_list) || !parse(token_list)) && err_exit)
                exit(EXIT_FAILURE);
            clear_token(token_list);
        }

    } while (lexer->buf[0] != EOF);
}

int main(int argc, char *argv[]) {
    FILE *in = NULL;
    err_exit = false;
    if (argc > 2) {
        error_handle("参数错误。用法为 calc [<文件名>]\n");
        exit(EXIT_FAILURE);
    } else if (argc == 1) {
        in = stdin;
        printf("欢迎使用calc, 如需帮助可使用命令calc -h\n");
    } else if (strcmp(argv[1], "-h") == 0) {
        help();
    } else {
        err_exit = true;
        if ((in = fopen(argv[1], "r")) == NULL) {
            error_handle("找不到文件 %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    Lexer lexer;
    lexer_init(&lexer, in);
    TokenList token_list;
    token_list_init(&token_list);

    scan(&lexer, &token_list);

    if (argc == 2) {
        free(in);
    }
    return 0;
}
