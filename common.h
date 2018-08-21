#ifndef _COMMON_H
#define _COMMON_H
#include<string>
#include<ctime>
#include<fstream>
#include<cstdlib>
#include<cstring>
#include<string.h>




#define MODE_DIR	01000					//目录标识
#define MODE_FILE	00000					//文件标识
#define FILE_DEF_PERMISSION 0664			//文件默认权限
#define DIR_DEF_PERMISSION	0755			//目录默认权限
//fstream disk_head;
extern const char* K_SYSTEM_FILE;
extern int g_current_dir_addr;

const int K_BLOCK_SIZE = 1024*4; //每一个block 为 4KB

const int K_INODE_SIZE = 512; //每一个inode大小
const int K_MAX_NAME_SIZE = 128;
const int K_INODE_NUM = 512; //512 文件
const int K_BLOCK_NUM = 1024 * 1024 /4;
//磁盘大小为1 G
const int K_BLOCKS_PER_GROUP = 64;

const int K_DIR_ITEM_PER_BLOCK = 28;

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

//Inode 块起始地址, 1 条 inode 项 512 bytes， 所以4 kB 总共有64block
const int K_INODE_STARTADDR = 66;

//data block 起始块地址,因为总共有16个block 用以存储inode
const int K_DATA_BLOCK_START_ADDR = 130;

//单个文件最大尺寸bytes为单位最大4096 + 48KB
const int K_FILE_MAX_SIZE_KB = 4096+48;


//inode 位图声明
extern bool inode_bitmap[K_INODE_NUM];
extern bool block_bitmap[K_BLOCK_NUM];
extern int  g_root_dir_inode_addr;
extern char g_current_user_name[100];
extern char g_current_host_name[100];
extern char g_current_group_name[100];
extern char g_current_user_dir_name[100];
extern char g_current_dir_name[100];
extern bool g_is_login;
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
	unsigned short i_mode;//，确定了文件的类型，以及它的所有者、它的group、其它用户访问此文件的权限。
	unsigned short i_counter; //总共有几个目录项
	char i_uname[20];
	char i_gname[20];
	unsigned int i_size;
	
	time_t i_ctime;
	time_t i_mtime;
	time_t i_atime;
	int i_dirBlock[12]; //这是inode直接指向的12 物理块， 也就是12*4 = 48 KB
	int i_indirBlock_1; //这是一级间接块，也就是用一个block来记录该文件剩下存储的block地址，一个地址4bytes， 所以是： 4KB / 4 * 4KB = 4M
	//合起来单个文件最大为 4096 + 48KB
};

//目录item 声明
struct DirItem {
	char itemName[K_MAX_NAME_SIZE];
	int inode_addr;
};

#endif
