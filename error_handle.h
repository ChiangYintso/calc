//
// Created by Jiang Yinzuo on 2020/5/28.
//

#ifndef CALC_ERROR_HANDLE_H
#define CALC_ERROR_HANDLE_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 0
#define SYNTAX_ERR 1
#define VAR_NOT_DEFINED_ERR 2
#define STACK_OVERFLOW_ERR 3
#define ARITHMETIC_ERR 4

extern _Bool err_exit;

typedef int ERR_NO;

static inline void error_handle(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
    if (err_exit)
        exit(EXIT_FAILURE);
}

#endif //CALC_ERROR_HANDLE_H
