#ifndef Z_JSON_H
#define Z_JSON_H

#include "../include/array.h"
#include "../include/string.h"
#include "../include/hash-table.h"

typedef enum {
    Z_JSON_STRING,
    Z_JSON_NUMBER,
    Z_JSON_BOOLEAN,
    Z_JSON_NULL,
    Z_JSON_OBJECT,
    Z_JSON_ARRAY,
} Z_Json_Value_Type;

typedef struct Z_Json_Value Z_Json_Value;

Z_DEFINE_ARRAY(Z_Json_Value_Array, Z_Json_Value);

struct Z_Json_Value {
    Z_Json_Value_Type type;
    union {
        Z_String string;
        double number;
        bool boolean;
        Z_Hash_Table object;
        Z_Json_Value_Array array;
    };
};

typedef struct {
    bool had_error;
    Z_Json_Value json;
    char *error;
} Z_Json_Parse_Result;

Z_Json_Parse_Result z_json_parse(Z_Pool *pool, Z_String_View document);
Z_Json_Parse_Result z_json_parse_file(Z_Pool *pool, const char *path);
Z_Json_Value z_json_get(const Z_Json_Value *json, const char *path);
void z_json_stringify(const Z_Json_Value *json, Z_String *out);

#endif
