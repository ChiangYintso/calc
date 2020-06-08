//
// Created by Jiang Yinzuo on 2020/5/28.
//

#ifndef CALC_LEXER_H
#define CALC_LEXER_H

#include <stdio.h>
#include "token.h"

#define MAX_BUF 80
typedef struct Lexer {
    FILE *in;
    char buf[MAX_BUF];
} Lexer;

static inline void lexer_init(Lexer *lexer, FILE *in) {
    lexer->in = in;
}

_Bool lex(Lexer *lexer, TokenList *token_list);

#endif //CALC_LEXER_H
