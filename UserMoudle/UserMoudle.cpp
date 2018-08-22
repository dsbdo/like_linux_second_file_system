#include "UserMoudle.h"

UserMoudle::UserMoudle(FileProcess* file_process)
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
    strcpy(user_root.name, "dsbdo_1");
    strcpy(user_root.passwd, "12345678");

    UserInfo user_root2;
    strcpy(user_root2.name, "dsbdo_2");
    strcpy(user_root2.passwd, "*********");
    m_file_process->create(g_current_dir_inode_addr, "passwd", (char*)&user_root, sizeof(UserInfo));

    int file_inode_addr = m_file_process->locateFile("/etc/passwd", 0);
    std::cout << file_inode_addr << std::endl;

    m_file_process->sysEditFile(file_inode_addr, (char *)&user_root2, sizeof(UserInfo), K_APPEND_FILE);
    char *buffer = new char[1024];
    m_file_process->readFile(file_inode_addr, buffer);
    UserInfo user2[2];
    memcpy(user2, buffer, sizeof(UserInfo)*2);
    std::cout << user2[0].name << " " << user2[0].passwd << std::endl;
    std::cout << user2[1].name << " " << user2[1].passwd << std::endl;
    return true;
}