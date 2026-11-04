#include "../include/json.h"
#include "../include/scanner.h"
#include "../include/time.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

Z_Json_Value Z_JSON_DUMMY = {0};

typedef struct {
    Z_Pool *pool;
    bool had_error;
    Z_Scanner scanner;
    char *error;
} Z_Json_Parser_State;

Z_Json_Value z_json_parse_value(Z_Json_Parser_State *parser);
Z_Json_Value z_json_create_boolean(bool boolean);
Z_Json_Value z_json_create_null();
Z_Json_Value z_json_create_string(Z_String string);
Z_Json_Value z_json_create_number(double number);
Z_Json_Value z_json_create_array(Z_Json_Value_Array array);
Z_Json_Value z_json_create_object(Z_Hash_Table object);
Z_Json_Value z_json_error(Z_Json_Parser_State *parser, const char *message);
Z_Json_Value z_json_parse_true(Z_Json_Parser_State *parser);
Z_Json_Value z_json_parse_false(Z_Json_Parser_State *parser);
Z_Json_Value z_json_parse_null(Z_Json_Parser_State *parser);
Z_Json_Value z_json_parse_string(Z_Json_Parser_State *parser);
bool z_json_parse_integer(Z_Json_Parser_State *parser, int *number);
double build_number_from_parts(int sign, int number, int fraction,
                               int exponent_sign, int exponent);
Z_Json_Value z_json_parse_number(Z_Json_Parser_State *parser);
Z_Json_Value z_json_parse_array(Z_Json_Parser_State *parser);
void z_json_parse_object_entry(Z_Json_Parser_State *parser, Z_Hash_Table *object);
Z_Json_Value z_json_parse_object(Z_Json_Parser_State *parser);
Z_Json_Value z_json_parse_value(Z_Json_Parser_State *parser);

Z_Json_Value z_json_create_boolean(bool boolean)
{
    Z_Json_Value value = {
        .type = Z_JSON_BOOLEAN,
        .boolean = boolean,
    };

    return value;
}

Z_Json_Value z_json_create_null()
{
    Z_Json_Value value = {
        .type = Z_JSON_NULL,
    };

    return value;
}

Z_Json_Value z_json_create_string(Z_String string)
{
    Z_Json_Value value = {
        .type = Z_JSON_STRING,
        .string = string,
    };

    return value;
}

Z_Json_Value z_json_create_number(double number)
{
    Z_Json_Value value = {
        .type = Z_JSON_NUMBER,
        .number = number,
    };

    return value;
}

Z_Json_Value z_json_create_array(Z_Json_Value_Array array)
{
    Z_Json_Value value = {
        .type = Z_JSON_ARRAY,
        .array = array,
    };

    return value;
}

Z_Json_Value z_json_create_object(Z_Hash_Table object)
{
    Z_Json_Value value = {
        .type = Z_JSON_OBJECT,
        .object = object,
    };

    return value;
}

Z_Json_Value z_json_error(Z_Json_Parser_State *parser, const char *message)
{
    parser->had_error = true;

    Z_String error = z_str_new(
        parser->pool,
        "%zu:%zu: %s",
        parser->scanner.line,
        parser->scanner.column,
        message
    );

    parser->error = error.ptr;
    return Z_JSON_DUMMY;
}

Z_Json_Value z_json_parse_true(Z_Json_Parser_State *parser)
{
    if (z_scanner_match_string(&parser->scanner, z_sv("true"))) {
        return z_json_create_boolean(true);
    }

    return z_json_error(parser, "Expected true");
}

Z_Json_Value z_json_parse_false(Z_Json_Parser_State *parser)
{
    if (z_scanner_match_string(&parser->scanner, z_sv("false"))) {
        return z_json_create_boolean(false);
    }

    return z_json_error(parser, "Expected false");
}

Z_Json_Value z_json_parse_null(Z_Json_Parser_State *parser)
{
    if (z_scanner_match_string(&parser->scanner, z_sv("null"))) {
        return z_json_create_null();
    }

    return z_json_error(parser, "Expected null");
}

Z_Json_Value z_json_parse_string(Z_Json_Parser_State *parser)
{
    if (!z_scanner_match(&parser->scanner, '"')) {
        return z_json_error(parser, "Expected string");
    }

    Z_String string = z_str_new(parser->pool, "");

    while (
        !z_scanner_is_at_end(&parser->scanner)
        && z_scanner_peek(&parser->scanner) != '"'
        && z_scanner_peek(&parser->scanner) != '\n'
    ) {
        if (z_scanner_match(&parser->scanner, '\\')) {
            if (z_scanner_is_at_end(&parser->scanner)) {
                return z_json_error(parser, "Expected end of string, got end of file");
            } else {
                z_str_push_back_char(&string, z_scanner_advance(&parser->scanner));
            }
        } else {
            z_str_push_back_char(&string, z_scanner_advance(&parser->scanner));
        }
    }

    if (z_scanner_match(&parser->scanner, '\n')) {
        return z_json_error(parser, "Expected end of string, got new line");
    }

    if (!z_scanner_match(&parser->scanner, '"')) {
        return z_json_error(parser, "Expected end of string, got end of file");
    }

    return z_json_create_string(string);
}

bool z_json_parse_integer(Z_Json_Parser_State *parser, int *number)
{
    if (!isdigit(z_scanner_peek(&parser->scanner))) {
        return false;
    }

    *number = 0;

    while (!z_scanner_is_at_end(&parser->scanner) && isdigit(z_scanner_peek(&parser->scanner))) {
        *number *= 10;
        *number += z_scanner_advance(&parser->scanner) - '0';
    }

    return true;
}

double build_number_from_parts(int sign, int number, int fraction,
                               int exponent_sign, int exponent)
{
    return ((sign * (double)number) + (fraction / pow(10, log10(fraction) + 1))) * pow(10, exponent_sign * exponent);
}

Z_Json_Value z_json_parse_number(Z_Json_Parser_State *parser)
{
    int sign = 1;
    int number = 0;
    int fraction = 0;
    int exponent = 1;
    int exponent_sign = 1;

    if (z_scanner_match(&parser->scanner, '-')) {
        sign = -1;
    }

    if (!z_json_parse_integer(parser, &number)) {
        return z_json_error(parser, "Expected number");
    }

    if (z_scanner_match(&parser->scanner, '.')) {
        if (!z_json_parse_integer(parser, &fraction)) {
            return z_json_error(parser, "Expected number");
        }
    }

    if (z_scanner_match(&parser->scanner, 'e') || z_scanner_match(&parser->scanner, 'E')) {
        if (z_scanner_match(&parser->scanner, '-')) {
            exponent_sign = -1;
        } else {
            z_scanner_match(&parser->scanner, '+');
        }

        if (!z_json_parse_integer(parser, &exponent)) {
            return z_json_error(parser, "Expected number");
        }
    }

    return z_json_create_number(build_number_from_parts(sign, number, fraction, exponent_sign, exponent));
}

Z_Json_Value z_json_parse_array(Z_Json_Parser_State *parser)
{
    if (parser->had_error) {
        return Z_JSON_DUMMY;
    }

    z_scanner_skip_spaces(&parser->scanner);

    if (!z_scanner_match(&parser->scanner, '[')) {
        return z_json_error(parser, "Expected '['");
    }

    Z_Json_Value_Array array = z_array_new(parser->pool, Z_Json_Value_Array);

    if (z_scanner_match(&parser->scanner, ']')) {
        return z_json_create_array(array);
    }

    do {
        z_scanner_skip_spaces(&parser->scanner);
        Z_Json_Value value = z_json_parse_value(parser);

        if (parser->had_error) {
            return Z_JSON_DUMMY;
        }

        z_array_push_back(&array, value);
        z_scanner_skip_spaces(&parser->scanner);
    } while (z_scanner_match(&parser->scanner, ','));

    if (!z_scanner_match(&parser->scanner, ']')) {
        return z_json_error(parser, "Expected ']'");
    }

    return z_json_create_array(array);
}

void z_json_parse_object_entry(Z_Json_Parser_State *parser, Z_Hash_Table *object)
{
    z_scanner_skip_spaces(&parser->scanner);
    Z_Json_Value key = z_json_parse_string(parser);

    if (parser->had_error) {
        return;
    }

    z_scanner_skip_spaces(&parser->scanner);

    if (!z_scanner_match(&parser->scanner, ':')) {
        z_json_error(parser, "Expected ':'");
        return;
    }

    z_scanner_skip_spaces(&parser->scanner);

    Z_Json_Value value = z_json_parse_value(parser);

    if (parser->had_error) {
        return;
    }

    z_hash_table_put(object, key.string.ptr, z_pool_memory_duplicate(parser->pool, &value, sizeof(Z_Json_Value)));
}

Z_Json_Value z_json_parse_object(Z_Json_Parser_State *parser)
{
    if (parser->had_error) {
        return Z_JSON_DUMMY;
    }

    z_scanner_skip_spaces(&parser->scanner);

    if (!z_scanner_match(&parser->scanner, '{')) {
        return z_json_error(parser, "Expected '{'");
    }

    Z_Hash_Table object = z_hash_table_new(parser->pool, z_str_equal, z_str_hash);

    if (z_scanner_match(&parser->scanner, '}')) {
        return z_json_create_object(object);
    }

    do {
        z_json_parse_object_entry(parser, &object);

        if (parser->had_error) {
            return Z_JSON_DUMMY;
        }

        z_scanner_skip_spaces(&parser->scanner);
    } while (z_scanner_match(&parser->scanner, ','));

    if (!z_scanner_match(&parser->scanner, '}')) {
        return z_json_error(parser, "Expected '}'");
    }

    return z_json_create_object(object);
}

Z_Json_Value z_json_parse_value(Z_Json_Parser_State *parser)
{
    z_scanner_skip_spaces(&parser->scanner);

    if (parser->had_error) {
        return Z_JSON_DUMMY;
    }

    if (z_scanner_is_at_end(&parser->scanner)) {
        return z_json_error(parser, "Expected expression, got end of file");
    }

    switch (z_scanner_peek(&parser->scanner)) {
        case '"': return z_json_parse_string(parser);
        case '[': return z_json_parse_array(parser);
        case '{': return z_json_parse_object(parser);

        case 't': return z_json_parse_true(parser);
        case 'f': return z_json_parse_false(parser);
        case 'n': return z_json_parse_null(parser);

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '.': case 'e': case '-': case '+':
            return z_json_parse_number(parser);

        default: return z_json_error(parser, "Expected: string | number | boolean | null | array | object");
    }
}

Z_Json_Parse_Result z_json_parse(Z_Pool *pool, Z_String_View document)
{
    Z_Json_Parser_State parser = {
        .pool = pool,
        .had_error = false,
        .scanner = z_scanner_new(document),
    };

    Z_Json_Value json = z_json_parse_value(&parser);

    Z_Json_Parse_Result result = {
        .had_error = parser.had_error,
        .error = parser.error,
        .json = json,
    };

    return result;
}

Z_Json_Parse_Result z_json_parse_file(Z_Pool *pool, const char *pathname)
{
    Z_Pool_Auto scratch = {0};
    Z_String document = z_str_new(&scratch, "");

    if (!z_str_push_back_file(&document, pathname)) {

        Z_Json_Parse_Result result = {
            .had_error = true,
            .error = z_str_new(pool, "Cannot read file: '%s': %s", pathname, strerror(errno)).ptr,
        };

        return result;
    }

    return z_json_parse(pool, z_sv(document));
}
