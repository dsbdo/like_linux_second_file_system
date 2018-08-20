#include<iostream>
#include<string>
#include<fstream>
#include<cstdlib>
#include "common.h"
#include "UserMoudle/UserMoudle.h"
#include "InitMoudle/InitMoudle.h"
#include "CommandParse/CommandParse.h"
const char* K_SYSTEM_FILE = "SystemDisk";
bool inode_bitmap[K_INODE_NUM];
bool block_bitmap[K_BLOCK_NUM];

int  g_root_dir_inode_addr;
char g_current_user_name[100];
char g_current_host_name[100];
char g_current_group_name[100];
char g_current_user_dir_name[100];


int main(int argc, char** argv) {
	using std::cout;
	using std::cin;
	using std::endl;
	using std::string;
	using std::fstream;
	using std::ios;
	
	//检查系统文件是否存在
	fstream disk_head;
	disk_head.open(K_SYSTEM_FILE, ios::in|ios::out|ios::binary);
	if(!disk_head) {
		cout << "File is not exist, 重新构建虚拟磁盘" << endl; 
		system("dd if=/dev/zero of=SystemDisk bs=1G count=1");
		InitMoudle init_moudle;
		//UserMoudle user_moudle;
		//初始化磁盘
		int init_result	= init_moudle.initDisk();
		if(init_result == 0) {
			//初始化磁盘成功，创建超级用户
			cout << "初始化磁盘成功，现在创建磁盘超级用户 root " << endl;
		     while(true) {
			cout << "请输入密码: " ;
			string passwd = "";
			cin >>  passwd;
			string repeat_passwd = "";
			cout << "请再输入一次密码: ";
			cin >> repeat_passwd;
			if (passwd == repeat_passwd) {
				//用户输入正确，开始创建超级用户
				//user_moudle.createRootUser(passwd);
				//如果创建失败呢? 先不管了，就这样吧
				break;
			}
			else {
				cout << "两次密码输入不一致，请重新输入" << endl;
				
			}
		    }
		}
	}
	else {
		cout << "go to next step" << endl;
	}	
	return 0;
}
