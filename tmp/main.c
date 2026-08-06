#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "../include/z_time.h"
#include "../include/z_string.h"
#include "../include/z_file.h"
#include "../include/z_heap.h"
#include "../include/z_hash_table.h"

#define ASSERT_EQ(expected, actual)                                   \
    do {                                                              \
        int _e = (expected);                                          \
        int _a = (actual);                                            \
        if (_e != _a) {                                               \
            fprintf(stderr,                                           \
                "ASSERT FAILED: expected=%d actual=%d (%s:%d)\n",     \
                _e, _a, __FILE__, __LINE__);                          \
            exit(1);                                                  \
        }                                                             \
    } while (0)

#define TEST_LIKE(str, pattern, expected) \
    ASSERT_EQ(expected, z_sv_like(z_sv(str), z_sv(pattern)))

void test_like(void)
{
    /* Your original tests */
    TEST_LIKE("11111111112222222221112a", "%%%%%%%%%%a", 1);
    TEST_LIKE("abc", "a_c", 1);
    TEST_LIKE("abc", "a_d", 0);
    TEST_LIKE("abcdef", "a%f", 1);
    TEST_LIKE("abcdef", "%cd%", 1);
    TEST_LIKE("abcdef", "%cg%", 0);
    TEST_LIKE("abc", "___", 1);
    TEST_LIKE("abc", "____", 0);
    TEST_LIKE("abc", "%", 1);
    TEST_LIKE("", "%", 1);
    TEST_LIKE("", "_", 0);

    /* Additional important cases */

    /* exact match */
    TEST_LIKE("abc", "abc", 1);
    TEST_LIKE("abc", "abcd", 0);

    /* leading % */
    TEST_LIKE("abcdef", "%def", 1);
    TEST_LIKE("abcdef", "%deg", 0);

    /* trailing % */
    TEST_LIKE("abcdef", "abc%", 1);
    TEST_LIKE("abcdef", "abd%", 0);

    /* multiple % */
    TEST_LIKE("abcdef", "%b%d%f", 1);
    TEST_LIKE("abcdef", "%b%z%f", 0);

    /* consecutive % should behave like one */
    TEST_LIKE("abcdef", "a%%f", 1);
    TEST_LIKE("abcdef", "a%%%f", 1);

    /* _ with % interaction */
    TEST_LIKE("abcdef", "a_c_e_", 1);
    TEST_LIKE("abcdef", "a_c_e", 0);

    /* only underscores */
    TEST_LIKE("abcde", "_____", 1);
    TEST_LIKE("abcde", "______", 0);

    /* empty string exact */
    TEST_LIKE("", "", 1);

    /* tricky backtracking cases */
    TEST_LIKE("aaaab", "a%b", 1);
    TEST_LIKE("aaaab", "a%ab", 1);
    TEST_LIKE("aaaab", "a%ac", 0);

    printf("All LIKE tests passed.\n");
}

int main(void)
{
        Z_Heap_Auto heap = {0};
        test_like();


        // Z_Hash_Table ht = z_hash_table_new(&heap, z_str_equal, z_str_hash);
        // z_hash_table_put(&ht, "computer", "bye");

        // printf("%s\n", (char*)z_hash_table_try_get(&ht, "omputer", "Not found"));

        return 0;
}
