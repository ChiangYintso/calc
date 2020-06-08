//
// Created by Jiang Yinzuo on 2020/5/28.
//

#include "vm.h"
#include "token.h"

#define CHAR_COUNT 63
#define MAX_TRIE_NODE 1000
struct VarTable {
    int next[MAX_TRIE_NODE][CHAR_COUNT];
    _Bool exists[MAX_TRIE_NODE];
    double value[MAX_TRIE_NODE];
    int total;
};

static struct VarTable vt;

void vm_clear() {
    bzero(&vt, sizeof(struct VarTable));
}

static inline int ch_id(char ch) {
    if ('0' <= ch && ch <= '9') return ch - '0';
    if ('a' <= ch && ch <= 'z') return ch - 'a' + 10;
    if ('A' <= ch && ch <= 'Z') return ch - 'A' + 36;
    return 62;
}

_Bool vm_load(const char *varname, double *r) {
    int t_idx = 0;
    for (int i = 0; varname[i]; ++i) {
        if ((t_idx = vt.next[t_idx][ch_id(varname[i])]) == 0) {
            return false;
        }
    }
    if (vt.exists[t_idx]) {
        *r = vt.value[t_idx];
        return true;
    }
    return false;
}

ERR_NO vm_assign(const char *varname, double v) {
    int t_idx = 0;
    for (int i = 0; varname[i]; ++i) {
        if (vt.next[t_idx][ch_id(varname[i])] == 0) {
            t_idx = vt.next[t_idx][ch_id(varname[i])] = ++vt.total;
            if (vt.total == MAX_TRIE_NODE) {
                return STACK_OVERFLOW_ERR;
            }
        } else t_idx = vt.next[t_idx][ch_id(varname[i])];
    }
    vt.exists[t_idx] = true;
    vt.value[t_idx] = v;
    return SUCCESS;
}
