#pragma once
#ifndef ARCH_H
#define ARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEMORY_SIZE (1024 * 1024)

typedef uint32_t mem_addr_t;
typedef uint32_t reg_val_t;


#define REGISTERS 16 
#define SP  9

#define STACK_START MEMORY_SIZE - 1
#define STACK_END MEMORY_SIZE / 2


typedef struct {
    uint8_t* memory;
    size_t memory_size;
    reg_val_t registers[REGISTERS];       
    mem_addr_t pc;                 
    uint8_t flag;
} CPU;


#define NOP   0x00
#define LOAD  0x01  
#define ADD   0x02  
#define SUB   0x03  
#define JMP   0x04  
#define CMP   0x05  
#define JZ    0x06  
#define PRINT 0x07
#define MUL 0x08
#define DIV 0x09
#define MOD 0x0A
#define INC 0x0B
#define DEC 0x0C
#define LOADR 0x0D
#define STORER 0x0E
#define CALL 0x0F
#define RET 0x10
#define PUSH 0x11
#define POP 0x12
#define HALT 0xFF

void cpu_init(CPU *cpu , size_t memory_size);
void cpu_load_program(CPU *cpu, const uint8_t *program, size_t size);
void cpu_step(CPU *cpu);
void cpu_run(CPU *cpu);



#endif
