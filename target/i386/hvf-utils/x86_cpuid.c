/*
 *  i386 CPUID helper functions
 *
 *  Copyright (c) 2003 Fabrice Bellard
 *  Copyright (c) 2017 Google Inc.
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
 *
 * cpuid
 */

#include "qemu/osdep.h"
#include "x86_cpuid.h"
#include "x86.h"
#include "vmx.h"

#define PPRO_FEATURES (CPUID_FP87 | CPUID_DE | CPUID_PSE | CPUID_TSC | \
    CPUID_MSR | CPUID_MCE | CPUID_CX8 | CPUID_PGE | CPUID_CMOV | \
    CPUID_PAT | CPUID_FXSR | CPUID_MMX | CPUID_SSE | CPUID_SSE2 | \
    CPUID_PAE | CPUID_SEP | CPUID_APIC)

struct x86_cpuid builtin_cpus[] = {
    {
        .name = "vmx32",
        .vendor1  = CPUID_VENDOR_INTEL_1,
        .vendor2  = CPUID_VENDOR_INTEL_2,
        .vendor3  = CPUID_VENDOR_INTEL_3,
        .level = 4,
        .family = 6,
        .model = 3,
        .stepping = 3,
        .features = PPRO_FEATURES,
        .ext_features = /*CPUID_EXT_SSE3 |*/ CPUID_EXT_POPCNT, CPUID_MTRR |
                    CPUID_CLFLUSH, CPUID_PSE36,
        .ext2_features = CPUID_EXT2_LM | CPUID_EXT2_SYSCALL | CPUID_EXT2_NX,
        .ext3_features = 0, /* CPUID_EXT3_LAHF_LM, */
        .xlevel = 0x80000004,
        .model_id = "vmx32",
    },
    {
        .name = "core2duo",
        .vendor1  = CPUID_VENDOR_INTEL_1,
        .vendor2  = CPUID_VENDOR_INTEL_2,
        .vendor3  = CPUID_VENDOR_INTEL_3,
        .level = 10,
        .family = 6,
        .model = 15,
        .stepping = 11,
        .features = PPRO_FEATURES |
        CPUID_MTRR | CPUID_CLFLUSH | CPUID_MCA |
        CPUID_PSE36 | CPUID_VME | CPUID_DTS | CPUID_ACPI | CPUID_SS |
        CPUID_HT | CPUID_TM | CPUID_PBE,
        .ext_features = CPUID_EXT_SSE3 | CPUID_EXT_SSSE3 | 
        CPUID_EXT_DTES64 | CPUID_EXT_DSCPL |
        CPUID_EXT_CX16 | CPUID_EXT_XTPR | CPUID_EXT_PDCM | CPUID_EXT_HYPERVISOR,
        .ext2_features = CPUID_EXT2_LM | CPUID_EXT2_SYSCALL | CPUID_EXT2_NX,
        .ext3_features = CPUID_EXT3_LAHF_LM,
        .xlevel = 0x80000008,
        .model_id = "Intel(R) Core(TM)2 Duo GETCPU     T7700  @ 2.40GHz",
    },
    {
        .name = "vmX",
        .vendor1  = CPUID_VENDOR_INTEL_1,
        .vendor2  = CPUID_VENDOR_INTEL_2,
        .vendor3  = CPUID_VENDOR_INTEL_3,
        .level = 0xd,
        .family = 6,
        .model = 15,
        .stepping = 11,
        .features = PPRO_FEATURES |
        CPUID_MTRR | CPUID_CLFLUSH | CPUID_MCA |
        CPUID_PSE36 | CPUID_VME | CPUID_DTS | CPUID_ACPI | CPUID_SS |
        CPUID_HT | CPUID_TM | CPUID_PBE,
        .ext_features = CPUID_EXT_SSE3 | CPUID_EXT_SSSE3 |
        CPUID_EXT_DTES64 | CPUID_EXT_DSCPL |
        CPUID_EXT_CX16 | CPUID_EXT_XTPR | CPUID_EXT_PDCM | CPUID_EXT_HYPERVISOR,
        .ext2_features = CPUID_EXT2_LM | CPUID_EXT2_SYSCALL | CPUID_EXT2_NX,
        .ext3_features = CPUID_EXT3_LAHF_LM,
        .xlevel = 0x80000008,
        .model_id = "Common vmX processor",
    },
};

static struct x86_cpuid *_cpuid;

void init_cpuid(struct CPUState *cpu)
{
    _cpuid = &builtin_cpus[2]; /* core2duo */
}

void get_cpuid_func(struct CPUState *cpu, int func, int cnt, uint32_t *eax,
                    uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
   uint32_t h_rax, h_rbx, h_rcx, h_rdx;
   host_cpuid(func, cnt, &h_rax, &h_rbx, &h_rcx, &h_rdx);
   uint32_t apic_id = X86_CPU(cpu)->apic_id;


    *eax = *ebx = *ecx = *edx = 0;
    switch (func) {
    case 0:
        *eax = _cpuid->level;
        *ebx = _cpuid->vendor1;
        *edx = _cpuid->vendor2;
        *ecx = _cpuid->vendor3;
        break;
    case 1:
        *eax = h_rax;/*_cpuid->stepping | (_cpuid->model << 3) |
                       (_cpuid->family << 6); */
        *ebx = (apic_id << 24) | (h_rbx & 0x00ffffff);
        *ecx = h_rcx;
        *edx = h_rdx;

        if (cpu->nr_cores * cpu->nr_threads > 1) {
            *ebx |= (cpu->nr_cores * cpu->nr_threads) << 16;
            *edx |= 1 << 28;    /* Enable Hyper-Threading */
        }

        *ecx = *ecx & ~(CPUID_EXT_OSXSAVE | CPUID_EXT_MONITOR |
                        CPUID_EXT_X2APIC | CPUID_EXT_VMX |
                        CPUID_EXT_TSC_DEADLINE_TIMER | CPUID_EXT_TM2 |
                        CPUID_EXT_PCID | CPUID_EXT_EST | CPUID_EXT_SSE42 |
                        CPUID_EXT_SSE41);
        *ecx |= CPUID_EXT_HYPERVISOR;
        break;
    case 2:
        /* cache info: needed for Pentium Pro compatibility */
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 4:
        /* cache info: needed for Core compatibility */
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 5:
        /* mwait info: needed for Core compatibility */
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 6:
        /* Thermal and Power Leaf */
        *eax = 0;
        *ebx = 0;
        *ecx = 0;
        *edx = 0;
        break;
    case 7:
        *eax = h_rax;
        *ebx = h_rbx & ~(CPUID_7_0_EBX_AVX512F | CPUID_7_0_EBX_AVX512PF |
                        CPUID_7_0_EBX_AVX512ER | CPUID_7_0_EBX_AVX512CD |
                        CPUID_7_0_EBX_AVX512BW | CPUID_7_0_EBX_AVX512VL |
                        CPUID_7_0_EBX_MPX | CPUID_7_0_EBX_INVPCID);
        *ecx = h_rcx & ~(CPUID_7_0_ECX_AVX512BMI);
        *edx = h_rdx;
        break;
    case 9:
        /* Direct Cache Access Information Leaf */
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 0xA:
        /* Architectural Performance Monitoring Leaf */
        *eax = 0;
        *ebx = 0;
        *ecx = 0;
        *edx = 0;
        break;
    case 0xB:
        /* CPU Topology Leaf */
        *eax = 0;
        *ebx = 0;   /* Means that we don't support this leaf */
        *ecx = 0;
        *edx = 0;
        break;
    case 0xD:
        *eax = h_rax;
        if (!cnt) {
            *eax &= (XSTATE_FP_MASK | XSTATE_SSE_MASK | XSTATE_YMM_MASK);
        }
        if (1 == cnt) {
            *eax &= (CPUID_XSAVE_XSAVEOPT | CPUID_XSAVE_XSAVEC);
        }
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 0x80000000:
        *eax = _cpuid->xlevel;
        *ebx = _cpuid->vendor1;
        *edx = _cpuid->vendor2;
        *ecx = _cpuid->vendor3;
        break;
    case 0x80000001:
        *eax = h_rax;/*_cpuid->stepping | (_cpuid->model << 3) |
                       (_cpuid->family << 6);*/
        *ebx = 0;
        *ecx = _cpuid->ext3_features & h_rcx;
        *edx = _cpuid->ext2_features & h_rdx;
        break;
    case 0x80000002:
    case 0x80000003:
    case 0x80000004:
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 0x80000005:
        /* cache info (L1 cache) */
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 0x80000006:
        /* cache info (L2 cache) */
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = h_rcx;
        *edx = h_rdx;
        break;
    case 0x80000007:
        *eax = 0;
        *ebx = 0;
        *ecx = 0;
        *edx = 0;   /* Note - We disable invariant TSC (bit 8) in purpose */
        break;
    case 0x80000008:
        /* virtual & phys address size in low 2 bytes. */
        *eax = h_rax;
        *ebx = 0;
        *ecx = 0;
        *edx = 0;
        break;
    case 0x8000000A:
        *eax = 0;
        *ebx = 0;
        *ecx = 0;
        *edx = 0;
        break;
    case 0x80000019:
        *eax = h_rax;
        *ebx = h_rbx;
        *ecx = 0;
        *edx = 0;
    case 0xC0000000:
        *eax = _cpuid->xlevel2;
        *ebx = 0;
        *ecx = 0;
        *edx = 0;
        break;
    default:
        *eax = 0;
        *ebx = 0;
        *ecx = 0;
        *edx = 0;
        break;
    }
}
