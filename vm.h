//
// Created by Jiang Yinzuo on 2020/5/28.
//

#ifndef CALC_VM_H
#define CALC_VM_H

#include "error_handle.h"

void vm_clear();

_Bool vm_load(const char *varname, double *r);

ERR_NO vm_assign(const char *varname, double v);

static inline void vm_output(double v) {
    if (v == (int) v) {
        printf("%d\n", (int) v);
    } else {
        printf("%.2lf\n", v);
    }
}

#endif //CALC_VM_H
