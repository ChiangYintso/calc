//
// Created by Jiang Yinzuo on 2020/5/28.
//

#include "parser.h"
#include "vm.h"

static char *msg;

static const char *err_fmt[] = {
        [0] = "success",
        [SYNTAX_ERR] = "语法错误",
        [VAR_NOT_DEFINED_ERR] = "找不到变量%s",
        [STACK_OVERFLOW_ERR] = "变量堆栈溢出，请清零",
        [ARITHMETIC_ERR] = "0不能做除数",
};

/* S → v = E | E? | clear | #S#
 * E → E + T | E - T | T
 * T → T * F | T / F | F
 * F → (E) | v | c */
#define CA_COUNT 12
static int ca[CA_COUNT][3] = {
        {VAR,       EQ,       RVALUE},  // v = E
        {RVALUE,    QUESTION, -1}, // E?
        {CLEAR,     -1,       -1},            // clear
        {RVALUE,    ADD,      RVALUE}, // E + T
        {RVALUE,    SUB,      RVALUE}, // E - T
        {RVALUE,    MULTI,    RVALUE}, // E * T
        {RVALUE,    DIV,      RVALUE},  // E / T
        {RVALUE,    -1,       -1},                // T or F
        {L_BRACKET, RVALUE,   R_BRACKET}, // (E)
        {VAR,       -1,       -1},                         // v
        {CONST,     -1,       -1},                       // c
        {WELL,      SENTENCE,   WELL}           // # S #
};

/*                     #,   =,   ?,   +,   -,   *,   /,   (,   ),   v,   c,   clear */
static char op_table[OPERATOR_COUNT - 2][OPERATOR_COUNT - 2] = {
        [WELL] =     {'=', '0', '<', '<', '<', '<', '<', '<', '0', '<', '<', '<'},
        [EQ]   =     {'>', '0', '0', '<', '<', '<', '<', '<', '0', '<', '<', '0'},
        [QUESTION] = {'>', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
        [ADD]  =     {'>', '0', '>', '>', '>', '<', '<', '<', '>', '<', '<', '0'},
        [SUB]  =     {'>', '0', '>', '>', '>', '<', '<', '<', '>', '<', '<', '0'},
        [MULTI] =    {'>', '0', '>', '>', '>', '>', '>', '<', '>', '<', '<', '0'},
        [DIV]  =     {'>', '0', '>', '>', '>', '>', '>', '<', '>', '<', '<', '0'},
        [L_BRACKET]= {'0', '0', '<', '<', '<', '<', '<', '<', '=', '<', '<', '0'},
        [R_BRACKET]= {'>', '0', '>', '>', '>', '>', '>', '0', '>', '0', '0', '0'},
        [VAR] =      {'>', '=', '>', '>', '>', '>', '>', '0', '>', '0', '0', '0'},
        [CONST]  =   {'>', '0', '>', '>', '>', '>', '>', '0', '>', '0', '0', '0'},
        [CLEAR] =    {'>', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
};

struct ReduceStack {
    enum TYPE last_tk_type;
    Token *stack[MAX_TOKEN_LEN + 2];
    int idx;
    Token *iter;
};

static inline void rs_init(struct ReduceStack *rs, TokenList *tk) {
    bzero(rs, sizeof(struct ReduceStack));
    rs->iter = tk->tokens;
}

static inline void rs_push(struct ReduceStack *rs) {
    rs->stack[rs->idx++] = rs->iter;
    rs->last_tk_type = rs->iter->token_type;
    ++rs->iter;
}

static inline char rs_cmp(struct ReduceStack *rs) {
    return op_table[rs->last_tk_type][rs->iter->token_type];
}

static inline _Bool rs_end(struct ReduceStack *rs) {
    return rs->stack[rs->idx - 1]->token_type == WELL;
}

/**
 * 比较归约栈中的符号是否在产生式表中
 * @param stack
 * @param i
 * @param from
 * @param to
 * @return 是：true；否：false
 */
static _Bool i_cmp(Token *stack[], int i, int from, int to) {
    int len = to - from + 1;
    if ((ca[i][1] == -1 && len != 1) ||
        (ca[i][1] != -1 && ca[i][2] == -1 && len != 2) ||
        (ca[i][2] != -1 && len != 3))
        return false;

    for (int j = from; j <= to; ++j) {
        if (ca[i][j - from] != stack[j]->token_type) {
            return false;
        }
    }
    return true;
}

static ERR_NO can_reduce(Token *stack[], int from, int to) {
    if (to - from + 1 > 3)
        return SYNTAX_ERR;

    // v = E
    if (i_cmp(stack, 0, from, to)) {
        stack[from]->token_type = SENTENCE;
        return vm_assign(stack[from]->varname, stack[to]->d_value);
    }


    // E?
    if (i_cmp(stack, 1, from, to)) {
        vm_output(stack[from]->d_value);
        stack[from]->token_type = SENTENCE;
        return SUCCESS;
    }

    // clear
    if (i_cmp(stack, 2, from, to)) {
        vm_clear();
        stack[from]->token_type = SENTENCE;
        return SUCCESS;
    }

    // E + T
    if (i_cmp(stack, 3, from, to)) {
        stack[from]->d_value += stack[to]->d_value;
        stack[from]->token_type = RVALUE;
        return SUCCESS;
    }

    // E - T
    if (i_cmp(stack, 4, from, to)) {
        stack[from]->d_value -= stack[to]->d_value;
        stack[from]->token_type = RVALUE;
        return SUCCESS;
    }

    // E * T
    if (i_cmp(stack, 5, from, to)) {
        stack[from]->d_value *= stack[to]->d_value;
        stack[from]->token_type = RVALUE;
        return SUCCESS;
    }

    // E / T
    if (i_cmp(stack, 6, from, to)) {
        if (stack[to]->d_value == 0) {
            return ARITHMETIC_ERR;
        }
        stack[from]->d_value /= stack[to]->d_value;
        stack[from]->token_type = RVALUE;
        return SUCCESS;
    }

    // T or F
    if (i_cmp(stack, 7, from, to)) {
        stack[from]->token_type = RVALUE;
        return SUCCESS;
    }

    // (E)
    if (i_cmp(stack, 8, from, to)) {
        stack[from]->d_value = stack[from + 1]->d_value;
        stack[from]->token_type = RVALUE;
        return SUCCESS;
    }

    // v
    if (i_cmp(stack, 9, from, to)) {
        double value;
        stack[from]->token_type = RVALUE;
        if (vm_load(stack[from]->varname, &value)) {
            stack[from]->d_value = value;
            return SUCCESS;
        }
        msg = stack[from]->varname;
        return VAR_NOT_DEFINED_ERR;
    }

    // c
    if (i_cmp(stack, 10, from, to)) {
        stack[from]->token_type = RVALUE;
        return SUCCESS;
    }

    // # S #
    if (i_cmp(stack, 11, from, to)) {
        stack[from]->token_type = SENTENCE;
        return SUCCESS;
    }
    return SYNTAX_ERR;
}

static _Bool rs_reduce(struct ReduceStack *rs) {
    int from = rs->idx - 1;
    enum TYPE last_type = rs->iter->token_type;
    while (rs->stack[from]->token_type == RVALUE ||
           op_table[rs->stack[from]->token_type][last_type] != '<') {

        if (rs->stack[from]->token_type != RVALUE) {
            last_type = rs->stack[from]->token_type;
        }
        --from;
    }

    ERR_NO err_no = can_reduce(rs->stack, from + 1, rs->idx - 1);
    if (err_no == SUCCESS) {
        rs->idx = from + 2;
        while (from >= 0) {
            if (rs->stack[from]->token_type != RVALUE) {
                rs->last_tk_type = rs->stack[from]->token_type;
                break;
            }
            --from;
        }
    } else {
        error_handle(err_fmt[err_no], msg);
    }
    return err_no == SUCCESS;
}

_Bool parse(TokenList *token_list) {
    struct ReduceStack rs;
    rs_init(&rs, token_list);
    rs_push(&rs);
    rs_push(&rs);
    while (!rs_end(&rs)) {
        switch (rs_cmp(&rs)) {
            case '>':
                if (!rs_reduce(&rs)) return false;
                break;
            case '=':
            case '<':
                rs_push(&rs);
                break;
            case '0':
            default:
                error_handle(err_fmt[SYNTAX_ERR]);
                return false;
        }
    }
    return true;
}