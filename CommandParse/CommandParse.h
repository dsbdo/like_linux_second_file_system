#ifndef _COMMANDPARSE_H
#define _COMMANDPARSE_H

#include <iostream>
#include <string>
#include<cstdlib>
#include<ctype.h>
#include<cstdio>
#include <termio.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include "../common.h"
#include "../UserMoudle/UserMoudle.h"
#include "../FileProcess/FileProcess.h"

class CommandParse
{
  public:
    CommandParse(FileProcess *file_process);
    ~CommandParse();
    void cmd();
    bool rootLogin(std::string passwd);

  private:
    void help();
    void newUser(std::string user_name, std::string passwd);
    void login(std::string user_name);
    void ls(std::string target);
    void create(std::string file_name);
    void del(std::string file_name);
    void open(std::string file_name,std::string content, int type = K_APPEND_FILE);
    void close(std::string file_name);
    void mkdir(std::string dir_name);
    void rm(std::string dir_name);
    void read(std::string file_name);
    void cd(std::string dir_name);
    //实现一个vi编辑器
    struct WinSize {
      int posi_x;
      int posi_y;
    };
    WinSize m_win_size;
    WinSize m_curr_posi;
    //键盘输入立即获取数据
    int getKeyBoardInput();
    void updateInfo(char*);
    void updateWinSize();
    void clear();
    void moveUp(int x);
    void moveDown(int x);
    void moveLeft(int x);
    void moveRight(int x);
    void moveTo(int x, int y);
    void resetCursor();
    void hideCursor();
    void showCursor();
    void hightLight();
    void unHightLight();
    UserMoudle* user_moudle;
    FileProcess* m_file_process;
};

#endif