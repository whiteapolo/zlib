#include <stdio.h>
#include "../all.h"
void main() {
    const char *p="/tmp/zlib_string_module_file_test";
    FILE *fixture=fopen(p,"w"); fputs("17 hello\nsecond line\n",fixture); fclose(fixture);
    int number=0; char word[32]={0};
    printf("scanf=%d ", z_file_scanf(p,"%d %31s",&number,word)); printf("%d %s\n",number,word);
    FILE *fp=fopen(p,"r"); Z_Pool_Auto pool={0}; Z_String line=z_str_new(&pool,"");
    printf("line-bytes=%zu ",z_file_read_line(fp,&line)); z_sv_print(z_sv(&line)); fclose(fp);
    printf("write=%d append=%d\n",z_file_write("/tmp/zlib_file_write_test","write"),z_file_append("/tmp/zlib_file_append_test","append"));
    char *args[]={"printf","pipe-ok",NULL}; Z_Piped_Process proc=z_pipe_process(args,Z_File_Redirect_Stdout);
    Z_String output=z_str_new(&pool,""); z_file_read_line(proc.stdout,&output); z_sv_println(z_sv(&output)); fclose(proc.stdout);
    char *catargs[]={"cat",NULL}; Z_Piped_Process duplex=z_pipe_process(catargs,Z_File_Redirect_Stdin|Z_File_Redirect_Stdout);
    fputs("stdin-ok",duplex.stdin); fclose(duplex.stdin); Z_String echoed=z_str_new(&pool,""); z_file_read_line(duplex.stdout,&echoed); z_sv_println(z_sv(&echoed)); fclose(duplex.stdout);
    char *errargs[]={"sh","-c","echo err-ok >&2",NULL}; Z_Piped_Process err=z_pipe_process(errargs,Z_File_Redirect_Stderr);
    Z_String error=z_str_new(&pool,""); z_file_read_line(err.stderr,&error); z_sv_println(z_sv(&error)); fclose(err.stderr);
}
