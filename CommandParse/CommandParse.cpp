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
        std::string mode;
        std::string content;
        cin >> mode;
        cin >> content;
        int mode_flag = 0;
        if (mode == "<")
        {
            mode_flag = K_APPEND_FILE;
        }
        else
        {
            mode_flag = K_COVER_FILE;
        }
        open(target, content, mode_flag);
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
    else if (op == "cd")
    {
        cin >> target;
        cd(target);
    }
    else if (op == "new")
    {
        std::string target;
        cin >> target;
        while (true)
        {
            cout << "请输入密码: ";
            string passwd = "";
            cin >> passwd;
            string repeat_passwd = "";
            cout << "请再输入一次密码: ";
            cin >> repeat_passwd;
            if (passwd == repeat_passwd)
            {
                //用户输入正确
                newUser(target, passwd);
                //如果创建失败呢? 先不管了，就这样吧
                break;
            }
            else
            {
                cout << "两次密码输入不一致，请重新输入" << endl;
            }
        }
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
    cout << "打开并编辑（以追加模式）文件命令:  open file_name < content" << endl;
    cout << "打开并编辑（以覆盖模式）文件命令：open file_name << content" << endl;
    cout << "关闭文件命令： close file_name" << endl;
    cout << "创建目录命令： mkdir dir_name" << endl;
    cout << "删除目录命令： rm dir_name" << endl;
    cout << "读取文件并进行编辑命令： read file_name" << endl;
    cout << "切换目录命令： cd dir_name" << endl;
    cout << "创建新用户命令: new user_name" << endl;
}

void CommandParse::newUser(std::string user_name, std::string passwd)
{
    user_moudle->createOrdinaryUser(user_name, passwd);
}
void CommandParse::login(std::string user_name)
{
    //登录其他用户
    using std::cin;
    using std::cout;
    using std::endl;
    using std::string;

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
            return;
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
    char *buf = new char;
    m_file_process->create(g_current_dir_inode_addr, (char *)file_name.c_str(), buf, 0);
}
//删除文件
void CommandParse::del(std::string file_name)
{
    m_file_process->del(g_current_dir_inode_addr, (char *)file_name.c_str());
}
//打开文件进行编辑
void CommandParse::open(std::string file_name, std::string content, int type)
{
    if (file_name.size() > +K_MAX_NAME_SIZE)
    {
        std::cout << "超过最大文件名长度" << std::endl;
        return;
    }
    char *buf = new char[K_BLOCK_SIZE];
    memset(buf, 0, K_BLOCK_SIZE);
    int file_indoe_addr = m_file_process->locateFile((char *)file_name.c_str(), g_current_dir_inode_addr);
    std::cout << file_indoe_addr << std::endl;
    if (file_indoe_addr == -1)
    {
        //文件不存在，创建文件
        create(file_name);
        file_indoe_addr = m_file_process->locateFile((char *)file_name.c_str(), g_current_dir_inode_addr);
        //m_file_process->readFile(file_indoe_addr, buf);
    }
    else
    {
        m_file_process->readFile(file_indoe_addr, buf);
    }
    if (type == K_APPEND_FILE)
    {
        //默认追加模式
        memcpy(buf + strlen(buf), (char *)content.c_str(), content.size());
        m_file_process->sysEditFile(file_indoe_addr, buf, content.size(), K_APPEND_FILE);
    }
    else
    {
        //覆盖模式
        memcpy(buf, (char *)content.c_str(), content.size());
        m_file_process->sysEditFile(file_indoe_addr, buf, content.size(), K_COVER_FILE);
    }
    // //打开文件进行编辑
    // if (file_name.size() > +K_MAX_NAME_SIZE)
    // {
    //     std::cout << "超过最大文件名长度" << std::endl;
    //     return;
    // }
    // char *buf = new char[2 * K_BLOCK_SIZE];
    // memset(buf, 0, 2 * K_BLOCK_SIZE);
    // int file_indoe_addr = m_file_process->locateFile((char *)file_name.c_str(), g_current_dir_inode_addr);
    // std::cout << file_indoe_addr << std::endl;
    // if (file_indoe_addr == -1)
    // {
    //     //文件不存在，创建文件
    //     create(file_name);
    // }
    // else
    // {
    //     m_file_process->readFile(file_indoe_addr, buf);
    // }
    // //打开系统缓冲区进行编辑,应该判断一下文件是否允许编辑，这部分先不写了

    // //第一清屏
    // system("clear");
    // //记录当前光标位置，输入输出缓冲区便是buf,每隔一段时间自动刷新全屏
    // int cur_x = 0;
    // int cur_y = 0;
    // clear();
    // //将所有内容输出到控制台上
    // updateInfo(buf);
    // int key_press = 0;
    // //使用getchar 获取每一个按键
    // while (true)
    // {
    //     key_press = getKeyBoardInput();
    //     //无视组合键,且输入大写字母时必须使用shift辅助方式输入
    //     //计算现在缓冲区下标位置
    //     int char_posi = m_win_size.posi_x * m_curr_posi.posi_y + m_curr_posi.posi_x;
    //     if (key_press >= 32 && key_press <= 126)
    //     {
    //         //该输入键均是合法字符
    //         //整体向后移动一位
    //         for (int i = strlen(buf); i >= char_posi; i++)
    //         {
    //             buf[i] = buf[i + 1];
    //         }
    //         buf[char_posi] = key_press;
    //         //更新屏幕信息
    //         updateInfo(buf);
    //         //控制光标信息
    //         if (m_curr_posi.posi_x == m_win_size.posi_x - 1)
    //         {
    //             //这个已经是最后一个字符的填充
    //             m_curr_posi.posi_x = 0;
    //             m_curr_posi.posi_y++;
    //             //移动光标
    //         }
    //         else
    //         {
    //             m_curr_posi.posi_x++;
    //         }
    //         moveTo(m_curr_posi.posi_x, m_curr_posi.posi_y);
    //     }
    //     else if (key_press == 10)
    //     {
    //         //回车键
    //         if (m_curr_posi.posi_x == m_win_size.posi_x - 2)
    //         {
    //             for (int i = strlen(buf); i >= char_posi; i++)
    //             {
    //                 buf[i] = buf[i + 1];
    //             }
    //             buf[char_posi] = key_press;
    //             //更新屏幕信息
    //             updateInfo(buf);
    //         }
    //         else {
    //             for(int i = m_curr_posi.posi_x; i < m_win_size.posi_x - 2; i++) {

    //             }
    //         }

    //         //更新终端信息,如果提前回车，应该如何处理呢，这一整行全部填充空格
    //     }
    //     else if (key_press == 27)
    //     {
    //         //ESC 键
    //     }
    //     else if (key_press == 127)
    //     {
    //         //退格键
    //     }
    //     else if (key_press == 9)
    //     {
    //         //tab 键，一次输出四个空格
    //     }
    // }
}
//关闭文件
void CommandParse::close(std::string file_name)
{
}

void CommandParse::mkdir(std::string dir_name)
{
    using std::cin;
    using std::cout;
    using std::endl;
    //没有返回，emmm,还是需要返回的
    bool flag = m_file_process->mkdir(g_current_dir_inode_addr, dir_name.c_str());
    if (flag)
    {
        cout << "创建文件夹成功" << endl;
    }
    else
    {
        cout << "创建失败" << endl;
    }
}
//删除目录
void CommandParse::rm(std::string dir_name)
{
    //dir_name  += "/";
    //int file_indoe_addr = m_file_process->locateFile((char*)dir_name.c_str(), g_current_dir_inode_addr);
    //m_file_process->rmall(file_indoe_addr);
    m_file_process->rmdir(g_current_dir_inode_addr, (char *)dir_name.c_str());
}
//读取文件内容直接进行输出
void CommandParse::read(std::string file_name)
{
    int file_indoe_addr = m_file_process->locateFile((char *)file_name.c_str(), g_current_dir_inode_addr);
    //需要预估文件大小
    char *buf = new char[K_BLOCK_SIZE];
    m_file_process->readFile(file_indoe_addr, buf);
    for (int i = 0; i < strlen(buf); i++)
    {
        std::cout << buf[i];
    }
    std::cout << std::endl;
}

void CommandParse::cd(std::string dir_name)
{
    //需要有人告诉我是否是绝对寻址
    if (dir_name[0] == '/')
    {
        m_file_process->cd(g_root_dir_inode_addr, (char *)dir_name.c_str(), 1);
    }
    else
    {
        m_file_process->cd(g_current_dir_inode_addr, (char *)dir_name.c_str());
    }
}

void CommandParse::updateInfo(char *buf)
{
    updateWinSize();
    fputs(buf, stdout);
    m_curr_posi.posi_x = strlen(buf) % m_win_size.posi_x;
    m_curr_posi.posi_y = strlen(buf) / m_win_size.posi_x;
}
void CommandParse::updateWinSize()
{
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    m_win_size.posi_x = w.ws_col;
    m_win_size.posi_y = w.ws_row;
}

void CommandParse::clear()
{
    std::cout << "\033[2J";
}
void CommandParse::moveUp(int x)
{
    printf("\033[%dA", x);
}
void CommandParse::moveDown(int x)
{
    printf("\033[%dB", x);
}
void CommandParse::moveLeft(int x)
{
    printf("\033[%dD", x);
}
void CommandParse::moveRight(int x)
{
    printf("\033[%dC", x);
}
void CommandParse::moveTo(int x, int y)
{
    printf("\033[%d;%dH", x, y);
}
void CommandParse::resetCursor()
{
    printf("\033[H");
}
void CommandParse::hideCursor()
{
    printf("\033[?25l");
}
void CommandParse::showCursor()
{
    printf("\033[?25h");
}
void CommandParse::hightLight()
{
    printf("\033[7m");
}
void CommandParse::unHightLight()
{
    printf("\033[27m");
}

int CommandParse::getKeyBoardInput()
{
    int in;
    struct termios new_settings;
    struct termios stored_settings;
    //获取终端参数
    tcgetattr(0, &stored_settings);
    new_settings = stored_settings;
    new_settings.c_lflag &= (~ICANON);
    new_settings.c_cc[VTIME] = 0;
    tcgetattr(0, &stored_settings);
    new_settings.c_cc[VMIN] = 1;
    //设置终端参数，TCSANOW 数据未传送完毕，马上改变属性
    tcsetattr(0, TCSANOW, &new_settings);
    in = getchar();
    tcsetattr(0, TCSANOW, &stored_settings);
    return in;
}
