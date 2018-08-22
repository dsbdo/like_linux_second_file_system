//用户模块管理
//主要负责用户的登陆，登出， 注册， 删除
#ifndef _USERMOUDLE_H
#define _USERMOUDLE_H


#include "../common.h"
#include "../FileProcess/FileProcess.h"
class UserMoudle {
	public:
		UserMoudle();
		~UserMoudle();
		bool createRootUser(std::string passwd);
		bool createOrdinaryUser(std::string user_name, std::string user_passed);
		bool logIn(std::string user_name, std::string user_passwd);
		bool logOut();
		bool deleteUser(std::string user_name);
	private:
		bool flag;
};

#endif
