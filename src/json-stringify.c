#include "../include/json-stringify.h"
#include "../include/string.h"

typedef struct {
    int level;
    int spaces_each_level;
} Z_Json_Indentation;

Z_Json_Indentation z_json_level_up(Z_Json_Indentation indentation);
void z_json_indentation_stringify(Z_Json_Indentation indentation, Z_String *out);
void z_json_array_stringify(const Z_Json_Value *value, Z_Json_Indentation indentation, Z_String *out);
void z_json_value_stringify(const Z_Json_Value *value, Z_Json_Indentation indentation, Z_String *out);
void z_json_object_stringify(const Z_Json_Value *json, Z_Json_Indentation indentation, Z_String *out);
void z_json_stringify(const Z_Json_Value *json, Z_String *out);

Z_Json_Indentation z_json_level_up(Z_Json_Indentation indentation)
{
    Z_Json_Indentation new = {
        .level = indentation.level + 1,
        .spaces_each_level = indentation.spaces_each_level,
    };

    return new;
}

void z_json_indentation_stringify(Z_Json_Indentation indentation, Z_String *out)
{
    int spaces = indentation.level * indentation.spaces_each_level;

    for (int i = 0; i < spaces; i++) {
        z_str_push_char(out, ' ');
    }
}

void z_json_array_stringify(const Z_Json_Value *value, Z_Json_Indentation indentation, Z_String *out)
{
    z_str_push_cstr(out, "[\n");
    Z_Json_Value_Array array = value->array;

    for (size_t i = 0; i < array.length; i++) {
        z_json_indentation_stringify(z_json_level_up(indentation), out);
        z_json_value_stringify(&array.ptr[i], z_json_level_up(indentation), out);

        if (i != array.length - 1) {
            z_str_push_char(out, ',');
        }

        z_str_push_char(out, '\n');
    }

    z_json_indentation_stringify(indentation, out);
    z_str_push_char(out, ']');
}

void z_json_value_stringify(const Z_Json_Value *value, Z_Json_Indentation indentation, Z_String *out)
{
    switch (value->type) {
        case Z_JSON_NUMBER:
            z_str_push(out, "%g", value->number);
            break;
        case Z_JSON_BOOLEAN:
            z_str_push(out, value->boolean ? "true" : "false");
            break;
        case Z_JSON_NULL:
            z_str_push(out, "null");
            break;
        case Z_JSON_STRING:
            z_str_push(out, "\"%s\"", value->string.ptr);
            break;
        case Z_JSON_ARRAY:
            z_json_array_stringify(value, indentation, out);
            break;
        case Z_JSON_OBJECT:
            z_json_object_stringify(value, indentation, out);
            break;
    }
}

void z_json_object_stringify(const Z_Json_Value *json, Z_Json_Indentation indentation, Z_String *out)
{
    Z_Pool_Auto pool = {0};

    if (z_hash_table_size(&json->object) == 0) {
        z_str_push_cstr(out, "{}");
        return;
    }

    Z_Pair pair;
    Z_Hash_Table_Iter iter = z_hash_table_iter(&json->object);

    size_t i = 1;

    z_str_push_cstr(out, "{\n");

    while (z_hash_table_iter_next(&iter, &pair)) {
        z_json_indentation_stringify(z_json_level_up(indentation), out);
        z_str_push(out, "\"%s\": ", (char *)pair.key);
        z_json_value_stringify(pair.value, z_json_level_up(indentation), out);

        if (i != z_hash_table_size(&json->object)) {
            z_str_push_char(out, ',');
        }

        z_str_push_char(out, '\n');
        i++;
    }

    z_json_indentation_stringify(indentation, out);
    z_str_push_char(out, '}');
}

void z_json_stringify(const Z_Json_Value *json, Z_String *out)
{
    Z_Json_Indentation indentation = {
        .level = 0,
        .spaces_each_level = 4,
    };

    z_json_value_stringify(json, indentation, out);
    z_str_push_char(out, '\n');
}
