#include <z_time.h>
#include <stdio.h>

Z_Time z_time(void);

double z_time_elapsed_seconds(Z_Time time);
double z_time_elapsed_mseconds(Z_Time time);

void z_time_print_elapsed_seconds(Z_Time time);
void z_time_print_elapsed_mseconds(Z_Time time);


Z_Clock z_time(void)
{
    return clock();
}

double z_time_elapsed_seconds(Z_Clock time)
{
    return ((double)(z_time() - time)) / CLOCKS_PER_SEC;
}

double z_time_elapsed_mseconds(Z_Clock time)
{
    return z_time_elapsed_seconds(time) * 1000;
}

void z_time_print_elapsed_seconds(Z_Clock time)
{
    printf("%lfs\n", z_time_elapsed_seconds(time));
}

void z_time_print_elapsed_mseconds(Z_Clock time)
{
    printf("%lfms\n", z_time_elapsed_mseconds(time));
}
