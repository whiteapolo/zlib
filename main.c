#include <stdio.h>
#include <assert.h>
#include "zlib.h"

int main(void)
{
  Z_Dictionary *dictionary = z_dictionary_new();
  z_dictionary_put(dictionary, "Itay", Z_HEAP_ALLOC(20, int), free);

  int *value = z_dictionary_get(dictionary, "Itay");
  if (value) {
    printf("%d\n", *value);
  }


  z_dictionary_free(dictionary, free);
}
