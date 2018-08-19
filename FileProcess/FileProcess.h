
/*
* 提供统一的对外写和读接口，
*
*/
#ifndef _FILEPROCESS_H

#define _FILEPROCESS_H

#include "../common.h"
class FileProcess {
	public:
		FileProcess();
		~FileProcess();
		//disk_head 便是磁盘头
		
		//按block 写入文件
		bool writeFile(char content[],int block_addr, int size_byte);
		bool setInodeBitmap(int);
		bool setBlockBitmap(int);
		
		
	private:
	std::fstream disk_head;
};
#endif
