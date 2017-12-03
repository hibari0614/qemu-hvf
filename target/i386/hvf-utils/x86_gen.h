/*
 * Copyright (C) 2016 Veertu Inc,
 * Copyright (C) 2017 Google Inc,
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __X86_GEN_H__
#define __X86_GEN_H__

#include <stdlib.h>
#include <stdio.h>
#include "qemu-common.h"

typedef uint64_t addr_t;

#define VM_PANIC(x) {\
    printf("%s\n", x); \
    abort(); \
}

#define VM_PANIC_ON(x) {\
    if (x) { \
        printf("%s\n", #x); \
        abort(); \
    } \
}

#define VM_PANIC_EX(...) {\
    printf(__VA_ARGS__); \
    abort(); \
}

#define VM_PANIC_ON_EX(x, ...) {\
    if (x) { \
        printf(__VA_ARGS__); \
        abort(); \
    } \
}

#define ZERO_INIT(obj) memset((void *) &obj, 0, sizeof(obj))

#endif
