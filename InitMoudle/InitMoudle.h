/*
*这一个模块主要实现的是虚拟磁盘的初始化工作，包括superblock的写入， inode bitmap 写入， block bitmap写入， inode 空间的开辟，创建用户文件，同时初始化超级用户
*/

#ifndef _INITMOUDLE_H
#define _INITMOUDLE_H
#include<iostream>
#include<fstream>
#include<ctime>
#include "../FileProcess/FileProcess.h"
#include "../common.h"
class InitMoudle {
	public:
		//InitMoudle();
		//~InitMoudle();
		int initDisk();
		
		//启动文件系统，读取超级块， bitmap等信息进入内存
		void bootFileSyatem();
		
	private:
		bool initSuperBlock();
		bool initInodeBitMap();
		bool initBlockBitMap();
		//创建根目录
		bool createRootDir();
		FileProcess m_file_process;
		
};

#endif
