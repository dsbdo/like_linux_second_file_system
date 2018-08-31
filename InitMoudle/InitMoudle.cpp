#include <cstring>
#include<memory.h>
#include "InitMoudle.h"

InitMoudle::InitMoudle(){
	m_file_process = new FileProcess();
}

InitMoudle::~InitMoudle() {}

//完成磁盘的划分，超级块与inode块的写入，同时挂载根目录， root 目录， home 目录
int InitMoudle::initDisk()
{
	initSuperBlock();
	initInodeBitMap();
	initBlockBitMap();

	m_file_process->loadBitMap();
	createRootDir();
	return 0;
}

bool InitMoudle::initSuperBlock()
{
	SuperBlock *init_superblock = new SuperBlock();
	init_superblock->s_Inode_num = (unsigned int)K_INODE_NUM;
	init_superblock->s_block_num = (unsigned int)K_BLOCK_NUM;
	init_superblock->s_free_Inode_num = (unsigned int)K_INODE_NUM;
	init_superblock->s_free_block_num = (unsigned int)K_BLOCK_NUM;

	//这里指的是存储数据块的开始地址
	init_superblock->s_free_data_block_addr = K_DATA_BLOCK_START_ADDR;
	init_superblock->s_block_size = K_BLOCK_SIZE;
	init_superblock->s_Inode_size = K_INODE_SIZE;
	init_superblock->s_superblock_size = sizeof(SuperBlock);
	init_superblock->s_blocks_per_group = K_BLOCKS_PER_GROUP;

	//磁盘分布
	init_superblock->s_superblock_startAddr = K_SUPERBLOCK_STARTADDR;
	init_superblock->s_InodeBitmap_startAddr = K_INODE_BITMAP_STARTADDR;
	init_superblock->s_blockBitmap_startAddr = K_BLOCK_BITMAP_STARTADDR;
	init_superblock->s_Inode_startAddr = K_INODE_STARTADDR;
	init_superblock->s_data_block_startAddr = K_DATA_BLOCK_START_ADDR;

	std::cout << "super block size = " << sizeof(SuperBlock) << " bytes" << std::endl;
	bool write_result = m_file_process->writeFile((char *)init_superblock, K_SUPERBLOCK_STARTADDR, sizeof(SuperBlock));
	if (write_result)
	{
		std::cout << "write block success, test write" << std::endl;
		//m_file_process->testWriteResult();
		return true;
	}
	else
	{
		return false;
	}

	//创建根目录
}

bool InitMoudle::initInodeBitMap() {
	// inode bitmap 写入
	memset(inode_bitmap, 0, sizeof(inode_bitmap));
	//磁盘头为disk_head;
	bool write_result = m_file_process->writeFile((char *)inode_bitmap, K_INODE_BITMAP_STARTADDR, sizeof(inode_bitmap));
	if (write_result)
	{
		//测一下inodebitmap 写了多少个block，因为仅有一个
		//m_file_process->setBlockBitmap(K_INODE_BITMAP_STARTADDR);
		return true;
	}
	else
	{
		//写失败
		return false;
	}
}
bool InitMoudle::initBlockBitMap() {

	memset(block_bitmap, 0, sizeof(block_bitmap));
	for(int i =0; i < K_DATA_BLOCK_START_ADDR; i++) {
		block_bitmap[i] = 1;
	}
	//这里需要64个block 来描述 block bitmap
	bool write_result = m_file_process->writeFile((char *)block_bitmap, K_BLOCK_BITMAP_STARTADDR, sizeof(block_bitmap));

	if (write_result)
	{
		return true;
	}
	else
	{
		//写失败
		return false;
	}
	//block_bitmap 也需要写入
	//将超级块写入
}

//创建根目录
bool InitMoudle::createRootDir() {
	//Inode root_inode
	//这里准备好材料，理论是需要询问是否inode 与 block 块的
	//因为是初始化所以就不准备了
	Inode root_dir_inode;

	//inode -> block , block 记录着目录的所有信息
	//根目录最多下辖28个目录或文件,一个block最多28条目录项记录
	DirItem dirlist[28] = {0};
	//插入目录项
	strcpy(dirlist[0].itemName,".");
	strcpy(dirlist[1].itemName, "..");
	int block_addr = m_file_process->allocBlock();
	int inode_addr = m_file_process->allocInode();
	std::cout <<  "alloc inode num for root dir is: " << inode_addr << std::endl;
	std::cout << "alloc block num for root dir is: " << block_addr << std::endl;
	if(block_addr != -1 && inode_addr != -1) {
		//能够分配到正确的节点地址
		dirlist[0].inode_addr = inode_addr;
		dirlist[1].inode_addr = inode_addr;
		//写磁盘 132 * 4 * 4  < 1024 *4
		m_file_process->writeBlockFile((char*)dirlist, block_addr, sizeof(dirlist));
		
		
		/*
		*
		* 测试函数
		*/
		m_file_process->testWriteBlock(block_addr);
		/*
		*
		* 测试函数结束
		*/


		//创建inode
		root_dir_inode.i_Inode_num = inode_addr;
		//上一次访问时间
		root_dir_inode.i_atime = time(NULL);
		//上次文件修改时间
		root_dir_inode.i_ctime = time(NULL);
		//上次权限修改时间
		root_dir_inode.i_mtime = time(NULL);

		strcpy(root_dir_inode.i_uname, g_current_user_name);
		strcpy(root_dir_inode.i_gname, g_current_group_name);
		root_dir_inode.i_counter = 2;
		root_dir_inode.i_dirBlock[0] = block_addr;
		for(int i = 1; i < 12; i++) {
			root_dir_inode.i_dirBlock[i] = -1;
		}
		root_dir_inode.i_size = K_BLOCK_SIZE;
		root_dir_inode.i_indirBlock_1 = -1;

		root_dir_inode.i_mode = MODE_DIR | DIR_DEF_PERMISSION;

		//写入磁盘
		m_file_process->writeInode((char*)&root_dir_inode, inode_addr);


		/*
		*
		* 测试函数
		*/
		m_file_process->testWriteInode(inode_addr);
		/*
		*
		* 测试函数结束
		*/

		//根目录创建成功，配备其他文件目录

		m_file_process->mkdir(root_dir_inode.i_Inode_num, "home");
		std::cout << "********************test mkdir home*************************" << std::endl;
			
			m_file_process->testWriteInode(inode_addr);

			m_file_process->testWriteBlock(block_addr);

		m_file_process->mkdir(root_dir_inode.i_Inode_num, "etc");
		std::cout << "********************test mkdir etc*************************" << std::endl;
			m_file_process->testWriteInode(inode_addr);
		    m_file_process->testWriteBlock(block_addr);
			m_file_process->mkdir(g_root_dir_inode_addr, "dsbdo");
			char buf[] = "/dsbdo";
			
			m_file_process->cd(0, buf, 1);
			m_file_process->mkdir(g_current_dir_inode_addr, "helloWorld");

			// m_file_process->ls(g_root_dir_inode_addr);
			// m_file_process->cd(g_current_dir_inode_addr,"..");
			// m_file_process->create(g_root_dir_inode_addr,"dsbdo.sys", "china is a great couintry");
			// m_file_process->del(g_root_dir_inode_addr, "dsbdo.sys");
			
			// m_file_process->cd(g_root_dir_inode_addr,"etc");
			// m_file_process->mkdir(g_current_dir_inode_addr, "test_dir");
			// m_file_process->create(g_current_dir_inode_addr,"dsbdo.sys", "china is a great couintry");
			// m_file_process->del(g_current_dir_addr, "dsbdo.sys");

			//m_file_process->rmall();
			//m_file_process->rmdir(g_root_dir_inode_addr,"etc");
		return true;
	}
	else {
		std::cout << "create root dir fail!" << std::endl;
		return false;
	}

}

//文件系统启动
void InitMoudle::bootFileSystem(){
	//读取超级块
	m_file_process->loadSuperBlock();
	//加载位图
	m_file_process->loadBitMap();
}

FileProcess* InitMoudle::getFileProcess() const {
	return m_file_process;
}