#ifndef ARCH_H
#define ARCH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MEMORY_SIZE (1024 * 1024)
#define REGISTERS 8


typedef struct {
    uint8_t memory[MEMORY_SIZE]; 
    uint32_t registers[REGISTERS];       
    uint32_t pc;                 
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
#define HALT 0xFF

void cpu_init(CPU *cpu);
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

char* load_files(char* path);
int file_exists(const char* path);
bool has_rjpc_extension(const char* path);

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
    long size = ftell(f);
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
    	printf("alksdland");
	return access(path, F_OK) == 0;
}
bool has_rjpc_extension(const char* path) {
    	printf("alksdland");
    const char* dot = strrchr(path, '.');
    return dot && strcmp(dot, ".rjpc") == 0;
}
#include "helper/helper.h" 
#include "arch.h"

void cpu_init(CPU *cpu){
	memset(cpu->memory , 0 , MEMORY_SIZE);
	memset(cpu->registers , 0 , sizeof(cpu->registers));
	cpu->pc = 0;
	cpu->flag = 0;
}
void cpu_load_program(CPU *cpu, const uint8_t *program, size_t size) {
    if (size > MEMORY_SIZE) {
        fprintf(stderr, "Program too large (%zu > %d)\n", size, MEMORY_SIZE);
        exit(1);
    }
    if (size == 0) {
        fprintf(stderr, "Empty program\n");
        exit(1);
    }
    memcpy(cpu->memory, program, size);
    printf("Loaded %zu bytes\n", size);  // Debug print
    cpu->flag = 0;
}

void cpu_step(CPU *cpu){
	uint8_t opcode = cpu->memory[cpu->pc];

	switch (opcode){
		case NOP:{
			cpu->pc ++;
			break;
			 }
		case LOAD:{
			uint8_t reg = cpu->memory[cpu->pc + 1];
			uint8_t val = cpu->memory[cpu->pc + 2];
			cpu->registers[reg] = val;
			cpu->pc+=3;
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
			uint8_t addr = cpu->memory[cpu->pc + 1];
			cpu->pc = addr;
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
			uint8_t addr = cpu->memory[cpu->pc + 1];
			if(cpu->flag) cpu->pc = addr;
			else cpu->pc += 2;
			break;
			}
		case PRINT:{
			uint8_t reg = cpu->memory[cpu->pc + 1];
            		printf("R%d = %d\n", reg, cpu->registers[reg]);
            		cpu->pc += 2;
            		break;
			   }
		case HALT:{
			cpu->pc = MEMORY_SIZE;
			break;
				  }
		default:{
	            fprintf(stderr, "Unknown opcode 0x%02X at PC %u\n", opcode, cpu->pc);
        	    cpu->pc = MEMORY_SIZE;  
            	    break;
			}

	}

}
void cpu_run(CPU *cpu) {
    while (cpu->pc < MEMORY_SIZE) {
        uint8_t opcode = cpu->memory[cpu->pc];
        if (opcode == HALT) break;  
        cpu_step(cpu);
    }
}

int main (int argc, char *argv[]) {
    if(argc < 2 || argc > 2 ) {
        printf("No file path was provided.\n");
        return 0;
    }

    if (!file_exists(argv[1])) {
        fprintf(stderr, "Error: File does not exist.\n");
        return 1;
    }

    if (!has_rjpc_extension(argv[1])) {
        fprintf(stderr, "Error: File must have a .rjpc extension.\n");
        return 1;
    }

    CPU cpu;
    cpu_init(&cpu);

    size_t prog_size = 0;
    uint8_t *prog = load_file(argv[1], &prog_size);
    if (!prog) {
        fprintf(stderr, "Failed to load program file.\n");
        return 1;
    }

    printf("hello");
    cpu_load_program(&cpu, prog, prog_size);
    free(prog);
    cpu_run(&cpu);

    return 0;
}

