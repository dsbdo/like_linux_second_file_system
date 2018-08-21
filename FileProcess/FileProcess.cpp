
#include <fstream>
#include <iostream>
#include<string.h>
#include "FileProcess.h"
FileProcess::FileProcess()
{
	//disk_head.open("")
	using std::ios;
	disk_head.open(K_SYSTEM_FILE, ios::in | ios::out | ios::binary);
	m_free_block_num = K_BLOCK_NUM - 82;
	m_free_inode_num = K_INODE_NUM;
}

FileProcess::~FileProcess()
{
}
bool FileProcess::writeFile(char content[], int block_addr, int size_byte)
{
	using std::cout;
	using std::endl;
	using std::fstream;
	using std::ios;
	disk_head.seekp(block_addr * K_BLOCK_SIZE, ios::beg);
	disk_head.write(content, size_byte);
	if (disk_head.good())
	{
		//写入文件成功，打印写入的内容
		// for (int i = 0; i < 512; i++)
		// {
		// 	cout << content[i] << " " << endl;
		// 	if ((i + 1) % 32 == 0)
		// 	{
		// 		cout << endl;
		// 	}
		// }
		cout << "写入成功" << endl;
		return true;
	}
	else
	{
		cout << "写入失败" << endl;
		return false;
	}

	//}
}

bool FileProcess::writeBlockFile(char content[], int block_addr, int size_byte)
{
	//一次4KB
	if (size_byte > 1024 * 4)
	{
		std::cout << "block file to big, write fail!" << std::endl;
	}
	else if (size_byte < 1024 * 4)
	{
		for (int i = 0; i < size_byte; i++)
		{
			buf_4KB[i] = content[i];
		}
		for (int i = size_byte; i < K_BLOCK_SIZE - 1; i++)
		{
			buf_4KB[i] = '0';
		}
		buf_4KB[K_BLOCK_SIZE - 1] = '\0';

		disk_head.seekp(block_addr * K_BLOCK_SIZE, std::ios::beg);
		disk_head.write(buf_4KB, K_BLOCK_SIZE);
	}
	else
	{
		//一次写一个磁盘块
		disk_head.seekp(block_addr * 1024 * 4, std::ios::beg);
		disk_head.write(content, 1024 * 4);
	}

	if (disk_head.good())
	{

		setBlockBitmap(block_addr);
		std::cout << "写入块文件成功" << std::endl;
		return true;
	}
	else
	{
		std::cout << "写入块文件失败" << std::endl;
		return false;
	}
}

bool FileProcess::writeInode(char content[], int inode_addr, int size_byte)
{
	disk_head.seekp(K_INODE_STARTADDR * K_BLOCK_SIZE + inode_addr * K_INODE_SIZE, std::ios::beg);
	disk_head.write(content, K_INODE_SIZE);
	if (disk_head.good())
	{
		//修改m_inode_bit_map, 并更新磁盘中的inode bitmap
		m_inode_bitmap[inode_addr] = 1;
		setInodeBitmap(inode_addr);
		std::cout << "write inode success" << std::endl;
	}
	else
	{
		std::cout << "write inode fail" << std::endl;
	}
}

//为inode 置位
bool FileProcess::setInodeBitmap(int inode_addr)
{
	m_inode_bitmap[inode_addr] = 1;
	m_free_inode_num--;
	using std::ios;
	disk_head.seekp(K_INODE_BITMAP_STARTADDR * K_BLOCK_SIZE + inode_addr, ios::beg);
	bool flag = 1;
	//存储到系统中的值是 0x01
	disk_head.write((char *)&flag, 1);
	if (disk_head.good())
	{
		return true;
	}
	else
	{
		return false;
	}
}
//为block 置位
bool FileProcess::setBlockBitmap(int block_addr)
{
	m_block_bitmap[block_addr] = 1;
	m_free_block_num--;
	using std::cout;
	using std::endl;
	using std::ios;
	disk_head.seekp(K_BLOCK_BITMAP_STARTADDR * K_BLOCK_SIZE + block_addr, ios::beg);
	bool flag = 1;
	//存储到系统中的值是 0x01
	disk_head.write((char *)&flag, 1);
	if (disk_head.good())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void FileProcess::loadBitMap()
{
	//seekg seek get 的意思
	disk_head.seekg(K_INODE_BITMAP_STARTADDR * 1024 * 4, std::ios::beg);
	disk_head.read(buf_4KB, 1024 * 4);
	//m_inode_bitmap  = (bool *)&buf_4KB;
	for (int i = 0; i < K_INODE_NUM; i++)
	{
		m_inode_bitmap[i] = (bool)buf_4KB[i];
	}

	std::cout << "DEBUG loadBitmap print m_inode_bitmap check:   !!!" << std::endl;
	for (int i = 0; i < 512; i++)
	{
		std::cout << m_inode_bitmap[i] << "|";
		if ((i + 1) % 64 == 0)
		{
			std::cout << std::endl;
		}
	}
	//需要读K_BLOCK_NUM /(1024*4) 块
	for (int i = 0; i < K_BLOCK_NUM / (1024 * 4); i++)
	{
		disk_head.seekg((K_BLOCK_BITMAP_STARTADDR + i) * 1024 * 4, std::ios::beg);
		disk_head.read(buf_4KB, 1024 * 4);
		for (int j = 0; j < 1024 * 4 && i * 1024 * 4 + j < K_BLOCK_NUM; j++)
		{
			m_block_bitmap[i * 1024 * 4 + j] = buf_4KB[j];
		}
	}

	std::cout << "m_block_bitmap check:   !!!" << std::endl;
	for (int i = 0; i < 1024; i++)
	{
		std::cout << m_block_bitmap[i] << "|";
		if ((i + 1) % 64 == 0)
		{
			std::cout << std::endl;
		}
	}
}

void FileProcess::loadSuperBlock()
{
	disk_head.seekg(0, std::ios::beg);
	disk_head.read(buf_4KB, K_BLOCK_SIZE);
	m_super_block = (SuperBlock *)&buf_4KB;

	std::cout << "打印超级块信息： ×××××××××" << std::endl;
	std::cout << "super->s_block_num: " << m_super_block->s_block_num << std::endl;
	std::cout << "super->s_block_size : " << m_super_block->s_block_size << std::endl;
	std::cout << "super->s_blockBitmap_startAddr: " << m_super_block->s_blockBitmap_startAddr << std::endl;
	using std::cout;
	using std::endl;
	cout << " super->s_blocks_per_group " << m_super_block->s_blocks_per_group << endl;
	cout << "super->s_data_block_startAddr: " << m_super_block->s_data_block_startAddr << endl;
	cout << "super->s_free_data_block_addr " << m_super_block->s_free_data_block_addr << endl;
	cout << "super->s_free_Inode_num: " << m_super_block->s_free_Inode_num << endl;
	cout << "super->s_Inode_size: " << m_super_block->s_Inode_size << endl;
	cout << "super->s_Inode_startAddr: " << m_super_block->s_Inode_startAddr << endl;
	cout << "super->s_InodeBitmap_startAddr: " << m_super_block->s_InodeBitmap_startAddr << endl;
	cout << "super->s_superblock_size: " << m_super_block->s_superblock_size << endl;
	cout << "super->s_superblock_startAddr: " << m_super_block->s_superblock_startAddr << endl;
	std::cout << "打印超级块信息： ××××××××× 打印结束" << std::endl;
}

bool FileProcess::isFreeInode()
{
	if (m_free_inode_num > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool FileProcess::isFreeBlock()
{
	if (m_free_block_num > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

int FileProcess::allocInode()
{
	using std::cout;
	using std::endl;
	if (isFreeInode())
	{
		//寻找一下，看那一个INODE是空的，寻找对象 Inode_bitmap
		for (int i = 0; i < K_INODE_NUM; i++)
		{
			if (!m_inode_bitmap[i])
			{
				//这一个是空的inode项
				return i;
			}
		}
		cout << "error in find free inode" << endl;
		return -1;
	}
	else
	{
		cout << "No inode free" << endl;
		return -1;
	}
}

int FileProcess::allocBlock()
{
	using std::cout;
	using std::endl;
	if (isFreeBlock())
	{
		for (int i = 0; i < K_BLOCK_NUM; i++)
		{
			if (!m_block_bitmap[i])
			{
				return i;
			}
		}
		cout << "error in find free block" << endl;
		return -1;
	}
	else
	{
		cout << "No inode free" << endl;
		return -1;
	}
}

bool FileProcess::freeFile()
{
	//这里就是找出文件的Inode地址即可
}

bool FileProcess::freeBlock(int block_addr)
{
	if (block_addr < K_DATA_BLOCK_START_ADDR)
	{
		std::cout << "释放违规块，不允许释放系统块 " << std::endl;
		return false;
	}
	else
	{
		//仅需要将bitmap设置为零即可
		if (m_block_bitmap[block_addr] == 0)
		{
			std::cout << "该磁盘块未使用，无法释放" << std::endl;
			return false;
		}
		else
		{
			//更新内存位图
			m_block_bitmap[block_addr] = 0;

			//更新磁盘block 位图
			disk_head.seekp(K_BLOCK_BITMAP_STARTADDR * K_BLOCK_SIZE + block_addr, std::ios::beg);
			bool flag = 0;
			//存储到系统中的值是 0x00
			disk_head.write((char *)&flag, 1);
		}
	}
}

bool FileProcess::freeInode(int inode_addr)
{
	if (inode_addr < 0 || inode_addr > K_INODE_NUM)
	{
		std::cout << "释放不合法的inode， 错误释放" << std::endl;
	}
	else
	{
		//读出inode 将所有block 进行释放
		char inode_buf[sizeof(Inode)];
		disk_head.seekg(K_INODE_STARTADDR * K_BLOCK_SIZE + inode_addr * K_INODE_SIZE, std::ios::beg);
		//读取inode信息
		char buf[K_INODE_SIZE];
		disk_head.read(buf, sizeof(Inode));
		Inode *temp_inode = (Inode *)buf;
		//前面inode 结构体中 已经说明直接指向的块最多为12块
		for (int i = 0; i < 12; i++)
		{
			if (temp_inode->i_dirBlock[i] != -1)
			{
				freeBlock(temp_inode->i_dirBlock[i]);
			}
		}

		//证明有间接块，加载间接块，
		if (temp_inode->i_indirBlock_1 != -1)
		{
			//加载该物理块
			disk_head.seekg(temp_inode->i_indirBlock_1 * K_BLOCK_SIZE, std::ios::beg);
			disk_head.read(buf_4KB, K_BLOCK_SIZE);
			//将一个char 数组转化为 int 数组
			int *blocks_need_free = (int *)buf_4KB;
			for (int i = 0; i < K_BLOCK_SIZE / 4; i++)
			{
				//证明是合法数据库才删
				if (blocks_need_free[i] >= K_DATA_BLOCK_START_ADDR)
				{
					freeBlock(blocks_need_free[i]);
				}
			}
		}
		//更新inode bitmap
		m_inode_bitmap[inode_addr] = 0;

		//更新内存inode 位图
		disk_head.seekp(K_INODE_BITMAP_STARTADDR * K_BLOCK_SIZE + inode_addr, std::ios::beg);
		bool flag = 0;
		disk_head.write((char *)&flag, 1);
	}
}

void FileProcess::readInode(Inode *inode_item, int inode_addr)
{
	using std::cout;
	using std::endl;
	using std::ios;
	disk_head.seekg(K_INODE_STARTADDR * K_BLOCK_SIZE + inode_addr * K_INODE_SIZE, ios::beg);
	//读取出512字节
	char buf[K_INODE_SIZE];
	disk_head.read(buf, K_INODE_SIZE);
	memcpy(inode_item, buf, K_BLOCK_SIZE);
	cout << "test read Inode: " << endl;
	
	cout << "test inode item inode num: " << inode_item->i_Inode_num << endl;
	cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_mode: " <<inode_item->i_mode<< endl;
	cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_counter:" <<inode_item->i_counter<< endl;
	cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_uname:" <<inode_item->i_uname<< endl;
	cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_gname:" <<inode_item->i_gname<< endl;
	cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_size:" <<inode_item->i_size<< endl;
	cout << "DEBUG::FILEPROCESS::readInode 382 i_ctime" <<inode_item->i_ctime << " "  << inode_item->i_mtime << " "<< inode_item->i_atime<< endl;
	for(int i = 0; i < 12; i++) {
		cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_dirBlock:" <<inode_item->i_dirBlock[i]<< endl;
	}
	cout << "test inode item inode indirBlock: " << inode_item->i_indirBlock_1 << endl;
}

void FileProcess::readBlock(char *&block_buf, int block_addr)
{
	//读出一个block
	disk_head.seekg(block_addr * K_BLOCK_SIZE, std::ios::beg);
	disk_head.read(block_buf, K_BLOCK_SIZE);
}
//创建目录
bool FileProcess::mkdir(int parent_inode_addr, const char dir_name[])
{
	if (strlen(dir_name) >= K_MAX_NAME_SIZE)
	{
		std::cout << "超过最大目录名长度" << std::endl;
	}
	//按理说目录就是一个文件罢了,想要创建多少个都是随便创建的
	//临时目录项，一个块最多28条目录项记录，因为sizeof(DirItem) = 132 byte, 一个block 4KB
	DirItem dirlist[28];

	Inode *current_inode = new Inode();

	readInode(current_inode, parent_inode_addr);

	testWriteInode(parent_inode_addr);
	std::cout << "DEBUG test in call mkdir function: " << current_inode->i_Inode_num << std::endl;

	int dir_item_cnt = 0; //遍历到第几个目录item
	//目录项数 +1
	int cnt = current_inode->i_counter + 1;
	int posi = -1, posj = -1;
	bool is_new_block = false;
	//直接块12 个， 每块28条目录项记录,找到最新可插入目录项，不想搞那么多了，就最多的目录项设置为28*12吧
	//这里需要考虑到用户一旦删除一个目录的话，就会出现一个空的目录项
	while (dir_item_cnt < 28 * 12)
	{
		int dir_block_num = dir_item_cnt / 28;

		//如果其中一块的目录项被全部清空删除了呢？在这里就不能直接跳过了，应该申请块同时插进新的块，而非继续往下走
		if (current_inode->i_dirBlock[dir_block_num] == -1)
		{
			//申请一个块进行写入
			int new_dir_block = allocBlock();
			if (new_dir_block != -1)
			{
				posi = new_dir_block;
				posj = 0;
				is_new_block = true;
				current_inode->i_dirBlock[dir_block_num] = new_dir_block;
			}
			else
			{
				std::cout << "DEBUG::FILEPROCESS::mkdir 431 block 分配失败，创建目录失败" << std::endl;
			}
		}
		else
		{
			//取出直接块
			char *block_buf = new char[K_BLOCK_SIZE];
			readBlock(block_buf, current_inode->i_dirBlock[dir_block_num]);

			//没有办法强制转型，所以采用内存复制的方式
			memcpy(dirlist, block_buf, sizeof(DirItem) * 28);

			delete[] block_buf;

			for (int j = 0; j < K_DIR_ITEM_PER_BLOCK; j++)
			{
				//同名，检查是否是文件名，如果一直没有检查同名文件夹目录，那就直接到最后一项添加多一项进去
				if (strcmp(dirlist[j].itemName, dir_name) == 0)
				{
					Inode *tmp_inode = new Inode();
					readInode(tmp_inode, dirlist[j].inode_addr);

					//该目录已经存在
					if (((tmp_inode->i_mode >> 9) & 1) == 1)
					{
						printf("目录已存在\n");
						return false;
					}
				}
				else if (strcmp(dirlist[j].itemName, "") == 0 || dir_item_cnt == (current_inode->i_counter + 1))
				{
					//找到一个空闲记录，将新目录创建到这个位置
					//记录这个位置
					if (posi == -1)
					{
						std::cout << "DEBUG::FILEPROCESS::mkdir dir 471 block position: " <<  current_inode->i_dirBlock[dir_block_num] << std::endl;
						std::cout << "DEBUG::FILEPROCESS::mkdir dir 472 item position: " << j << std::endl;
						posi =  current_inode->i_dirBlock[dir_block_num];
						posj = j;
						break;
					}
				}
				dir_item_cnt++;
				//超过最大目录项数
				if (dir_item_cnt == 28 * 12)
				{
					posi = -2;
					break;
				}
			}
		}

		if (posi != -1)
		{
			break;
		}
	}
	//posi == -2 证明 目录项已经超过最大目录项了，因为不想理那个简接块了， 所以最大就设置成28*12
	if (posi != -2)
	{
		//是否是新申请的块
		if (is_new_block)
		{
			memset(dirlist, 0, sizeof(dirlist));
		}
		else
		{
			char *block_buf = new char[K_BLOCK_SIZE];
			readBlock(block_buf, posi);
			memcpy(dirlist, block_buf, sizeof(DirItem) * 28);
			for (int debug_i = 0; debug_i < 28; debug_i++)
			{
				std::cout << "DEBUG::FILEPROCESS::mkdir 505 dirlist: dir_name:" << dirlist[debug_i].itemName << " |||dir->inode is: " << dirlist[debug_i].inode_addr << std::endl;
			}
			delete[] block_buf;
		}

		//创建目录名
		strcpy(dirlist[posj].itemName, dir_name);

		int child_dir_inode_addr = allocInode();
		if (child_dir_inode_addr == -1)
		{
			std::cout << "DEBUG::FILEPROCESS::mkdir 519 inode 分配失败" << std::endl;
			return false;
		}
		dirlist[posj].inode_addr = child_dir_inode_addr;
		for (int debug_i = 0; debug_i < 28; debug_i++)
		{
			std::cout << "DEBUG::FILEPROCESS::mkdir 521 dirlist: dir_name:" << dirlist[debug_i].itemName << " |||dir->inode is: " << dirlist[debug_i].inode_addr << std::endl;
		}
		//为新目录分配inode
		Inode *new_dir_inode = new Inode();
		new_dir_inode->i_Inode_num = child_dir_inode_addr;
		new_dir_inode->i_atime = time(NULL);
		new_dir_inode->i_ctime = time(NULL);
		new_dir_inode->i_mtime = time(NULL);
		strcpy(new_dir_inode->i_uname, g_current_user_name);
		strcpy(new_dir_inode->i_gname, g_current_group_name);
		//目录项数
		new_dir_inode->i_counter = 2;
		//分配对应inode的磁盘块,用以写这个新目录的目录
		int child_dir_block_addr = allocBlock();
		if (child_dir_block_addr == -1)
		{
			std::cout << "DEBUG::FILEPROCESS::mkdir 525 block 分配失败" << std::endl;
			return false;
		}

		DirItem dirlist2[28] = {0};
		strcpy(dirlist2[0].itemName, ".");
		strcpy(dirlist2[1].itemName, "..");
		dirlist2[0].inode_addr = child_dir_inode_addr; //当前目录地址
		dirlist2[1].inode_addr = parent_inode_addr;	//上一级目录地址

		//写入磁盘块
		writeBlockFile((char *)dirlist2, child_dir_block_addr, K_BLOCK_SIZE);
		std::cout << "DEBUG::FILEPROCESS::mkdir 545 testWriteBlock start$$$$$$$$$$$$$$$$$$$$" << std::endl;
		testWriteBlock(child_dir_block_addr);
		std::cout << "DEBUG::FILEPROCESS::mkdir 545 testWriteBlock over$$$$$$$$$$$$$$$$$$$$$" << std::endl;

		new_dir_inode->i_dirBlock[0] = child_dir_block_addr;
		for (int k = 1; k < 12; k++)
		{
			new_dir_inode->i_dirBlock[k] = -1;
		}
		new_dir_inode->i_size = K_BLOCK_SIZE;
		new_dir_inode->i_indirBlock_1 = -1;
		new_dir_inode->i_mode = MODE_DIR | DIR_DEF_PERMISSION;
		//将inode 进行写入
		writeInode((char *)new_dir_inode, child_dir_inode_addr);
		std::cout << "DEBUG::FILEPROCESS::mkdir 559 testWriteInode start$$$$$$$$$$$$$$$$$$$$" << std::endl;
		testWriteInode(child_dir_inode_addr);
		std::cout << "DEBUG::FILEPROCESS::mkdir 559 testWriteInode over$$$$$$$$$$$$$$$$$$$$$" << std::endl;
		//将当前块写回

		for (int debug_i = 0; debug_i < 28; debug_i++)
		{
			std::cout << "DEBUG::FILEPROCESS::mkdir 570 dirlist: dir_name:" << dirlist[debug_i].itemName << " |||dir->inode is: " << dirlist[debug_i].inode_addr << std::endl;
		}
		writeBlockFile((char *)dirlist, posi, K_BLOCK_SIZE);
		std::cout << "DEBUG::FILEPROCESS::mkdir 566 testWriteBlock parent_block start$$$$$$$$$$$$$$$$$$$$" << std::endl;
		testWriteBlock(posi);
		std::cout << "DEBUG::FILEPROCESS::mkdir 566 testWriteBlock parent_block over$$$$$$$$$$$$$$$$$$$$$" << std::endl;
		//将当前inode 进行写回
		current_inode->i_counter++;
		writeInode((char *)current_inode, parent_inode_addr);

		std::cout << "DEBUG::FILEPROCESS::mkdir 572 testWriteInode parent_inode_addr start$$$$$$$$$$$$$$$$$$$$" << std::endl;
		testWriteInode(parent_inode_addr);
		std::cout << "DEBUG::FILEPROCESS::mkdir 572 testWriteInode parent_inode_addr over$$$$$$$$$$$$$$$$$$$$$" << std::endl;
		return true;
	}
	else
	{
		std::cout << "error in mkdir, over max dir item" << std::endl;
		return false;
	}
}

//  //这个不是很好实现，先不处理
// void FileProcess::rmall(int parent_inode_addr) {
// 	//取出Inode
// 	Inode* current_inode;
// 	readInode(current_inode, parent_inode_addr);

// 	//取出目录项数
// 	if(current_inode->i_counter <= 2) {
// 		freeBlock(current_inode->i_dirBlock[0]);
// 		freeInode(parent_inode_addr);
// 		return ;
// 	}

// 	int counter = 0;
// 	while(counter < 28 * 12) {
// 		DirItem dirlist[28] = {0};
// 		if(current_inode->i_dirBlock[counter/28] == -1) {
// 			counter += 28;
// 			continue;
// 		}

// 		//取出磁盘块
// 		int parent_block_addr = current_inode->i_dirBlock[counter/28];
// 		char* block_buf = new char [K_BLOCK_SIZE];
// 		readBlock(block_buf, parent_block_addr);
// 		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);
// 		delete[] block_buf;

// 		//从磁盘块依次取出目录项，递归删除
// 		bool flag = false;
// 		for(int j = 0; j < K_DIR_ITEM_PER_BLOCK; j++) {
// 			if( ! (strcmp(dirlist[j].itemName,".")==0 || strcmp(dirlist[j].itemName,"..")==0 || strcmp(dirlist[j].itemName,"")==0 ) ){
// 				flag = true;
// 				rmall(dirlist[j].inode_addr);
// 		}
// 		counter++;

// 	}

// 	}

// }

void FileProcess::rmall(int parinoAddr) //删除该节点下所有文件或目录
{
	//从这个地址取出inode
	Inode *cur = new Inode();
	readInode(cur, parinoAddr);

	//取出目录项数
	int cnt = cur->i_counter;
	if (cnt <= 2)
	{
		freeBlock(cur->i_dirBlock[0]);
		freeInode(parinoAddr);
		return;
	}

	//依次取出磁盘块
	int i = 0;
	while (i < 28 * 12)
	{ //小于160
		DirItem dirlist[28] = {0};
		if (cur->i_dirBlock[i / 28] == -1)
		{
			i += 28;
			continue;
		}
		//取出磁盘块
		int parblockAddr = cur->i_dirBlock[i / 28];
		char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(block_buf, parblockAddr);
		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);

		//从磁盘块中依次取出目录项，递归删除
		int j;
		bool f = false;
		for (j = 0; j < 16; j++)
		{
			//Inode tmp;
			if (!(strcmp(dirlist[j].itemName, ".") == 0 ||
				  strcmp(dirlist[j].itemName, "..") == 0 ||
				  strcmp(dirlist[j].itemName, "") == 0))
			{
				f = true;
				rmall(dirlist[j].inode_addr); //递归删除
			}

			cnt = cur->i_counter;

			i++;
		}

		//该磁盘块已空，回收
		if (f)
			freeBlock(parblockAddr);
	}
	//该inode已空，回收
	freeInode(parinoAddr);
	return;
}
bool FileProcess::create(int parinoAddr, char name[], char buf[]) //创建文件函数，在该目录下创建文件，文件内容存在buf
{
	if (strlen(name) >= K_MAX_NAME_SIZE)
	{
		printf("超过最大文件名长度\n");
		return false;
	}

	DirItem dirlist[28]; //临时目录清单

	//从这个地址取出inode
	Inode *cur = new Inode();
	readInode(cur, parinoAddr);

	int i = 0;
	int posi = -1, posj = -1; //找到的目录位置
	int dno;
	int cnt = cur->i_counter + 1; //目录项数
	while (i < 28 * 12)
	{
		//160个目录项之内，可以直接在直接块里找
		dno = i / 28; //在第几个直接块里

		if (cur->i_dirBlock[dno] == -1)
		{
			i += 16;
			continue;
		}

		char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(block_buf, cur->i_dirBlock[dno]);
		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);
		delete[] block_buf;
		//输出该磁盘块中的所有目录项
		int j;
		for (j = 0; j < 28; j++)
		{

			if (posi == -1 && strcmp(dirlist[j].itemName, "") == 0)
			{
				//找到一个空闲记录，将新文件创建到这个位置
				posi = dno;
				posj = j;
			}
			else if (strcmp(dirlist[j].itemName, name) == 0)
			{
				//重名，取出inode，判断是否是文件
				Inode *cur2 = new Inode();
				readInode(cur2, dirlist[j].inode_addr);
				if (((cur2->i_mode >> 9) & 1) == 0)
				{ //是文件且重名，不能创建文件
					printf("文件已存在\n");
					buf[0] = '\0';
					return false;
				}
			}
			i++;
		}
	}
	if (posi != -1)
	{
		//之前找到一个目录项了
		//取出之前那个空闲目录项对应的磁盘块
		// fseek(fr,cur.i_dirBlock[posi],SEEK_SET);
		// fread(dirlist,sizeof(dirlist),1,fr);
		// fflush(fr);
		char *block_buf = new char[K_BLOCK_SIZE];

		readBlock(block_buf, dno);
		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);
		//创建这个目录项
		strcpy(dirlist[posj].itemName, name); //文件名
		int chiinoAddr = allocInode();		  //分配当前节点地址
		if (chiinoAddr == -1)
		{
			printf("inode分配失败\n");
			return false;
		}
		dirlist[posj].inode_addr = chiinoAddr; //给这个新的目录分配的inode地址

		//设置新条目的inode
		Inode p;
		p.i_Inode_num = chiinoAddr;
		p.i_atime = time(NULL);
		p.i_ctime = time(NULL);
		p.i_mtime = time(NULL);
		strcpy(p.i_uname, g_current_user_name);
		strcpy(p.i_gname, g_current_group_name);
		p.i_counter = 1; //只有一个文件指向

		//将buf内容存到磁盘块
		int k;
		int len = strlen(buf); //文件长度，单位为字节
		for (k = 0; k < len; k += K_BLOCK_SIZE)
		{ //最多12次，12个磁盘快，即最多5K
			//分配这个inode的磁盘块，从控制台读取内容
			int curblockAddr = allocBlock();
			if (curblockAddr == -1)
			{
				printf("block分配失败\n");
				return false;
			}
			p.i_dirBlock[k / K_BLOCK_SIZE] = curblockAddr;
			//写入到当前目录的磁盘块
			//情况不明待调试
			writeBlockFile(buf + k, curblockAddr);
		}
		//剩下的物理块全部置为 -1
		for (k = len / K_BLOCK_SIZE + 1; k < 12; k++)
		{
			p.i_dirBlock[k] = -1;
		}

		if (len == 0)
		{ //长度为0的话也分给它一个block
			int curblockAddr = allocBlock();
			if (curblockAddr == -1)
			{
				printf("block分配失败\n");
				return false;
			}
			p.i_dirBlock[k / K_BLOCK_SIZE] = curblockAddr;
			//写入到当前目录的磁盘块
			writeBlockFile(buf, curblockAddr, 0);
		}
		p.i_size = len;
		p.i_indirBlock_1 = -1; //没使用一级间接块
		p.i_mode = 0;
		p.i_mode = MODE_FILE | FILE_DEF_PERMISSION;

		//将inode写入到申请的inode地址
		writeInode((char *)&p, chiinoAddr);

		//将当前目录的磁盘块写回
		writeBlockFile((char *)dirlist, cur->i_dirBlock[posi]);

		//写回inode
		cur->i_counter++;
		writeInode((char *)cur, parinoAddr);
		return true;
	}
	else
		return false;
}

bool FileProcess::del(int parinoAddr, char name[]) //删除文件函数。在当前目录下删除文件
{
	if (strlen(name) >= K_MAX_NAME_SIZE)
	{
		printf("超过最大目录名长度\n");
		return false;
	}

	//从这个地址取出inode
	Inode *cur = new Inode();
	readInode(cur, parinoAddr);

	//取出目录项数
	int cnt = cur->i_counter;

	//判断文件模式。6为owner，3为group，0为other
	int filemode;
	if (strcmp(g_current_user_name, cur->i_uname) == 0)
		filemode = 6;
	else if (strcmp(g_current_group_name, cur->i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	if (((cur->i_mode >> filemode >> 1) & 1) == 0)
	{
		//没有写入权限
		printf("权限不足：无写入权限\n");
		return false;
	}

	//依次取出磁盘块
	int i = 0;
	while (i < 28 * 12)
	{ //小于160
		DirItem dirlist[28] = {0};

		if (cur->i_dirBlock[i / 28] == -1)
		{
			i += 28;
			continue;
		}
		//取出磁盘块
		int parblockAddr = cur->i_dirBlock[i / 28];
		char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(block_buf, parblockAddr);
		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);
		delete[] block_buf;

		//找到要删除的目录
		int pos;
		for (pos = 0; pos < 28; pos++)
		{
			Inode *tmp = new Inode();
			//取出该目录项的inode，判断该目录项是目录还是文件
			readInode(tmp, dirlist[pos].inode_addr);

			if (strcmp(dirlist[pos].itemName, name) == 0)
			{
				if (((tmp->i_mode >> 9) & 1) == 1)
				{   //找到目录
					//是目录，不管
				}
				else
				{
					//是文件

					//释放block
					int k;
					for (k = 0; k < 12; k++)
						if (tmp->i_dirBlock[k] != -1)
							freeBlock(tmp->i_dirBlock[k]);

					//释放inode
					freeInode(dirlist[pos].inode_addr);

					//删除该目录条目，写回磁盘
					strcpy(dirlist[pos].itemName, "");
					dirlist[pos].inode_addr = -1;
					writeBlockFile((char *)dirlist, parblockAddr);

					cur->i_counter--;

					writeInode((char *)cur, parinoAddr);
					return true;
				}
			}
			i++;
		}
	}

	printf("没有找到该文件!\n");
	return false;
}

void FileProcess::ls(int parinoAddr) //显示当前目录下的所有文件和文件夹。参数：当前目录的inode节点地址
{
	Inode *cur = new Inode();
	//取出这个inode
	readInode(cur, parinoAddr);

	//取出目录项数
	int cnt = cur->i_counter;

	//判断文件模式。6为owner，3为group，0为other
	int filemode;
	if (strcmp(g_current_user_name, cur->i_uname) == 0)
		filemode = 6;
	else if (strcmp(g_current_group_name, cur->i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	if (((cur->i_mode >> filemode >> 2) & 1) == 0)
	{
		//没有读取权限
		printf("权限不足：无读取权限\n");
		return;
	}

	//依次取出磁盘块
	int i = 0;
	while (i < cnt && i < 28 * 12)
	{
		DirItem dirlist[28] = {0};
		if (cur->i_dirBlock[i / 28] == -1)
		{
			i += 16;
			continue;
		}
		//取出磁盘块
		int parblockAddr = cur->i_dirBlock[i / 28];
		char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(block_buf, parblockAddr);
		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);
		delete[] block_buf;
		//输出该磁盘块中的所有目录项
		int j;
		for (j = 0; j < 16 && i < cnt; j++)
		{
			Inode *tmp = new Inode();
			//取出该目录项的inode，判断该目录项是目录还是文件
			readInode(tmp, dirlist[j].inode_addr);

			if (strcmp(dirlist[j].itemName, "") == 0)
			{
				continue;
			}

			//输出信息
			if (((tmp->i_mode >> 9) & 1) == 1)
			{
				printf("d");
			}
			else
			{
				printf("-");
			}

			tm *ptr; //存储时间
			ptr = gmtime(&(tmp->i_mtime));

			//输出权限信息
			int t = 8;
			while (t >= 0)
			{
				if (((tmp->i_mode >> t) & 1) == 1)
				{
					if (t % 3 == 2)
						printf("r");
					if (t % 3 == 1)
						printf("w");
					if (t % 3 == 0)
						printf("x");
				}
				else
				{
					printf("-");
				}
				t--;
			}
			printf("\t");

			//其它
			printf("%d\t", tmp->i_counter);																												//链接
			printf("%s\t", tmp->i_uname);																												//文件所属用户名
			printf("%s\t", tmp->i_gname);																												//文件所属用户名
			printf("%d B\t", tmp->i_size);																												//文件大小
			printf("%d.%d.%d %02d:%02d:%02d  ", 1900 + ptr->tm_year, ptr->tm_mon + 1, ptr->tm_mday, (8 + ptr->tm_hour) % 24, ptr->tm_min, ptr->tm_sec); //上一次修改的时间
			printf("%s", dirlist[j].itemName);																											//文件名
			printf("\n");
			i++;
		}
	}
	/*  未写完 */
}

void FileProcess::cd(int parinoAddr, char name[]) //进入当前目录下的name目录
{
	//取出当前目录的inode
	Inode *cur = new Inode();
	readInode(cur, parinoAddr);
	//依次取出inode对应的磁盘块，查找有没有名字为name的目录项
	int i = 0;

	//取出目录项数
	int cnt = cur->i_counter;

	//判断文件模式。6为owner，3为group，0为other
	int filemode;
	if (strcmp(g_current_user_name, cur->i_uname) == 0)
		filemode = 6;
	else if (strcmp(g_current_group_name, cur->i_gname) == 0)
		filemode = 3;
	else
		filemode = 0;

	while (i < 28 * 12)
	{
		DirItem dirlist[28] = {0};
		if (cur->i_dirBlock[i / 28] == -1)
		{
			i += 28;
			continue;
		}
		//取出磁盘块
		int parblockAddr = cur->i_dirBlock[i / 28];
		char *block_buf = new char[K_BLOCK_SIZE];

		//输出该磁盘块中的所有目录项
		int j;
		for (j = 0; j < 28; j++)
		{
			if (strcmp(dirlist[j].itemName, name) == 0)
			{
				Inode *tmp;
				//取出该目录项的inode，判断该目录项是目录还是文件

				readInode(tmp, dirlist[j].inode_addr);
				if (((tmp->i_mode >> 9) & 1) == 1)
				{
					//找到该目录，判断是否具有进入权限
					if (((tmp->i_mode >> filemode >> 0) & 1) == 0 && strcmp(g_current_user_name, "root") != 0)
					{ //root用户所有目录都可以查看
						//没有执行权限
						printf("权限不足：无执行权限\n");
						return;
					}

					//找到该目录项，如果是目录，更换当前目录

					g_current_dir_addr = dirlist[j].inode_addr;
					if (strcmp(dirlist[j].itemName, ".") == 0)
					{
						//本目录，不动
					}
					else if (strcmp(dirlist[j].itemName, "..") == 0)
					{
						//上一次目录
						int k;
						for (k = strlen(g_current_dir_name); k >= 0; k--)
							if (g_current_dir_name[k] == '/')
								break;
						g_current_dir_name[k] = '\0';
						if (strlen(g_current_dir_name) == 0)
							g_current_dir_name[0] = '/', g_current_dir_name[1] = '\0';
					}
					else
					{
						if (g_current_dir_name[strlen(g_current_dir_name) - 1] != '/')
							strcat(g_current_dir_name, "/");
						strcat(g_current_dir_name, dirlist[j].itemName);
					}

					return;
				}
				else
				{
					//找到该目录项，如果不是目录，继续找
				}
			}

			i++;
		}
	}

	//没找到
	printf("没有该目录\n");
	return;
}

void FileProcess::testWriteBlock(int block_addr)
{
	disk_head.seekg(block_addr * K_BLOCK_SIZE, std::ios::beg);
	disk_head.read(buf_4KB, K_BLOCK_SIZE);
	std::cout << "size of(DirItem): " << sizeof(DirItem) << std::endl;
	char *buf = new char[sizeof(DirItem)];
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < sizeof(DirItem); j++)
		{
			buf[j] = buf_4KB[i * sizeof(DirItem) + j];
		}
		DirItem *temp = (DirItem *)buf;
		std::cout << "DEBUG::FILEPROCESS::testWriteBLock 1084 temp ->inode_addr： " << temp->inode_addr << std::endl;
		std::cout << "DEBUG::FILEPROCESS::testWriteBLock 1084 temp -> itemName: " << temp->itemName << std::endl;
	}
}

bool FileProcess::testWriteResult()
{
	disk_head.seekg(0, std::ios::beg);
	char buf[1024 * 4];
	disk_head.read(buf, 1024 * 4);
	SuperBlock *super = (SuperBlock *)&buf;
	std::cout << "super->s_block_num: " << super->s_block_num << std::endl;
	std::cout << "super->s_block_size : " << super->s_block_size << std::endl;
	std::cout << "super->s_blockBitmap_startAddr: " << super->s_blockBitmap_startAddr << std::endl;
	using std::cout;
	using std::endl;
	cout << " super->s_blocks_per_group " << super->s_blocks_per_group << endl;
	cout << "super->s_data_block_startAddr: " << super->s_data_block_startAddr << endl;
	cout << "super->s_free_data_block_addr " << super->s_free_data_block_addr << endl;
	cout << "super->s_free_Inode_num: " << super->s_free_Inode_num << endl;
	cout << "super->s_Inode_size: " << super->s_Inode_size << endl;
	cout << "super->s_Inode_startAddr: " << super->s_Inode_startAddr << endl;
	cout << "super->s_InodeBitmap_startAddr: " << super->s_InodeBitmap_startAddr << endl;
	cout << "super->s_superblock_size: " << super->s_superblock_size << endl;
	cout << "super->s_superblock_startAddr: " << super->s_superblock_startAddr << endl;
	return true;
}

void FileProcess::testWriteInode(int inode_addr)
{
	using std::cout;
	using std::endl;
	using std::ios;
	disk_head.seekg(K_INODE_STARTADDR * K_BLOCK_SIZE + inode_addr * K_INODE_SIZE, ios::beg);
	char buf[K_INODE_SIZE];
	disk_head.read(buf, sizeof(Inode));
	cout << "Inode size: " << sizeof(Inode) << endl;
	Inode *temp_inode = (Inode *)buf;
	cout << "i_Inode_num: " << temp_inode->i_Inode_num << endl;
	cout << "i_mode: " << temp_inode->i_mode << endl;
	cout << "i_counter: " << temp_inode->i_counter << endl;
	cout << "i_uname: " << temp_inode->i_uname << endl;
	cout << "i_gname: " << temp_inode->i_gname << endl;
	cout << "i_size: " << temp_inode->i_size << endl;
	cout << "i_ctime: " << temp_inode->i_ctime << endl;
	cout << "i_mtime: " << temp_inode->i_mtime << endl;
	cout << "i_atime: " << temp_inode->i_atime << endl;
	for (int i = 0; i < 12; i++)
	{
		cout << "i_dirBlock[" << i << "]" << temp_inode->i_dirBlock[i] << endl;
	}

	cout << "i_indirBlock_1: " << temp_inode->i_indirBlock_1 << endl;
	//读出位图
	cout << "Inode is： " << inode_addr << endl;
	disk_head.seekg(K_INODE_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	disk_head.read(buf_4KB, 512);

	for (int i = 0; i < 512; i++)
	{
		cout << (int)buf_4KB[i] << " ";
		if ((i + 1) % 64 == 0)
		{
			cout << endl;
		}
	}

	cout << "检查block bitmap： ——————————————————————" << endl;
	disk_head.seekg(K_BLOCK_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	disk_head.read(buf_4KB, 512);

	for (int i = 0; i < 512; i++)
	{
		cout << (int)buf_4KB[i] << " ";
		if ((i + 1) % 64 == 0)
		{
			cout << endl;
		}
	}
}

void FileProcess::coutBitmap()
{
	using std::cout;
	using std::endl;
	using std::ios;
	cout << "检查 Inode bitmap： ××××××××××××××××" << endl;
	disk_head.seekg(K_INODE_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	disk_head.read(buf_4KB, 512);

	for (int i = 0; i < 512; i++)
	{
		cout << (int)buf_4KB[i] << " ";
		if ((i + 1) % 64 == 0)
		{
			cout << endl;
		}
	}

	cout << "检查block bitmap： ——————————————————————" << endl;
	disk_head.seekg(K_BLOCK_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	disk_head.read(buf_4KB, 512);

	for (int i = 0; i < 512; i++)
	{
		cout << (int)buf_4KB[i] << " ";
		if ((i + 1) % 64 == 0)
		{
			cout << endl;
		}
	}
}
