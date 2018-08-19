#ifndef _COMMON_H
#define _COMMON_H
#include<string>
#include<ctime>
#include<fstream>
//fstream disk_head;
extern const char* K_SYSTEM_FILE;
const int K_BLOCK_SIZE = 1024*4; //每一个block 为 4KB

const int K_INODE_SIZE = 128; //每一个inode大小
const int K_MAX_NAME_SIZE = 128;
const int K_INODE_NUM = 512; //512 文件
const int K_BLOCK_NUM = 1024 * 1024 /4;
//磁盘大小为1 G
const int K_BLOCKS_PER_GROUP = 64;

//文件拥有者权限说明
const int K_OWNER_R = 4;
const int K_OWNER_W = 2;
const int K_OWNER_X = 1;
//用户组对文件的权限
const int K_GROUP_R = 4;
const int K_GROUP_W = 2;
const int K_GROUP_X = 1;
//其他用户对文件的权限
const int K_OTHER_R = 4;
const int K_OTHER_W = 2;
const int K_OTHER_X = 1;



//超级块地址起始地址
const int K_SUPERBLOCK_STARTADDR = 0;

//Inode bitmap 起始地址
const int K_INODE_BITMAP_STARTADDR = 1;

//block bitmap 起始地址， 按block 编址
const int K_BLOCK_BITMAP_STARTADDR = 2;

//Inode 块起始地址, 1 条 inode 项 128 bytes， 所以4 kB 总共有32 个 inode
const int K_INODE_STARTADDR = 66;

//data block 起始块地址,因为总共有16个block 用以存储inode
const int K_DATA_BLOCK_START_ADDR = 82;

//单个文件最大尺寸bytes为单位最大560KB
const int K_FILE_MAX_SIZE_KB = 560;


//inode 位图声明
extern bool inode_bitmap[K_INODE_NUM];
extern bool block_bitmap[K_BLOCK_NUM];

//数据块起始地址
//声明superblock 数据结构
struct SuperBlock {
	//inode 数量 最多65536
	unsigned int s_Inode_num;
	//block number最多4294967294 
	unsigned int s_block_num;

	unsigned int s_free_Inode_num;
	unsigned int s_free_block_num;
	int s_free_data_block_addr;
	int s_free[K_BLOCKS_PER_GROUP];
	
	unsigned int s_block_size;
	unsigned int s_Inode_size;
	unsigned int s_superblock_size;
	unsigned int s_blocks_per_group;
	

	//磁盘分布
	int s_superblock_startAddr;
	int s_InodeBitmap_startAddr;
	int s_blockBitmap_startAddr;
	int s_Inode_startAddr;
	int s_data_block_startAddr;
};

//inode块信息
struct Inode {
	unsigned short i_Inode_num;
	unsigned short i_mode;
	unsigned short i_counter;
	char i_uname[20];
	char i_gname[20];
	unsigned int i_size;
	
	time_t i_ctime;
	time_t i_mtime;
	time_t i_atime;
	int i_dirBlock[12];//这是inode直接指向的12 物理块， 也就是12*4 = 48 KB
	int i_indorBlock_1; //这是一级间接块，也就是用一个block来记录该文件剩下存储的block地址，一个地址4bytes， 所以是： 512 / 4 * 4KB = 512 KB
	//合起来单个文件最大为560KB
};

//目录item 声明
struct DirItem {
	char itemName[K_MAX_NAME_SIZE];
	int inode_addr;
};

#endif
