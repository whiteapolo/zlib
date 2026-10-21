#include <stdio.h>
#include "../all.h"
void main() {
    Z_Pool_Auto pool={0};
    Z_Hash_Table empty=z_hash_table_new(&pool,z_str_equal,z_str_hash);
    printf("empty=%zu fallback=%s\n",z_hash_table_size(&empty),(char*)z_hash_table_try_get(&empty,"none","fallback"));
    Z_Hash_Table h=z_hash_table_new_with_capacity(&pool,z_str_equal,z_str_hash,16);
    char *k1="one",*k2="two",*v1="1",*v2="2",*v3="uno";
    Z_Maybe_Pair old=z_hash_table_put(&h,k1,v1); z_hash_table_put(&h,k2,v2);
    printf("initial=%d size=%zu get=%s fallback=%s\n",old.ok,z_hash_table_size(&h),(char*)z_hash_table_get(&h,"one"),(char*)z_hash_table_try_get(&h,"missing","none"));
    old=z_hash_table_put(&h,k1,v3); printf("replaced=%s contains=%d\n",(char*)old.pair.value,z_hash_table_contains(&h,"one"));
    Z_Hash_Table_Iter it=z_hash_table_iter(&h); Z_Pair pair; size_t n=0; while(z_hash_table_iter_next(&it,&pair)) n++;
    Z_Pair_Array arr=z_hash_table_to_array(&pool,&h); printf("iter=%zu array=%zu\n",n,arr.length);
    Z_Maybe_Pair removed=z_hash_table_delete(&h,"two"); printf("deleted=%d size=%zu\n",removed.ok,z_hash_table_size(&h));
}
