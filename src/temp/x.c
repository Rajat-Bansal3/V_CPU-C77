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
#ifndef HELPER_H
#define HELPER_H

#include <unistd.h>  
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../arch.h"

uint8_t* load_file(const char *path, size_t *size_out);
int file_exists(const char* path);
bool has_rjpc_extension(const char* path);
uint32_t read_u32_le(uint8_t *ptr);
uint32_t read_mem_addr(const CPU *cpu, mem_addr_t addr);
void write_mem_addr(uint8_t *mem, reg_val_t val);

#endif
#include "helper.h"
uint8_t* load_file(const char *path, size_t *size_out) {
    if (!path) return NULL;

    FILE *f = fopen(path, "rb");  
    if (!f) {
        printf("null");
        perror("Failed to open file");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    rewind(f);

    if (size <= 0) {
        printf("null");
	    fclose(f);
        return NULL;
    }
	
    uint8_t *buffer = malloc(size);
    if (!buffer) {
        printf("null");
        perror("Failed to allocate memory");
        fclose(f);
        return NULL;
    }

    size_t read = fread(buffer, 1, size, f);
    fclose(f);
	
    if (read != size) {
        printf("null");
        perror("Failed to read full file");
        free(buffer);
        return NULL;
    }
	
    *size_out = size;
    return buffer;
}


int file_exists(const char* path) {
	return access(path, F_OK) == 0;
}
bool has_rjpc_extension(const char* path) {
    const char* dot = strrchr(path, '.');
    return dot && strcmp(dot, ".rjpc") == 0;
}
uint32_t read_u32_le(uint8_t *ptr) {
    return ((uint32_t)ptr[0]) |
           ((uint32_t)ptr[1] << 8) |
           ((uint32_t)ptr[2] << 16) |
           ((uint32_t)ptr[3] << 24);
}
uint32_t read_mem_addr(const CPU *cpu, mem_addr_t addr) {
    return cpu->memory[addr] |
          (cpu->memory[addr + 1] << 8) |
          (cpu->memory[addr + 2] << 16) |
          (cpu->memory[addr + 3] << 24);
}
void write_mem_addr(uint8_t *mem, reg_val_t val) {
    mem[0] = (val >> 0) & 0xFF;
    mem[1] = (val >> 8) & 0xFF;
    mem[2] = (val >> 16) & 0xFF;
    mem[3] = (val >> 24) & 0xFF;
}
#include "helper/helper.h" 
#include "arch.h"

void cpu_init(CPU *cpu , size_t memory_size){
	cpu->memory = malloc(memory_size * sizeof(uint8_t));
	if(!cpu->memory){
		fprintf(stderr,"\nMemory allocation error");
		return;
	}
	cpu->memory_size = memory_size;
	memset(cpu->memory, 0 , memory_size * sizeof(mem_addr_t));
	memset(cpu->registers , 0 , sizeof(cpu->registers));
	cpu->pc = 0;
	cpu->flag = 0;
	cpu->registers[SP] = memory_size - 1;
}
void cpu_load_program(CPU *cpu, const uint8_t *program, size_t size) {
    if (size > cpu->memory_size) {
        fprintf(stderr, "Program too fat (%zu > %zu)\n", size, cpu->memory_size);
        exit(1);
    }
    if (size == 0) {
        fprintf(stderr, "Empty Bitxch\n");
        exit(1);
    }
    cpu->registers[SP] = cpu->memory_size - 1;
    memcpy(cpu->memory, program, size);
    cpu->flag = 0;
}

void cpu_step(CPU *cpu){
	if (cpu->pc >= cpu->memory_size) {
        	cpu->pc = cpu->memory_size;
        	return;
    	}

	uint8_t opcode = cpu->memory[cpu->pc];

	switch (opcode){
		case NOP:{
			cpu->pc ++;
			break;
			 }
		case LOAD:{
			if (cpu->pc + 5 >= cpu->memory_size) {
                		cpu->flag = 1;
                		cpu->pc = cpu->memory_size;
                		break;
            		}

			uint8_t reg = cpu->memory[cpu->pc + 1];
			reg_val_t val = read_u32_le(&cpu->memory[cpu->pc + 2]);
			cpu->registers[reg] = val;
			cpu->pc+=6;
			break;
			  }
		case ADD:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
			uint8_t r2 = cpu->memory[cpu->pc + 2];
			cpu->registers[r1] += cpu->registers[r2];
			cpu->pc+= 3;
			break;
			 }
		case SUB:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
                        uint8_t r2 = cpu->memory[cpu->pc + 2];
                        cpu->registers[r1] -= cpu->registers[r2]; 
                        cpu->pc+= 3;
                        break;
			 }
		case JMP:{
			mem_addr_t addr = read_mem_addr(cpu,cpu->memory[cpu->pc + 1]);
			if (addr >= cpu->memory_size) {
        			cpu->flag = 1;
        			cpu->pc = cpu->memory_size;
    			} else {
        			cpu->pc = addr;
    			}
			break;
			 }
		case CMP:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
                        uint8_t r2 = cpu->memory[cpu->pc + 2];
			cpu->flag = (cpu->registers[r1] == cpu->registers[r2]) ? 1 : 0;
			cpu->pc+= 3;
			break;
			 }
		case JZ:{
			if (cpu->pc + 4 >= cpu->memory_size) {
                		cpu->flag = 1;
                		cpu->pc = cpu->memory_size;
                		break;
            		}
			mem_addr_t addr = read_mem_addr(cpu,cpu->memory[cpu->pc + 1]);
			if(cpu->flag) cpu->pc = addr;
			else cpu->pc += 5;
			break;
			}
		case PRINT:{
			uint8_t reg = cpu->memory[cpu->pc + 1];
            		printf("R%d = %d\n", reg, cpu->registers[reg]);
            		cpu->pc += 2;
            		break;
			   }
		case MUL:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
			uint8_t r2 = cpu->memory[cpu->pc + 2];
			uint64_t res = (uint32_t)cpu->registers[r1] * (uint32_t)cpu->registers[r2];
			if( res > UINT32_MAX) cpu->flag = 1;else cpu->flag = 0;
			cpu->registers[r1] = (reg_val_t)res;
			cpu->pc += 3;
			break;
			 }
		case DIV:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
                        uint8_t r2 = cpu->memory[cpu->pc + 2];
			if(cpu->registers[r2] == 0){ 
				cpu->flag = 1;
				cpu->pc = cpu->memory_size;
				break;
			}
                        cpu->registers[r1] /= cpu->registers[r2];
                        cpu->pc += 3;
                        break;
			 }
		case MOD:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
			reg_val_t val = read_u32_le(&cpu->memory[cpu->pc + 2]);
			if(val == 0){
                                cpu->flag = 1; 
                                cpu->pc = cpu->memory_size;
				break;
                        }
			cpu->registers[r1] %= val;
			cpu->pc += 6;
			break;
			 }
		case INC:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
                        cpu->registers[r1]++;
			cpu->pc += 2;
                        break;
			 }
		case DEC:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];  
                        cpu->registers[r1]--;             
                        cpu->pc += 2;
                        break;
			 }
		case LOADR:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
			uint8_t r2 = read_mem_addr(cpu,cpu->memory[cpu->pc + 2]);
			reg_val_t addr = cpu->registers[r2];
			if(addr + sizeof(reg_val_t) > cpu->memory_size){
				cpu->flag = 1;
				cpu->pc = cpu->memory_size;
				break;
			}
			cpu->registers[r1] = read_u32_le(&cpu->memory[addr]);
			cpu->pc+=3;
			break;
			   }
		case STORER:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
                        uint8_t r2 = cpu->memory[cpu->pc + 2];
                        reg_val_t val = cpu->registers[r1];
			mem_addr_t addr = cpu->registers[r2];
			if(addr + sizeof(reg_val_t) > cpu->memory_size){
				cpu->flag = 1;
                                cpu->pc = cpu->memory_size;
                                break;
                        }
			write_mem_addr(&cpu->memory[addr], val);
                        cpu->pc+=3;
                        break;
			    }
		case PUSH:{
			uint8_t reg = cpu->memory[cpu->pc + 1];
			
			  }
		case POP:{}
		case CALL:{}
		case RET:{}
		case HALT:{
			cpu->pc = cpu->memory_size;
			break;
				  }
		default:{
	            fprintf(stderr, "not recognised opcode 0x%02X at PC %u\n", opcode, cpu->pc);
        	    cpu->pc = cpu->memory_size;  
            	    break;
			}

	}

}
void cpu_run(CPU *cpu) {
    while (cpu->pc < cpu->memory_size) {
        uint8_t opcode = cpu->memory[cpu->pc];
        if (opcode == HALT) break;  
        cpu_step(cpu);
    }
}

int main (int argc, char *argv[]) {
    if(argc < 2 || argc > 2 ) {
        printf("No file path was given\n");
        return 0;
    }

    if (!file_exists(argv[1])) {
        fprintf(stderr, "does not exist.\n");
        return 1;
    }

    if (!has_rjpc_extension(argv[1])) {
        fprintf(stderr, "must have a .rjpc extension\n");
        return 1;
    }

    CPU cpu;
    cpu_init(&cpu , MEMORY_SIZE);

    size_t prog_size = 0;
    uint8_t *prog = load_file(argv[1], &prog_size);
    if (!prog) {
        fprintf(stderr, "Failed to load file.\n");
        return 1;
    }

    cpu_load_program(&cpu, prog, prog_size);
    free(prog);
    cpu_run(&cpu);

    return 0;
}

