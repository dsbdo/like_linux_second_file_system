#include <stdio.h>
#include <unistd.h>

int main(void)
{
    char str[] = "\033[31mplease wait...\033[?25l";
    char ch[3] = {'\\', '-', '/'};
    int  times = 20;

    fputs(str, stderr);
    while (times --)
    {
        fputc(ch[0], stderr);
        fputs("\033[1D", stderr);
        usleep(200000);
        fputc(ch[1], stderr);
        fputs("\033[1D", stderr);
        usleep(200000);
        fputc(ch[2], stderr);
        fputs("\033[1D", stderr);
        usleep(200000);  
    }
    fputs("\n\033[?25h", stderr);

    return 0;
}