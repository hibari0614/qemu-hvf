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
#ifndef __X86_EMU_H__
#define __X86_EMU_H__

#include "x86.h"
#include "x86_decode.h"

void init_emu(struct CPUState *cpu);
bool exec_instruction(struct CPUState *cpu, struct x86_decode *ins);

void load_regs(struct CPUState *cpu);
void store_regs(struct CPUState *cpu);

void simulate_rdmsr(struct CPUState *cpu);
void simulate_wrmsr(struct CPUState *cpu);

addr_t read_reg(struct CPUState *cpu, int reg, int size);
void write_reg(struct CPUState *cpu, int reg, addr_t val, int size);
addr_t read_val_from_reg(addr_t reg_ptr, int size);
void write_val_to_reg(addr_t reg_ptr, addr_t val, int size);
void write_val_ext(struct CPUState *cpu, addr_t ptr, addr_t val, int size);
uint8_t *read_mmio(struct CPUState *cpu, addr_t ptr, int bytes);
addr_t read_val_ext(struct CPUState *cpu, addr_t ptr, int size);

void exec_movzx(struct CPUState *cpu, struct x86_decode *decode);
void exec_shl(struct CPUState *cpu, struct x86_decode *decode);
void exec_movsx(struct CPUState *cpu, struct x86_decode *decode);
void exec_ror(struct CPUState *cpu, struct x86_decode *decode);
void exec_rol(struct CPUState *cpu, struct x86_decode *decode);
void exec_rcl(struct CPUState *cpu, struct x86_decode *decode);
void exec_rcr(struct CPUState *cpu, struct x86_decode *decode);
#endif
