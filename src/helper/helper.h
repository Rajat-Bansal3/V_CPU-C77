#ifndef HELPER_H
#define HELPER_H

#include <unistd.h>  
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t* load_file(const char *path, size_t *size_out);
int file_exists(const char* path);
bool has_rjpc_extension(const char* path);

#endif
