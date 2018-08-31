#include "CommandParse.h"

CommandParse::CommandParse(FileProcess *file_process)
{
    m_file_process = file_process;
    user_moudle = new UserMoudle(file_process);
}
CommandParse::~CommandParse()
{
    delete user_moudle;
}
void CommandParse::cmd()
{
    using std::cin;
    using std::cout;
    using std::endl;
    using std::string;
    string op;     //命令
    string target; //操作对象
    cout << "\033[1;32m" << g_current_user_name << '@' << g_current_host_name << ':' << "\033[0m"
         << "\033[1;34m" << g_current_dir_name << "\033[0m"
         << "$ ";
    cin >> op;
    if (op == "help")
    {
        help();
    }
    else if (op == "login")
    {
        cin >> target;
        login(target);
    }
    else if (op == "ls")
    {
        //in >> target;
        ls(target);
    }
    else if (op == "create")
    {
        cin >> target;
        create(target);
    }
    else if (op == "del")
    {
        cin >> target;
        del(target);
    }
    else if (op == "open")
    {
        cin >> target;
        open(target);
    }
    else if (op == "close")
    {
        cin >> target;
        close(target);
    }
    else if (op == "mkdir")
    {
        cin >> target;
        mkdir(target);
    }
    else if (op == "rm")
    {
        cin >> target;
        rm(target);
    }
    else if (op == "read")
    {
        cin >> target;
        read(target);
    }
    else if(op == "cd") {
        cin >> target;
        cd(target);
    }
    else
    {
        cout << "错误命令, 输入 help 打开命令帮助页面" << endl;
    }
    return;
}

bool CommandParse::rootLogin(std::string passwd)
{
    using std::cout;
    using std::endl;
    if (user_moudle->logIn("root", passwd))
    {
        cout << "root login..." << endl;
        return true;
    }
    else
    {
        return false;
    }
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
    cout << "切换目录命令： cd dir_name" << endl;
}
void CommandParse::login(std::string user_name)
{
    //登录其他用户
    using std::cin;
    using std::cout;
    using std::string;
    using std::endl;

    cout << "请输入密码: ";
    string passwd = "";
    cin >> passwd;
    string repeat_passwd = "";
    cout << "请再输入一次密码: ";
    cin >> repeat_passwd;
    if (passwd == repeat_passwd)
    {
        //用户输入正确，开始创建超级用户
        //user_moudle.createRootUser(passwd);
        if (user_moudle->logIn(user_name, passwd))
        {
            g_is_login = true;
            cout << g_current_dir_inode_addr << endl;
            return ;
        }
        else
        {
            cout << "用户密码错误" << endl;
        }
        //如果创建失败呢? 先不管了，就这样吧
    }
    else
    {
        cout << "两次密码输入不一致，登录失败" << endl;
    }
  
}
void CommandParse::ls(std::string target)
{
    //if(target != )
    m_file_process->ls(g_current_dir_inode_addr);
    return;
}

//创建文件
void CommandParse::create(std::string file_name)
{
    char buf[10] = {'A','B','C', 'D', 'E', '\0', 'H','L'};
    m_file_process->create(g_current_dir_inode_addr, (char*)file_name.c_str(), buf, 10);
}
//删除文件
void CommandParse::del(std::string file_name)
{
    m_file_process->del(g_current_dir_inode_addr, (char*)file_name.c_str());
}
//打开文件进行编辑
void CommandParse::open(std::string file_name)
{
    //打开文件进行编辑
    if(file_name.size() >+ K_MAX_NAME_SIZE) {
        std::cout << "超过最大文件名长度" << std::endl;
        return ;
    }
    char* buf = new char[2*K_BLOCK_SIZE];
    memset(buf, 0, 2*K_BLOCK_SIZE);
    int file_indoe_addr = m_file_process->locateFile((char*)file_name.c_str(),g_current_dir_inode_addr);
    std::cout <<  file_indoe_addr << std::endl;
    if(file_indoe_addr == -1){
        //文件不存在，创建文件
        create(file_name);
    }
    else {
        m_file_process->readFile(file_indoe_addr, buf);
    }
    //打开系统缓冲区进行编辑,应该判断一下文件是否允许编辑，这部分先不写了

    //第一清屏
    system("clear");
    int cur_x, cur_y, win_x, win_y;
    std::cout << "\033[20;20H"  << std::endl;





}
//关闭文件
void CommandParse::close(std::string file_name)
{
}

void CommandParse::mkdir(std::string dir_name)
{
    using std::cout;
    using std::cin;
    using std::endl;
    //没有返回，emmm,还是需要返回的
    bool flag = m_file_process->mkdir(g_current_dir_inode_addr, dir_name.c_str());
    if(flag) {
        cout << "创建文件夹成功" << endl;
    }else {
        cout << "创建失败" << endl;
    }
}
//删除目录
void CommandParse::rm(std::string dir_name)
{
    //dir_name  += "/";
    //int file_indoe_addr = m_file_process->locateFile((char*)dir_name.c_str(), g_current_dir_inode_addr);
   //m_file_process->rmall(file_indoe_addr);
   m_file_process->rmdir(g_current_dir_inode_addr, (char*)dir_name.c_str());
}
//读取文件内容直接进行输出
void CommandParse::read(std::string file_name)
{
    int file_indoe_addr = m_file_process->locateFile((char*)file_name.c_str(), g_current_dir_inode_addr);
    //需要预估文件大小
    char* buf = new char[K_BLOCK_SIZE];
    m_file_process->readFile(file_indoe_addr, buf);
    for(int i =0; i < K_BLOCK_SIZE; i++) {
        std::cout << buf[i];
    }
    std::cout << std::endl; 
}

void CommandParse::cd(std::string dir_name) {
    //需要有人告诉我是否是绝对寻址
    if(dir_name[0] == '/') {
         m_file_process->cd(g_root_dir_inode_addr, (char*)dir_name.c_str(), 1);
    }
    else {
          m_file_process->cd(g_current_dir_inode_addr, (char*)dir_name.c_str());
    }
   
}

