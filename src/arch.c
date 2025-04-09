#include "helper/helper.h" 
#include "arch.h"

void cpu_init(CPU *cpu , size_t memory_size){
	cpu->memory = malloc(memory_size * sizeof(uint8_t));
	if(!cpu->memory){
		fprintf(stderr,"\nMemory allocation error");
		return;
	}
	cpu->memory_size = memory_size;
	memset(cpu->memory, 0 , memory_size );
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
			if(cpu->pc + 4 > cpu->memory_size) {
				cpu->pc = cpu->memory_size;
				cpu->flag = 1;
				break;
			}
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
			mem_addr_t addr = read_u32_le(&cpu->memory[cpu->pc + 1]);
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

