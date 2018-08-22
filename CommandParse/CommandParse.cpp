#include "CommandParse.h"

void CommandParse::cmd()
{
    using std::cin;
    using std::cout;
    using std::endl;
    using std::string;
    string op;     //命令
    string target; //操作对象
    cout << "\033[1;32m"<< g_current_user_name << '@' << g_current_host_name << ':' << "\033[0m" << "\033[1;34m" << g_current_dir_name << "\033[0m" << "$ ";
    cin >> op;
    if (op == "help"){
        help();
    }
    else if (op == "login"){
        cin >> target;
        login(target);
    }
    else if (op == "ls"){
        //in >> target;
        ls(target);
    }
    else if (op == "create"){
        cin >> target;
        create(target);
    }
    else if (op == "del"){
        cin >> target;
        del(target);
    }
    else if (op == "open"){
        cin >> target;
        open(target);
    }
    else if (op == "close"){
        cin >> target;
        close(target);
    }
    else if (op == "mkdir"){
        cin >> target;
        mkdir(target);
    }
    else if (op == "rm"){
        cin >> target;
        rm(target);
    }
    else if (op == "read"){
        cin >> target;
        read(target);
    }
    else {
        cout << "错误命令, 输入 help 打开命令帮助页面" << endl;
    }
    return ;
}

bool CommandParse::rootLogin(std::string passwd)
{
    using std::cout;
    using std::endl;
    cout << "root login..." << endl;
}
//打印帮助命令
void CommandParse::help()
{
    using std::cout;
    using std::endl;
    cout << "帮助命令：    help" << endl;
    cout << "用户登录命令： login username" << endl;
    cout << "文件列举命令： ls" << endl;
    cout << "创建文件命令： create file_name" << endl;
    cout << "删除文件命令： del file_name" << endl;
    cout << "打开文件命令:  open file_name" << endl;
    cout << "关闭文件命令： close file_name" << endl;
    cout << "创建目录命令： mkdir dir_name" << endl;
    cout << "删除目录命令： rm dir_name" << endl;
    cout << "读取文件并进行编辑命令： read file_name" << endl;
}
void CommandParse::login(std::string user_name) {

}
void CommandParse::ls(std::string target){
    std::cout << "ls" << std::endl;
    return;
}
void CommandParse::create(std::string file_name){
}
void CommandParse::del(std::string file_name){

}
void CommandParse::open(std::string file_name){

}
void CommandParse::close(std::string file_name){

}
void CommandParse::mkdir(std::string dir_name){

}
void CommandParse::rm(std::string dir_name){

}
void CommandParse::read(std::string file_name){

}