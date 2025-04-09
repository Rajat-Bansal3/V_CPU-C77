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
