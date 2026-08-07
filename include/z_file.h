#ifndef FILE_H
#define FILE_H

#include <z_heap.h>
#include <z_string.h>

bool z_write_file(const char *pathname, const char *format, ...);
bool z_append_file(const char *pathname, const char *format, ...);
bool z_scanf_file(const char *pathname, const char *format, ...);

// TODO: fix
// Z_Maybe_String_Array z_read_directory(Z_Heap *heap, const char *pathname);

#endif