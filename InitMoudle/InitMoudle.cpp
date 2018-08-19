#include <cstring>
#include "InitMoudle.h"

//InitMoudle::InitMoudle(){}

//InitMoudle::~InitMoudle() {}

//完成磁盘的划分，超级块与inode块的写入，同时挂载根目录， root 目录， home 目录
int InitMoudle::initDisk() {
	SuperBlock init_superblock;
	init_superblock.s_Inode_num = (unsigned int) K_INODE_NUM;
	init_superblock.s_block_num = (unsigned int) K_BLOCK_NUM;
	init_superblock.s_free_Inode_num = (unsigned int) K_INODE_NUM;
	init_superblock.s_free_block_num = (unsigned int) K_BLOCK_NUM;
	
	//这里指的是存储数据块的开始地址
	init_superblock.s_free_data_block_addr =  K_DATA_BLOCK_START_ADDR;
	init_superblock.s_block_size = K_BLOCK_SIZE;
	init_superblock.s_Inode_size = K_INODE_SIZE;
	init_superblock.s_superblock_size = sizeof(SuperBlock);
	init_superblock.s_blocks_per_group = K_BLOCKS_PER_GROUP;
	
	//磁盘分布
	init_superblock.s_superblock_startAddr = K_SUPERBLOCK_STARTADDR;
	init_superblock.s_InodeBitmap_startAddr = K_INODE_BITMAP_STARTADDR;
	init_superblock.s_blockBitmap_startAddr = K_BLOCK_BITMAP_STARTADDR;
	init_superblock.s_Inode_startAddr = K_INODE_STARTADDR;
	init_superblock.s_data_block_startAddr = K_DATA_BLOCK_START_ADDR;
	
	// inode bitmap 写入
	memset(inode_bitmap, 0, sizeof(inode_bitmap));
	//磁盘头为disk_head;
	bool write_result = m_file_process.writeFile((char*)inode_bitmap, K_INODE_BITMAP_STARTADDR,sizeof(inode_bitmap));
	if(write_result) {
		//测一下inodebitmap 写了多少个block，因为仅有一个
		m_file_process.setBlockBitmap(K_INODE_BITMAP_STARTADDR);
	}
	else {
		//写失败
	}
	memset(block_bitmap, 0, sizeof(block_bitmap));
	
	//这里需要64个block 来描述 block bitmap
	write_result = m_file_process.writeFile((char*)block_bitmap, K_BLOCK_BITMAP_STARTADDR, sizeof(block_bitmap));

	if(write_result) {
		for(int i = 0; i < K_BLOCK_NUM/(4*1024); i++) {
			m_file_process.setBlockBitmap(K_BLOCK_BITMAP_STARTADDR+i);
		}
	}
	else {
		//写失败
	}
	//block_bitmap 也需要写入
	//将超级块写入
	std::cout << "super block size = "<< sizeof(SuperBlock) << " bytes" << std::endl;
	write_result = m_file_process.writeFile((char*)&init_superblock, K_SUPERBLOCK_STARTADDR, sizeof(SuperBlock));
	if(write_result) {
		std::cout << "write block success" << std::endl;
	}
	else {
		
	}
	 
	return true;
}

