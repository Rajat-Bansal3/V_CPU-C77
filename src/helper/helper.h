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
