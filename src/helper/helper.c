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
