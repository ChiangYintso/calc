//
// Created by Jiang Yinzuo on 2020/5/28.
//

#include "lexer.h"

static inline _Bool is_blank(char ch) {
    return ch == 0 || ch == ' ' || ch == '\r' || ch == '\n';
}

static inline _Bool is_digit(char ch) {
    return '0' <= ch && ch <= '9';
}

static inline _Bool is_digit_or_dot(char ch) {
    return ch == '.' || is_digit(ch);
}

static inline _Bool is_identifier_begin(char ch) {
    return ('A' <= ch && ch <= 'Z') ||
           ('a' <= ch && ch <= 'z') ||
           ch == '_';
}

static inline _Bool is_identifier(char ch) {
    return is_identifier_begin(ch) || is_digit(ch);
}

static void divide_token(_Bool (*ch_func)(char),
                         char tar_buf[],
                         int *restrict end_idx,
                         char *restrict src_buf) {
    int buf_i = 0;
    while (ch_func(src_buf[*end_idx])) {
        tar_buf[buf_i++] = src_buf[(*end_idx)++];
        if (buf_i > 20) {
            error_handle("符号长度不得超过%d\n", MAX_TOKEN_LEN);
        }
    }
    tar_buf[buf_i] = '\0';
}

#define DIVIDE_TOKEN(ch_func) divide_token(ch_func, \
                                           buf, \
                                           &i, \
                                           lexer->buf)

static _Bool str_to_double(const char *buf, double *result) {

    int i;
    for (i = 0; buf[i] && buf[i] != '.'; ++i) {
        *result = *result * 10 + buf[i] - '0';
    }
    if (buf[i] == '.') {
        ++i;
        double base = 0.1;
        for (; buf[i] && base >= 0.0001; ++i) {
            if (buf[i] == '.') {
                error_handle("非法数字");
                return false;
            }
            *result += base * (buf[i] - '0');
            base /= 10;
        }
    }
    return true;
}

_Bool lex(Lexer *lexer, TokenList *token_list) {

    char buf[MAX_TOKEN_LEN + 1];
    add_token(WELL, token_list);
    for (int i = 0; lexer->buf[i];) {
        if (!is_blank(lexer->buf[i])) {
            token_list_check(token_list);

            if (is_identifier_begin(lexer->buf[i])) {
                DIVIDE_TOKEN(is_identifier);
                if (strcmp(buf, "clear") == 0)
                    add_token(CLEAR, token_list);
                else
                    add_var_token(VAR, token_list, buf);
            } else if (is_digit_or_dot(lexer->buf[i])) {
                DIVIDE_TOKEN(is_digit_or_dot);
                double value = 0;
                if (str_to_double(buf, &value))
                    add_const_token(CONST, token_list, value);
                else return false;
            } else {
                switch (lexer->buf[i]) {
                    case '=':
                        add_token(EQ, token_list);
                        break;
                    case '?':
                        add_token(QUESTION, token_list);
                        break;
                    case '+':
                        add_token(ADD, token_list);
                        break;
                    case '-':
                        add_token(SUB, token_list);
                        break;
                    case '*':
                        add_token(MULTI, token_list);
                        break;
                    case '/':
                        add_token(DIV, token_list);
                        break;
                    case '(':
                        add_token(L_BRACKET, token_list);
                        break;
                    case ')':
                        add_token(R_BRACKET, token_list);
                        break;
                    default:
                        error_handle("非法的符号%s", lexer->buf[i]);
                        return false;
                }
                ++i;
            }
        } else {
            ++i;
        }
    }
    add_token(WELL, token_list);
    return true;
}
