#ifndef _COMMANDPARSE_H
#define _COMMANDPARSE_H

#include <iostream>
#include <string>
#include<cstdlib>
#include<ctype.h>
#include<cstdio>
#include "../common.h"
#include "../UserMoudle/UserMoudle.h"
#include "../FileProcess/FileProcess.h"

class CommandParse
{
  public:
    CommandParse(FileProcess *file_process);
    ~CommandParse();
    //命令部分的处理， 文件命令处理
    //create, touch, 创建文件命令
    //mkdir 命令， rm 删除文件目录命令， del 删除文件名令
    //ls 文件枚举命令
    //open打开文件命令
    //read write 命令
    //用户命令处理
    //login
    void cmd();
    bool rootLogin(std::string passwd);

  private:
    void help();
    void login(std::string user_name);
    void ls(std::string target);
    void create(std::string file_name);
    void del(std::string file_name);
    void open(std::string file_name);
    void close(std::string file_name);
    void mkdir(std::string dir_name);
    void rm(std::string dir_name);
    void read(std::string file_name);
    void cd(std::string dir_name);
    //实现退格
 
    UserMoudle* user_moudle;
    FileProcess* m_file_process;
};

#endif