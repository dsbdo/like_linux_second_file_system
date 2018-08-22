#include "UserMoudle.h"

UserMoudle::UserMoudle() {

}
UserMoudle::~UserMoudle(){

}
//创建超级用户
bool UserMoudle::createRootUser(std::string passwd) {
    //打开文件进行写入，inode 中的i_size 记录文件的大小， 其以byte为单位
    //读文件
    std::cout << m_file_process.locateFile("/etc/passwd", 0) << std::endl;
}