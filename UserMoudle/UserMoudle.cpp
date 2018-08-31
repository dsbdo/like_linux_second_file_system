#include "UserMoudle.h"

UserMoudle::UserMoudle(FileProcess *file_process)
{
    m_file_process = file_process;
}
UserMoudle::~UserMoudle()
{
}
//创建超级用户
bool UserMoudle::createRootUser(std::string passwd)
{
    //打开文件进行写入，inode 中的i_size 记录文件的大小， 其以byte为单位
    //读文件
    m_file_process->cd(g_root_dir_inode_addr, "etc");

    UserInfo user_root;
    strcpy(user_root.name, "root");
    strcpy(user_root.passwd, passwd.c_str());

    // UserInfo user_root2;
    // strcpy(user_root2.name, "dsbdo_2");
    // strcpy(user_root2.passwd, "*********");
    //创建文件,这里就已经写入一个root了
    m_file_process->create(g_current_dir_inode_addr, "passwd", (char *)&user_root, sizeof(UserInfo));
    //定位文件地址
    int file_inode_addr = m_file_process->locateFile("/etc/passwd", 0);
    std::cout << g_current_dir_inode_addr << std::endl;
    std::cout << file_inode_addr << std::endl;
    //写入文件
    // m_file_process->sysEditFile(file_inode_addr, (char *)&user_root, sizeof(UserInfo), K_APPEND_FILE);
    // char *buffer = new char[1024];
    // m_file_process->readFile(file_inode_addr, buffer);
    // UserInfo user2[2];
    // memcpy(user2, buffer, sizeof(UserInfo) * 2);
    // std::cout << user2[0].name << " " << user2[0].passwd << std::endl;
    //std::cout << user2[1].name << " " << user2[1].passwd << std::endl;
    m_file_process->cd(g_current_dir_inode_addr, "..");
    std::cout << g_current_dir_inode_addr << std::endl;
    strcpy(g_current_user_name, "root");
    strcpy(g_current_dir_name, "/");
    //g_current_user_name ="root";
    //g_current_user_dir_name = "/";
    std::cout << g_current_user_name << std::endl;
    std::cout << g_current_dir_name << std::endl;
    return true;
}

//创建普通用户
bool UserMoudle::createOrdinaryUser(std::string user_name, std::string passwd)
{
    //定位passwd文件的位置
    int file_inode_addr = m_file_process->locateFile("/etc/passwd", 0);
    UserInfo new_user;
    strcpy(new_user.name, user_name.c_str());
    strcpy(new_user.passwd, passwd.c_str());
    m_file_process->sysEditFile(file_inode_addr, (char *)&new_user, sizeof(UserInfo), K_APPEND_FILE);
    file_inode_addr = m_file_process->locateFile("/home/", g_root_dir_inode_addr);
    m_file_process->mkdir(file_inode_addr,user_name.c_str());
    // char *buffer = new char[1024];
    // m_file_process->readFile(file_inode_addr, buffer);
    // UserInfo user2[4];
    // memcpy(user2, buffer, sizeof(UserInfo) * 4);
    // std::cout << user2[0].name << " " << user2[0].passwd << std::endl;
    // std::cout << user2[1].name << " " << user2[1].passwd << std::endl;
    // std::cout << user2[3].name << " " << user2[3].passwd << std::endl;
    return true;
}
//登录系统
bool UserMoudle::logIn(std::string user_name, std::string passwd)
{
    //登录那就将所有block取出来，一块一块读，直到找到该用户
    int file_inode_addr = m_file_process->locateFile("/etc/passwd", g_root_dir_inode_addr);
    //将所有block读取出来
    Inode *temp_inode = new Inode();
    m_file_process->readInode(temp_inode, file_inode_addr);
    //遍历12个块，12个直接块
    char *buf_4kb = new char[K_BLOCK_SIZE];
    for (int i = 0; i < 12; i++)
    {
        if (temp_inode->i_dirBlock[i] != -1)
        {
            //该块有效
            m_file_process->readBlock(buf_4kb, temp_inode->i_dirBlock[i]);
            //遍历该块,一个记录可能在两个块之间，嘤嘤嘤，写坏了
            UserInfo user[1];
            //这里是有问题的，因为一个记录可能在两个块之间，所以必须考虑跨块之间的文件读写，先不处理
            for (int j = 0; ; j++)
            {
                if(sizeof(UserInfo)*(j+1) < K_BLOCK_SIZE){
                    memcpy(user, buf_4kb + j*sizeof(UserInfo), sizeof(UserInfo));
                    if(strcmp(user[0].name, user_name.c_str()) == 0 && strcmp(user[0].passwd, passwd.c_str())== 0) {
                        if(user_name == "root") {
                            //超级用户登录
                            strcpy(g_current_user_name, user_name.c_str());
                            strcpy(g_current_user_dir_name, "/");
                            g_current_dir_inode_addr = g_root_dir_inode_addr;
                            return true;
                        }
                        //切换用户
                        
                        char* user_dir= new char[6+user_name.size()+1+1];
                        // "/home/"+user_name+"/";
                        user_dir[0] = '/';
                        user_dir[1] = 'h';
                        user_dir[2] = 'o';
                        user_dir[3] = 'm';
                        user_dir[4] = 'e';
                        user_dir[5] = '/';
                        for(int i = 0; i < user_name.size(); i++) {
                            user_dir[i+6] = user_name[i];
                        }
                        user_dir[6+user_name.size()] = '/';
                        user_dir[6+user_name.size()+1] = '\0';
                        std::cout << user_dir << std::endl;
                        //切换文件路径支持绝对路径寻址与相对路径寻址
                        m_file_process->cd(g_root_dir_inode_addr, user_dir, 1);
                        strcpy(g_current_user_name, user_name.c_str());
                        //确定当前inode 位置
                        //g_current_dir_inode_addr = m_file_process->locateFile((char*)user_dir.c_str(), g_root_dir_inode_addr);
                        strcpy(g_current_user_dir_name, user_dir);
                        return true;
                    }
                }
                else {
                    return false;
                }
            }
        }
    }
    return false;
}
bool UserMoudle::isUserExit(std::string user_name) {
    //登录那就将所有block取出来，一块一块读，直到找到该用户
    int file_inode_addr = m_file_process->locateFile("/etc/passwd", g_root_dir_inode_addr);
    //将所有block读取出来
    Inode *temp_inode = new Inode();
    m_file_process->readInode(temp_inode, file_inode_addr);
    //遍历12个块，12个直接块
    char *buf_4kb = new char[K_BLOCK_SIZE];
    for (int i = 0; i < 12; i++)
    {
        if (temp_inode->i_dirBlock[i] != -1)
        {
            //该块有效
            m_file_process->readBlock(buf_4kb, temp_inode->i_dirBlock[i]);
            //遍历该块,一个记录可能在两个块之间，嘤嘤嘤，写坏了
            UserInfo user[1];
            //这里是有问题的，因为一个记录可能在两个块之间，所以必须考虑跨块之间的文件读写，先不处理
            for (int j = 0; ; j++)
            {
                if(sizeof(UserInfo)*(j+1) < K_BLOCK_SIZE){
                    memcpy(user, buf_4kb + j*sizeof(UserInfo), sizeof(UserInfo));
                    if(strcmp(user[0].name, user_name.c_str()) == 0) {
                        return true;
                    }
                }
                else {
                    return false;
                }
            }
        }
    }
}