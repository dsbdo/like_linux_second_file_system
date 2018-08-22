
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
		bool writeBlockFile(char content[], int block_addr, int size_byte=K_BLOCK_SIZE);
		bool writeInode(char content[], int inode_addr, int size_byte = K_INODE_SIZE);

		bool setInodeBitmap(int);
		bool setBlockBitmap(int);
		
		bool isFreeInode();
		bool isFreeBlock();
		
		int allocInode();
		int allocBlock();

		bool freeFile();
		bool freeBlock(int block_addr);
		bool freeInode(int inode_addr);

		void loadBitMap();
		void loadSuperBlock();

		//读操作
		void readInode(Inode* inode_item, int inode_addr);
		void readBlock(char* block_buf,int block_addr);
		void readFile(int file_inode_addr, char* buf);	

		bool mkdir(int parent_inode_addr, const char name[]);
		//删除当前目录下的所有文件
		void rmall(int parent_inode_addr);
		bool rmdir(int parent_inode_addr, char name[]);
		bool create(int parent_inode_addr, char name[], char buf[], int size_byte);
		bool del(int parent_inode_addr, char name[]);
		void ls(int);
		void cd(int, char name[]);
		
		//打开文件并编辑
		void vi(int file_inode_addr);
			
		//系统打开文件并写文件, edit_type 是指追加，还是清空重写
		void sysEditFile(int file_inode_addr, char content[],int content_size_byte, int edit_type);
		
		int locateFile(char path[], int current_inode);
		//将本目录要寻找的文件名与本目录inode地址传递，进行寻找并返回对应inode
		int locateFileHelp(char name[], int cur_inode_addr, int name_len, int locate_dir_or_file = 1);


		bool testWriteResult();
		void testWriteBlock(int block_addr, int type = 1);
		void testWriteInode(int inode_addr );

		void coutBitmap();
		
	private:
		int m_free_inode_num;
		int m_free_block_num;
	  std::fstream disk_head;
	  	bool m_inode_bitmap[K_INODE_NUM];
		bool m_block_bitmap[K_BLOCK_NUM]; 
		char buf_4KB[1024*4];
		SuperBlock* m_super_block;
};
#endif
