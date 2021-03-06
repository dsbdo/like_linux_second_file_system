#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "common.h"
#include "UserMoudle/UserMoudle.h"
#include "InitMoudle/InitMoudle.h"
#include "CommandParse/CommandParse.h"
const char *K_SYSTEM_FILE = "SystemDisk";
bool inode_bitmap[K_INODE_NUM];
bool block_bitmap[K_BLOCK_NUM];

const int g_root_dir_inode_addr = 0;
char g_current_user_name[100] = "root";
char g_current_host_name[100] = "dsbdo_host";
char g_current_group_name[100] = "root_group";
char g_current_user_dir_name[100] = "/";
char g_current_dir_name[100] = "/";
int g_current_dir_inode_addr = 0;
int g_current_dir_addr;
bool g_is_login = false;
int g_root_inode_addr = 0;
int main(int argc, char **argv)
{
	using std::cin;
	using std::cout;
	using std::endl;
	using std::fstream;
	using std::ios;
	using std::string;

	//检查系统文件是否存在
	fstream disk_head;
	disk_head.open(K_SYSTEM_FILE, ios::in | ios::out | ios::binary);
	InitMoudle init_moudle;
	UserMoudle user_moudle(init_moudle.getFileProcess());
	CommandParse cmd_object(init_moudle.getFileProcess());
	if (!disk_head)
	{
		cout << "File is not exist, 重新构建虚拟磁盘" << endl;
		system("dd if=/dev/zero of=SystemDisk bs=1G count=1");

		//初始化磁盘
		int init_result = init_moudle.initDisk();
		if (init_result == 0)
		{
			//初始化磁盘成功，创建超级用户
			cout << "初始化磁盘成功，现在创建磁盘超级用户 root " << endl;
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
					//用户输入正确，开始创建超级用户
					user_moudle.createRootUser(passwd);
					cmd_object.rootLogin(passwd);
					//如果创建失败呢? 先不管了，就这样吧
					break;
				}
				else
				{
					cout << "两次密码输入不一致，请重新输入" << endl;
				}
			}
		}
	}
	else
	{

		int init_result = init_moudle.initDisk();
		init_moudle.bootFileSystem();

		user_moudle.createRootUser("1234");
		//user_moudle.createOrdinaryUser("dsbdo", "1234567");

		while (true)
		{
			cout << "超级用户登录请输入密码: ";
			string passwd = "";
			cin >> passwd;
			string repeat_passwd = "";
			cout << "请再输入一次密码: ";
			cin >> repeat_passwd;
			if (passwd == repeat_passwd)
			{
				//用户输入正确，开始创建超级用户
				//user_moudle.createRootUser(passwd);
				if (cmd_object.rootLogin(passwd))
				{
					g_is_login = true;
					break;
				}
				else
				{
					cout << "用户密码错误" << endl;
				}
				//如果创建失败呢? 先不管了，就这样吧
			}
			else
			{
				cout << "两次密码输入不一致，请重新输入" << endl;
			}
		}
	}
	while (1)
	{
		if (g_is_login)
		{
			cmd_object.cmd();
		}
		else
		{
			cmd_object.cmd();
		}
	}
	return 0;
}
