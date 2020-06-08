//
// Created by Jiang Yinzuo on 2020/5/28.
//

#ifndef CALC_TOKEN_H
#define CALC_TOKEN_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "error_handle.h"

#define MAX_TOKEN_LEN 20
#define OPERATOR_COUNT 14
typedef struct Token {
    enum TYPE {
        WELL, EQ, QUESTION, ADD, SUB, MULTI, DIV,
        L_BRACKET, R_BRACKET, VAR, CONST, CLEAR, RVALUE, SENTENCE
    } token_type;

    double d_value;
    char varname[MAX_TOKEN_LEN + 1];

} Token;

#define MAX_TL_LEN 20 /* 一条语句最大单词数量 */
typedef struct TokenList {
    int len;
    Token tokens[MAX_TL_LEN + 2]; /* 加上首尾两个# */
} TokenList;

static inline void token_list_init(TokenList *token_list) {
    bzero(token_list, sizeof(TokenList));
}

static inline void token_list_check(TokenList *token_list) {
    if (token_list->len == MAX_TL_LEN + 2) {
        error_handle("语句单词数量不得超过%d\n", MAX_TOKEN_LEN);
    }
}

static inline void add_token(enum TYPE type, TokenList *token_list) {
    token_list->tokens[token_list->len++].token_type = type;
}

static inline void add_const_token(enum TYPE type, TokenList *token_list, double d_value) {
    token_list->tokens[token_list->len].d_value = d_value;
    token_list->tokens[token_list->len++].token_type = type;
}

static inline void add_var_token(enum TYPE type, TokenList *token_list, char varname[]) {
    strcpy(token_list->tokens[token_list->len].varname, varname);
    token_list->tokens[token_list->len++].token_type = type;
}

static inline void clear_token(TokenList *token_list) {
    token_list->len = 0;
}

#endif //CALC_TOKEN_H
