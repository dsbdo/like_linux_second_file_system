#include <stdio.h>
//带有宏定义函数的头文件
#include<stdlib.h>
#include <unistd.h>
//清屏
#define CLEAR() printf("\033[2J")
 
// 上移光标
#define MOVEUP(x) printf("\033[%dA", (x))
 
// 下移光标
#define MOVEDOWN(x) printf("\033[%dB", (x))
 
// 左移光标
#define MOVELEFT(y) printf("\033[%dD", (y))
 
// 右移光标
#define MOVERIGHT(y) printf("\033[%dC",(y))
 
// 定位光标
#define MOVETO(x,y) printf("\033[%d;%dH", (x), (y))
 
// 光标复位
#define RESET_CURSOR() printf("\033[H")
// 隐藏光标
#define HIDE_CURSOR() printf("\033[?25l")
 
// 显示光标
#define SHOW_CURSOR() printf("\033[?25h")
 
//清除从光标到行尾的内容
#define CLEAR_LINE() printf("\033[K")
//反显
#define HIGHT_LIGHT() printf("\033[7m")
#define UN_HIGHT_LIGHT() printf("\033[27m")
 
//设置颜色
#define SET_FRONT_COLOR(color) printf("\033[%dm",(color))
#define SET_BACKGROUND_COLOR(color) printf("\033[%dm",(color))
//前景色
#define FRONT_BLACK 30
#define FRONT_RED 31
#define FRONT_GREEN 32
#define FRONT_YELLOW 33
#define FRONT_BLUE 34
#define FRONT_PURPLE 35
#define FRONT_DEEP_GREEN 36
#define FRONT_WHITE 37
 
//背景色
#define BACKGROUND_BLACK 40
#define BACKGROUND_RED 41
#define BACKGROUND_GREEN 42
#define BACKGROUND_YELLOW 43
#define BACKGROUND_BLUE 44
#define BACKGROUND_PURPLE 45
#define BACKGROUND_DEEP_GREEN 46
#define BACKGROUND_WHITE 47

 
int main(int argc,char **argv)
{
    char c;
    while(true) {
        c = getchar();
        fputc(c, stdout);
    }

        //利用宏定义函数
        SET_BACKGROUND_COLOR(BACKGROUND_RED);
        printf("前白后红");
        sleep(3);
        SET_FRONT_COLOR(FRONT_BLUE);
        printf("前蓝后红");
        sleep(3);
        SET_BACKGROUND_COLOR(BACKGROUND_BLACK);
        SET_FRONT_COLOR(FRONT_WHITE);
        printf("开始测试光标移动:");
        sleep(3);
        MOVEDOWN(5);
        printf("下移5格后打印的结果");
        sleep(3);
        MOVEUP(3);
        printf("再上移3格后打印的结果");
        sleep(3);
        MOVETO(12,12);
        printf("移动到12,12后打印的结果");
        sleep(3);
        
        //直接使用控制语句
        printf("\033[31mThe color,%s!\033[1m\n"," 1");
        printf("\033[31mThe color,%s!\033[4m\n"," 2");
        printf("\033[31mThe color,%s!\033[5m\n"," 3");
        printf("\033[31mThe color,%s!\033[7m\n"," 4");
        printf("\033[31mThe color,%s!\033[8m\n"," 5");
        printf("\033[31mThe color,%s!\033[0m\n"," 6");
        printf("\033[47;31mThe color,%s!\033[0m\n"," 7");
        printf("\033[47mThe color,%s!\033[0m\n"," 8");
        sleep(2);
        printf("\033[45m%s!\033[5A\n","up 5");
        sleep(2);
        printf("\033[44m%s!\033[9B\n","down 9");
        sleep(2);
        printf("\033[43m%s!\033[19C\n","right 19");
        printf("right19");
        sleep(2);
        printf("\033[46m%s!\033[10D\n","left 10");
        printf("left 10");
        sleep(2);
        printf("\033[44m%s!\033[50;20H\n","move to y:50,x 20");
        printf("y50 x 20");
        sleep(2);
        printf("\033[44m%s!\033[?25l\n","hide cursor");
        sleep(2);
        printf("\033[40m%s!\033[?25h\n","sow cursor");
        sleep(2);
        //printf("\033[47m%s!\033[2J\n","clear scleen");
        sleep(2);
        return 0;
}
