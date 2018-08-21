
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
		void readBlock(char*& block_buf,int block_addr);

		bool mkdir(int parent_inode_addr, const char name[]);
		//删除当前目录下的所有文件
		void rmall(int parent_inode_addr);
		bool rmdir(int parent_inode_addr, char name[]);
		bool create(int parent_inode_addr, char name[], char buf[]);
		bool del(int parent_inode_addr, char name[]);
		void ls(int);
		void cd(int, char name[]);

		bool testWriteResult();
		void testWriteBlock(int block_addr);
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
