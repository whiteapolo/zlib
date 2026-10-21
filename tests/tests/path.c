#include <stdio.h>
#include <stdlib.h>
#include "../all.h"
void main() {
    setenv("HOME", "/tmp/zlib-path-home", 1);
    Z_Pool_Auto pool={0}; Z_String expanded=z_str_new(&pool,""); Z_String compressed=z_str_new(&pool,"");
    const char *home=z_env_get("HOME","."); Z_String_View homeview=z_sv(home);
    Z_String_View tilde=z_sv("~/docs"); bool ex=z_path_expand_tilde(tilde,&expanded);
    bool co=z_path_compress_tilde(z_sv(&expanded),&compressed);
    printf("expand=%d suffix=%.*s compress=%d result=",ex,5,expanded.ptr+homeview.length,co); z_sv_println(z_sv(&compressed));
    printf("directory=%d file=%d missing=%d\n",z_path_is_directory("."),z_path_is_regular_file("tests/path.c"),z_path_is_directory("/no/such/zlib-path"));
    Z_String unchanged=z_str_new(&pool,""); printf("non-tilde=%d ",z_path_expand_tilde(z_sv("relative"),&unchanged)); z_sv_println(z_sv(&unchanged));
    Z_String unchanged2=z_str_new(&pool,""); printf("outside-home=%d ",z_path_compress_tilde(z_sv("/other/place"),&unchanged2)); z_sv_println(z_sv(&unchanged2));
}
