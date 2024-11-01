/*
 BSD 3-Clause License
 
 Copyright (c) 2024, k4m1
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
 
 3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __CPU_INST_COMMON_H__
#define __CPU_INST_COMMON_H__

#include <stdint.h>
#include <sys/io.h>

/* Command register 0 settings 
 *
 * @member CR0_PE -- Paging enabled
 * @member CR0_MP -- Monitor co-process
 * @member CR0_EM -- x87 FPU emulation
 * @member CR0_TS -- Task switched
 * @member CR0_ET -- Extension type
 * @member CR0_NE -- Numeric Error
 * @member CR0_WP -- Write Protect
 * @member CR0_NW -- Not write through
 * @member CR0_CD -- Cache Disabled
 * @member CR0_PG -- Paging
 */
enum CR0_SETTING {
    CR0_PE = ( 1 << 0 ),
    CR0_MP = ( 1 << 1 ),
    CR0_EM = ( 1 << 2 ),
    CR0_TS = ( 1 << 3 ),
    CR0_ET = ( 1 << 4 ),
    CR0_NE = ( 1 << 5 ),
    CR0_WP = ( 1 << 16 ),
    CR0_AM = ( 1 << 18 ),
    CR0_NW = ( 1 << 29 ),
    CR0_CD = ( 1 << 30 ),
    CR0_PG = ( 1 << 31 )
};

/* Helper structure for tracking cpu state in 16-bit mode. 
 * The register order matches pusha/popa when we have downwards growing stack.
 *
 */
typedef struct {
    uint16_t di;
    uint16_t si;
    uint16_t bp;
    uint16_t sp;
    uint16_t bx;
    uint16_t dx;
    uint16_t cx;
    uint16_t ax;
} cpu_state_16b;

/* Helper structure for tracking cpu state in 32-bit mode. 
 * The register order matches pusha/popa when we have downwards growing stack.
 *
 */
typedef struct {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} cpu_state_32b;

static inline uint32_t __attribute__((always_inline)) get_cr0(void) {
    uint32_t r;
    asm volatile("mov   %0, cr0":"=r"(r));
    return r;
}

static inline void __attribute__((always_inline)) set_cr0(uint32_t v) {
    asm volatile("mov   cr0, %0"::"r"(v));
}

static inline uint32_t __attribute__((always_inline)) get_cr3(void) {
    uint32_t r;
    asm volatile("mov   %0, cr3":"=r"(r));
    return r;
}

static inline void __attribute__((always_inline)) set_cr3(uint32_t v) {
    asm volatile("mov   cr3, %0"::"r"(v));
}

static inline uint32_t __attribute__((always_inline)) get_cr4(void) {
    uint32_t r;
    asm volatile("mov   %0, cr4":"=r"(r));
    return r;
}

static inline void __attribute__((always_inline)) set_cr4(uint32_t v) {
    asm volatile("mov   cr4, %0"::"r"(v));
}

static inline uint32_t __attribute__((always_inline)) rdmsr(uint32_t msr) {
    uint32_t ret;
    asm volatile(
            "mov   ecx, %0;"
            "rdmsr;"
            "mov   %1, eax;"
            :"=r"(ret)
            :"r"(msr)
            :"eax","ecx"
    );
    return ret;
}

static inline void __attribute__((always_inline)) wrmsr(uint32_t msr, uint32_t val) {
    asm volatile(
            "mov    ecx, %0;"
            "mov    eax, %1;"
            "wrmsr;"
            ::"r"(msr),"r"(val)
            :"eax","ecx"
    );
}

/* Read gdtr to given 6-byte location
 *
 * @param void *dst -- where to read gdtr to
 */
static inline void __attribute__((always_inline)) read_gdtr(void *dst) {
    asm volatile("sgdt %0" : "=m"(dst));
}

/* Write gdtr fro mgiven 6-byte location
 *
 * @param void *src -- where to read gdtr from
 */
static inline void __attribute((always_inline)) write_gdtr(void *src) {
    asm volatile("lgdt %0" :: "m"(src));
}


/* Read code segment register
*
* @return uint16_t cs
*/
static inline uint16_t __attribute__((always_inline)) read_cs() {
   uint16_t ret;
   asm volatile("mov %0, cs" : "=r"(ret));
   return ret;
}

/* Read data segment register
*
* @return uint16_t ds
*/
static inline uint16_t __attribute__((always_inline)) read_ds() {
   uint16_t ret;
   asm volatile("mov %0, ds" : "=r"(ret));
   return ret;
}

/* Read extra segment register
*
* @return uint16_t es
*/
static inline uint16_t __attribute__((always_inline)) read_es() {
   uint16_t ret;
   asm volatile("mov %0, es" : "=r"(ret));
   return ret;
}

/* Read stack segment register
*
* @return uint16_t ss
*/
static inline uint16_t __attribute__((always_inline)) read_ss() {
   uint16_t ret;
   asm volatile("mov %0, ss" : "=r"(ret));
   return ret;
}

/* Read general purpose F segment register
*
* @return uint16_t fs
*/
static inline uint16_t __attribute__((always_inline)) read_fs() {
   uint16_t ret;
   asm volatile("mov %0, fs" : "=r"(ret));
   return ret;
}

/* Read general purpose G segment register
*
* @return uint16_t gs
*/
static inline uint16_t __attribute__((always_inline)) read_gs() {
   uint16_t ret;
   asm volatile("mov %0, gs" : "=r"(ret));
   return ret;
}

/* Write data segment register
*
* @param uint16_t ds -- new data segment register value
*/
static inline void __attribute__((always_inline)) write_ds(uint32_t ds) {
   asm volatile("mov ds, %0" :: "r"(ds));
}

/* Write extra segment register
*
* @param uint16_t es -- new data segment register value
*/
static inline void __attribute__((always_inline)) write_es(uint32_t es) {
   asm volatile("mov es, %0" :: "r"(es));
}

/* Write stack segment register
*
* @param uint16_t ss -- new data segment register value
*/
static inline void __attribute__((always_inline)) write_ss(uint32_t ss) {
   asm volatile("mov ss, %0" :: "r"(ss));
}

/* Write general purpose F segment register
*
* @param uint16_t fs -- new data segment register value
*/
static inline void __attribute__((always_inline)) write_fs(uint32_t fs) {
   asm volatile("mov fs, %0" :: "r"(fs));
}

/* Write general purpose G segment register
*
* @param uint16_t gs -- new data segment register value
*/
static inline void __attribute__((always_inline)) write_gs(uint32_t gs) {
   asm volatile("mov gs, %0" :: "r"(gs));
}


#endif // __CPU_INST_COMMON_H__
