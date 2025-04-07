#include "helper/helper.h" 
#include "arch.h"

void cpu_init(CPU *cpu){
	memset(cpu->memory , 0 , MEMORY_SIZE);
	memset(cpu->registers , 0 , sizeof(cpu->registers));
	cpu->pc = 0;
	cpu->flag = 0;
	cpu->registers[SP] = MEMORY_SIZE;
}
void cpu_load_program(CPU *cpu, const uint8_t *program, size_t size) {
    if (size > MEMORY_SIZE) {
        fprintf(stderr, "Program too fat (%zu > %d)\n", size, MEMORY_SIZE);
        exit(1);
    }
    if (size == 0) {
        fprintf(stderr, "Empty Bitxch\n");
        exit(1);
    }
    memcpy(cpu->memory, program, size);
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
		case MUL:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
			uint8_t r2 = cpu->memory[cpu->pc + 2];
			cpu->registers[r1] *= cpu->registers[r2];
			cpu->pc += 3;
			break;
			 }
		case DIV:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
                        uint8_t r2 = cpu->memory[cpu->pc + 2];
			if(cpu->registers[r2] == 0){
				printf("can't divide by 0"); 
				cpu->pc = MEMORY_SIZE;
				break;
			}
                        cpu->registers[r1] /= cpu->registers[r2];
                        cpu->pc += 3;
                        break;
			 }
		case MOD:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
			uint8_t val = cpu->memory[cpu->pc + 2];
			if(val == 0){
                                printf("can't calc modulo by 0"); 
                                cpu->pc = MEMORY_SIZE; 
                                break;
                        }
			cpu->registers[r1] %= val;
			cpu->pc += 3;
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
			uint8_t r2 = cpu->memory[cpu->pc + 2];
			if(cpu->registers[r2] > MEMORY_SIZE){
				perror("cant load from outside the memory");
				cpu->pc = MEMORY_SIZE;
				break;
			}
			cpu->registers[r1] = cpu->memory[cpu->registers[r2]];
			cpu->pc+=3;
			break;
			   }
		case STORER:{
			uint8_t r1 = cpu->memory[cpu->pc + 1];
                        uint8_t r2 = cpu->memory[cpu->pc + 2];
                        if(cpu->registers[r1] > MEMORY_SIZE){
                                perror("cant store outside the memory");
                                cpu->pc = MEMORY_SIZE;
                                break;
                        }
			cpu->memory[cpu->registers[r2]] = cpu->registers[r1];
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
			cpu->pc = MEMORY_SIZE;
			break;
				  }
		default:{
	            fprintf(stderr, "not recognised opcode 0x%02X at PC %u\n", opcode, cpu->pc);
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
    cpu_init(&cpu);

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

