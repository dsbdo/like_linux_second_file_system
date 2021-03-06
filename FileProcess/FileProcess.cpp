
#include <fstream>
#include <iostream>
#include <string.h>
#include "FileProcess.h"
FileProcess::FileProcess()
{
	//disk_head.open("")
	using std::ios;
	disk_head.open(K_SYSTEM_FILE, ios::in | ios::out | ios::binary);
	m_free_block_num = K_BLOCK_NUM - 82;
	m_free_inode_num = K_INODE_NUM;
	//m_current_inode = new Inode();
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
		cout << "写入成功" << endl;
		return true;
	}
	else
	{
		cout << "写入失败" << endl;
		return false;
	}
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

	// std::cout << "DEBUG loadBitmap print m_inode_bitmap check:   !!!" << std::endl;
	// for (int i = 0; i < 512; i++)
	// {
	// 	std::cout << m_inode_bitmap[i] << "|";
	// 	if ((i + 1) % 64 == 0)
	// 	{
	// 		std::cout << std::endl;
	// 	}
	// }
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

	// std::cout << "m_block_bitmap check:   !!!" << std::endl;
	// for (int i = 0; i < 1024; i++)
	// {
	// 	std::cout << m_block_bitmap[i] << "|";
	// 	if ((i + 1) % 64 == 0)
	// 	{
	// 		std::cout << std::endl;
	// 	}
	// }
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

bool FileProcess::isFreeInode(int inode_addr)
{
	//1 是表示有人在用， 0 表示空闲
	return m_inode_bitmap[inode_addr] == 0 ? true : false;
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
	char *temp2 = new char[200];
	disk_head.read((char *)buf, K_INODE_SIZE);
	char *temp = new char[200];
	memcpy(inode_item, buf, sizeof(Inode));
	// cout << "test read Inode: " << endl;

	// cout << "test inode item inode num: " << inode_item->i_Inode_num << endl;
	// cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_mode: " << inode_item->i_mode << endl;
	// cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_counter:" << inode_item->i_counter << endl;
	// cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_uname:" << inode_item->i_uname << endl;
	// cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_gname:" << inode_item->i_gname << endl;
	// cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_size:" << inode_item->i_size << endl;
	// cout << "DEBUG::FILEPROCESS::readInode 382 i_ctime" << inode_item->i_ctime << " " << inode_item->i_mtime << " " << inode_item->i_atime << endl;
	// for (int i = 0; i < 12; i++)
	// {
	// 	cout << "DEBUG::FILEPROCESS::readInode 382 inode_item->i_dirBlock:" << inode_item->i_dirBlock[i] << endl;
	// }
	// cout << "test inode item inode indirBlock: " << inode_item->i_indirBlock_1 << endl;
}

void FileProcess::readBlock(char *block_buf, int block_addr)
{
	//读出一个block
	disk_head.seekg(block_addr * K_BLOCK_SIZE, std::ios::beg);
	disk_head.read(block_buf, K_BLOCK_SIZE);
}

void FileProcess::readFile(int file_inode_addr, char *buf)
{
	//读取文件内容，并输出到指定的buffer
	Inode *current_inode = new Inode();
	readInode(current_inode, file_inode_addr);
	int file_blocks = current_inode->i_size / K_BLOCK_SIZE;
	for (int i = 0; i < file_blocks; i++)
	{
		readBlock(buf_4KB, current_inode->i_dirBlock[i]);
		memcpy(buf + i * K_BLOCK_SIZE, buf_4KB, K_BLOCK_SIZE);
	}
	//还有残留的一部分
	int len = current_inode->i_size - file_blocks * K_BLOCK_SIZE;
	readBlock(buf_4KB, current_inode->i_dirBlock[file_blocks]);
	memcpy(buf, buf_4KB, len);
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

	//testWriteInode(parent_inode_addr);
	//std::cout << "DEBUG test in call mkdir function: " << current_inode->i_Inode_num << std::endl;

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
			readBlock(buf_4KB, current_inode->i_dirBlock[dir_block_num]);

			//没有办法强制转型，所以采用内存复制的方式
			memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);

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
					delete tmp_inode;
				}
				else if (strcmp(dirlist[j].itemName, "") == 0 || dir_item_cnt == (current_inode->i_counter + 1))
				{
					//找到一个空闲记录，将新目录创建到这个位置
					//记录这个位置
					if (posi == -1)
					{
						//std::cout << "DEBUG::FILEPROCESS::mkdir dir 471 block position: " << current_inode->i_dirBlock[dir_block_num] << std::endl;
						//std::cout << "DEBUG::FILEPROCESS::mkdir dir 472 item position: " << j << std::endl;
						posi = current_inode->i_dirBlock[dir_block_num];
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

			readBlock(buf_4KB, posi);
			memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);
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
		// std::cout << "DEBUG::FILEPROCESS::mkdir 545 testWriteBlock start$$$$$$$$$$$$$$$$$$$$" << std::endl;
		// testWriteBlock(child_dir_block_addr);
		// std::cout << "DEBUG::FILEPROCESS::mkdir 545 testWriteBlock over$$$$$$$$$$$$$$$$$$$$$" << std::endl;

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

		writeBlockFile((char *)dirlist, posi, K_BLOCK_SIZE);

		current_inode->i_counter++;
		writeInode((char *)current_inode, parent_inode_addr);
		return true;
	}
	else
	{
		std::cout << "error in mkdir, over max dir item" << std::endl;
		return false;
	}
}
//等一下需要将上一级的inode->cnt -1
void FileProcess::rmall(int parinoAddr) //删除该节点下所有文件或目录
{
	//从这个地址取出inode
	Inode *cur = new Inode();
	readInode(cur, parinoAddr);

	//取出目录项数
	int cnt = cur->i_counter;
	if (cnt <= 2)
	{
		//文件与空目录直接在这里进行删除
		//freeBlock(cur->i_dirBlock[0]);
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

		readBlock(buf_4KB, parblockAddr);
		memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);

		//从磁盘块中依次取出目录项，递归删除
		int j;
		bool f = false;
		for (j = 0; j < 28; j++)
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
	}
	//该inode已空，回收
	freeInode(parinoAddr);
	return;
}

bool FileProcess::rmdir(int parinoAddr, char name[]) //目录删除函数
{
	if (strlen(name) >= K_MAX_NAME_SIZE)
	{
		printf("超过最大目录名长度\n");
		return false;
	}

	if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
	{
		printf("错误操作\n");
		return 0;
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

	if ((((cur->i_mode >> filemode >> 1) & 1) == 0) && (strcmp(g_current_user_name, "root") != 0))
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
		readBlock(buf_4KB, parblockAddr);
		memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);

		//找到要删除的目录
		int j;
		for (j = 0; j < 28; j++)
		{
			Inode *tmp = new Inode();
			//取出该目录项的inode，判断该目录项是目录还是文件
			readInode(tmp, dirlist[j].inode_addr);

			if (strcmp(dirlist[j].itemName, name) == 0)
			{
				if (((tmp->i_mode >> 9) & 1) == 1)
				{ //找到目录
					//是目录

					rmall(dirlist[j].inode_addr);

					//删除该目录条目，写回磁盘
					strcpy(dirlist[j].itemName, "");
					dirlist[j].inode_addr = -1;
					writeBlockFile((char *)dirlist, parblockAddr);
					memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);
					cur->i_counter--;
					writeInode((char *)cur, parinoAddr);

					return true;
				}
				else
				{
					//不是目录，不管
				}
			}
			i++;
			delete tmp;
		}
	}

	printf("没有找到该目录\n");
	return false;
}

bool FileProcess::create(int parinoAddr, char name[], char buf[], int size_byte) //创建文件函数，在该目录下创建文件，文件内容存在buf
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
	while (i < 28 * 12 && posi == -1)
	{
		//160个目录项之内，可以直接在直接块里找
		dno = i / 28; //在第几个直接块里

		if (cur->i_dirBlock[dno] == -1)
		{
			i += 28;
			continue;
		}

		//char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(buf_4KB, cur->i_dirBlock[dno]);
		memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);
		//delete[] block_buf;
		//输出该磁盘块中的所有目录项
		int j;
		for (j = 0; j < 28; j++)
		{

			if (posi == -1 && strcmp(dirlist[j].itemName, "") == 0)
			{
				//找到一个空闲记录，将新文件创建到这个位置
				posi = cur->i_dirBlock[dno];
				posj = j;
				break;
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
		int len = size_byte; //文件长度，单位为字节
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

			//测试DEBUG
			testWriteBlock(curblockAddr, 2);
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
		testWriteInode(chiinoAddr);

		//将当前目录的磁盘块写回
		writeBlockFile((char *)dirlist, cur->i_dirBlock[dno]);
		testWriteBlock(cur->i_dirBlock[dno], 1);
		//写回inode
		cur->i_counter++;
		writeInode((char *)cur, parinoAddr);
		testWriteInode(parinoAddr);

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
	{
		DirItem dirlist[28] = {0};

		if (cur->i_dirBlock[i / 28] == -1)
		{
			i += 28;
			continue;
		}
		//取出磁盘块
		int parblockAddr = cur->i_dirBlock[i / 28];
		//char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(buf_4KB, parblockAddr);
		memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);
		//delete[] block_buf;

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
					// for (k = 0; k < 12; k++)
					// 	if (tmp->i_dirBlock[k] != -1)
					// 		freeBlock(tmp->i_dirBlock[k]);

					//释放inode
					freeInode(dirlist[pos].inode_addr);

					//删除该目录条目，写回磁盘
					strcpy(dirlist[pos].itemName, "");
					dirlist[pos].inode_addr = -1;
					writeBlockFile((char *)dirlist, parblockAddr);
					testWriteBlock(parblockAddr, 1);
					cur->i_counter--;

					writeInode((char *)cur, parinoAddr);
					testWriteInode(parinoAddr);
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
			i += 28;
			continue;
		}
		//取出磁盘块
		int parblockAddr = cur->i_dirBlock[i / 28];
		//char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(buf_4KB, parblockAddr);
		memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);
		//delete[] block_buf;
		//输出该磁盘块中的所有目录项
		Inode *tmp = new Inode();
		for (int j = 0; j < 28 && i < cnt; j++)
		{
			//取出该目录项的inode，判断该目录项是目录还是文件
			//在这里需要判断inode 是否已经释放
			if (isFreeInode(dirlist[j].inode_addr))
			{
				continue;
			}
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
		delete tmp;
	}
	/*  未写完 */
}

void FileProcess::cd(int parinoAddr, char name[], int type) //进入当前目录下的name目录
{
	char *temp_name = new char[128];
	strcpy(temp_name, name);

	//取出当前目录的inode
	if (type == 1)
	{
		//绝对地址切换
		int str_len = strlen(name);
		if (name[str_len - 1] == '/' && str_len == 1)
		{
			//直接切换到绝对地址
			g_current_dir_inode_addr = g_root_dir_inode_addr;
			return;
		}
		int last_position = 0;
		for (int i = str_len - 1; i > 0; i--)
		{
			if (name[str_len - 1] == '/' && i == str_len - 1)
			{
				i--;
			}
			else if (name[i] == '/')
			{
				last_position = i;
				break;
			}
		}
		//将进行字符串进行剪切
		char *path = new char[128];
		memset(path, 0, 128);
		memcpy(path, name, last_position + 1);

		memcpy(name, name + (last_position + 1), str_len - last_position - 1);
		if (name[str_len - last_position - 2] == '/')
		{
			name[str_len - last_position - 2] = '\0';
		}
		name[str_len - last_position - 1] = '\0';

		// std::cout << path << std::endl;
		// std::cout << name << std::endl;
		int file_inode_addr = locateFile(path, g_root_dir_inode_addr);
		delete path;
		parinoAddr = file_inode_addr;
	}

	//不一定找得到
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
		//char *block_b= new char[K_BLOCK_SIZE];
		readBlock(buf_4KB, parblockAddr);
		memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);

		//输出该磁盘块中的所有目录项
		int j;
		for (j = 0; j < 28; j++)
		{
			if (strcmp(dirlist[j].itemName, name) == 0)
			{
				Inode *tmp = new Inode();
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
						//切换到本目录，不动
					}
					else if (strcmp(dirlist[j].itemName, "..") == 0)
					{
						//切换到上一次目录，这个明显不对，需要修改
						int k;
						for (k = strlen(g_current_dir_name); k >= 0; k--)
						{
							if (g_current_dir_name[k] == '/')
							{
								break;
							}
							g_current_dir_name[k] = '\0';
							//根目录直接跳出
						}
						//设置g_current_dir_inode_addr
						g_current_dir_inode_addr = dirlist[j].inode_addr;
						//需要重新设置目录名称
						int counter = 0;
						for (int i = strlen(g_current_dir_name) - 1; i >= 0; i--)
						{
							if (g_current_dir_name[i] == '/')
							{
								counter++;
							}
							if (counter == 2)
							{
								counter = i;
								break;
							}
						}
						//将字符串进行覆盖
						memcpy(g_current_dir_name, g_current_dir_name, counter + 1);
						g_current_dir_name[counter + 1] = '\0';
					}
					else
					{
						//切换到当前目录下的一个子目录，
						if (g_current_dir_name[strlen(g_current_dir_name) - 1] != '/')
							strcat(g_current_dir_name, "/");
						strcat(g_current_dir_name, dirlist[j].itemName);
						if (g_current_dir_name[strlen(g_current_dir_name) - 1] != '/')
							strcat(g_current_dir_name, "/");
						//形成的格式大概为 g_current_dir_name/name
						//设置当前inode地址
						g_current_dir_inode_addr = dirlist[j].inode_addr;
					}
					if (type == 1)
					{
						strcpy(g_current_dir_name, temp_name);
						if (g_current_dir_name[strlen(g_current_dir_name) - 1] != '/')
							strcat(g_current_dir_name, "/");
					}
					return;
				}
				else
				{
					//找到该目录项，如果不是目录，继续找
				}
				delete tmp;
			}

			i++;
		}
	}

	//没找到
	printf("没有该目录\n");
	return;
}

//系统编辑文件的方法
void FileProcess::sysEditFile(int file_inode_addr, char content[], int content_size_byte, int edit_type)
{
	//不想判断是否有简接块了，真的懒得写了
	Inode *current_inode = new Inode();
	readInode(current_inode, file_inode_addr);
	//文件占用磁盘块数目
	if (edit_type == K_APPEND_FILE)
	{
		int file_blocks = current_inode->i_size / K_BLOCK_SIZE;
		if ((content_size_byte + current_inode->i_size) / K_BLOCK_SIZE > file_blocks)
		{
			//需要申请block
			int need_to_alloc_block = (content_size_byte + current_inode->i_size) / K_BLOCK_SIZE - file_blocks;
			//从哪一个位置插入
			int position = current_inode->i_size % K_BLOCK_SIZE;
			//插入多少可以填满这个block
			int len = K_BLOCK_SIZE - position;
			readBlock(buf_4KB, current_inode->i_dirBlock[file_blocks]);
			memcpy(buf_4KB + position, content, len);
			writeBlockFile(buf_4KB, current_inode->i_dirBlock[file_blocks]);

			for (int i = 0; i < need_to_alloc_block; i++)
			{
				int new_block = allocBlock();
				if (content_size_byte - (len + i * K_BLOCK_SIZE) >= K_BLOCK_SIZE)
				{
					memcpy(buf_4KB, content + len + i * K_BLOCK_SIZE, K_BLOCK_SIZE);
				}
				else
				{
					memcpy(buf_4KB, content + len + i * K_BLOCK_SIZE, content_size_byte - (len + i * K_BLOCK_SIZE));
				}

				writeBlockFile(buf_4KB, new_block);
				current_inode->i_dirBlock[file_blocks + i + 1] = new_block;
			}
			//更新inode
			current_inode->i_size += content_size_byte;
			writeInode((char *)current_inode, file_inode_addr);
		}
		else
		{
			//可以直接追加，追加的方式，读取block 到缓冲区，然后再将block 写回
			//寻找这个文件的最后一个block
			if (current_inode->i_indirBlock_1 == -1)
			{
				//无简接块,
				readBlock(buf_4KB, current_inode->i_dirBlock[file_blocks]);
				int position = current_inode->i_size % K_BLOCK_SIZE;
				memcpy(buf_4KB + position, content, content_size_byte);
				//std::cout << buf_4KB << std::endl;
				//current_inode->i_size += content_size_byte;
				writeBlockFile(buf_4KB, current_inode->i_dirBlock[file_blocks]);
			}
			current_inode->i_size += content_size_byte;
			writeInode((char *)current_inode, file_inode_addr);
		}
	}
	else if (edit_type == K_COVER_FILE)
	{
		//未写完！！！！！
		//以覆盖的形式写文件,不
		//写文件
		if (content_size_byte <= K_BLOCK_SIZE * 12)
		{
			int file_blocks = content_size_byte / K_BLOCK_SIZE;
			for (int i = 0; i < file_blocks; i++)
			{
				memcpy(buf_4KB, content + i * K_BLOCK_SIZE, K_BLOCK_SIZE);
				writeBlockFile(buf_4KB, current_inode->i_dirBlock[i]);
			}
			int left_byte_len = content_size_byte - file_blocks * K_BLOCK_SIZE;
			memcpy(buf_4KB, content + file_blocks * K_BLOCK_SIZE, left_byte_len);

			writeBlockFile(buf_4KB, current_inode->i_dirBlock[file_blocks]);
		}
		current_inode->i_size += content_size_byte;
		writeInode((char *)current_inode, file_inode_addr);
	}

	delete current_inode;
}

int FileProcess::locateFile(char path[], int current_inode_addr)
{
	// /etc/***,绝对寻址
	// ./ 相对寻址，支持 ./.././../      locate
	int len = strlen(path);
	int counter_src = 0;
	int counter_des = 0;
	//int current_inode = current_inode_addr;

	if (path[0] == '/')
	{
		current_inode_addr = g_root_dir_inode_addr;
		if (strlen(path) == 1)
		{
			return g_root_dir_inode_addr;
		}
		counter_src++;
		//绝对寻址
	}
	else if (path[0] == '.' && path[1] == '/')
	{
		//本目录相对寻址
		counter_src += 2;
	}
	else
	{
		//本目录相对寻址
		counter_src = 0;
	}
	char *name = new char[128];
	memset(name, 0, 128);
	while (true)
	{
		if (path[counter_src] != '/' && counter_src < len)
		{
			name[counter_des] = path[counter_src];
			counter_src++;
			counter_des++;
		}
		else
		{
			std::cout << "name is: " << name << "bytes; " << name << std::endl;

			if (counter_src == len)
			{
				//定位文件Inode
				current_inode_addr = locateFileHelp(name, current_inode_addr, counter_des, 1);
			}
			else
			{
				//定位目录Inode
				current_inode_addr = locateFileHelp(name, current_inode_addr, counter_des, 2);
			}
			if (current_inode_addr == -1)
			{
				return -1;
			}
			else if (counter_src == len)
			{
				return current_inode_addr;
			}
			else
			{
				counter_des = 0;
				counter_src++; //“跳掉 /”
				if (counter_src == len)
				{
					return current_inode_addr;
				}
			}
		}
	}
}

int FileProcess::locateFileHelp(char name[], int inode_addr, int name_len, int locate_dir_or_file)
{
	//1 默认找文件
	//2 找目录
	//读取到的有效的dir_item 项
	int effect_dir_item = 0;
	DirItem dirlist[K_DIR_ITEM_PER_BLOCK];
	char *dir_file_name = new char[name_len + 1];
	strcpy(dir_file_name, name);
	std::cout << "find file name is: " << dir_file_name << std::endl;
	//读取当前Inode
	Inode *cur = new Inode();
	readInode(cur, inode_addr);
	Inode *tmp_inode = new Inode();
	for (int i = 0; i < 12; i++)
	{
		if (cur->i_dirBlock[i] != -1)
		{
			//这个block 里面有目录需要处理
			readBlock(buf_4KB, cur->i_dirBlock[i]);
			memcpy(dirlist, buf_4KB, sizeof(DirItem) * 28);
			//遍历当前目录项块
			for (int j = 0; j < K_DIR_ITEM_PER_BLOCK; j++)
			{
				//如果这个块中有上一个文件的目录余留，如何处理呢？我觉得啊，不如创建目录的时候就把块重置好，这样就不用太过麻烦
				//也就是dirlist[j].itemName == "", 即为无效
				if (dirlist[j].itemName != "")
				{
					effect_dir_item++;
					if (strcmp(dir_file_name, dirlist[j].itemName) == 0)
					{
						//判断是否是目录还是文件,需要将这个目录或者文件的inode 读取出来，判断是文件还是目录
						readInode(tmp_inode, dirlist[j].inode_addr);
						if ((tmp_inode->i_mode >> 9 & 1) == 1 && locate_dir_or_file == 2)
						{
							//这一项是一个目录
							//已经找到项
							delete cur;
							delete tmp_inode;
							return dirlist[j].inode_addr;
						}
						else if ((tmp_inode->i_mode >> 9 & 1) == 0 && locate_dir_or_file == 1)
						{
							//这一项是一个目录
							//已经找到项
							delete cur;
							delete tmp_inode;
							return dirlist[j].inode_addr;
						}
					}
				}
			}
		}
	}
	delete cur;
	delete tmp_inode;
	return -1;
}

void FileProcess::testWriteBlock(int block_addr, int type)
{
	// disk_head.seekg(block_addr * K_BLOCK_SIZE, std::ios::beg);
	// disk_head.read(buf_4KB, K_BLOCK_SIZE);
	// if (type == 1)
	// {
	// 	std::cout << "size of(DirItem): " << sizeof(DirItem) << std::endl;
	// 	char *buf = new char[sizeof(DirItem)];
	// 	for (int i = 0; i < 16; i++)
	// 	{
	// 		for (int j = 0; j < sizeof(DirItem); j++)
	// 		{
	// 			buf[j] = buf_4KB[i * sizeof(DirItem) + j];
	// 		}
	// 		DirItem *temp = (DirItem *)buf;
	// 		std::cout << "DEBUG::FILEPROCESS::testWriteBLock 1084 temp ->inode_addr： " << temp->inode_addr << std::endl;
	// 		std::cout << "DEBUG::FILEPROCESS::testWriteBLock 1084 temp -> itemName: " << temp->itemName << std::endl;
	// 	}
	// }
	// else if (type == 2)
	// {
	// 	//检查写文件内容
	// 	std::cout << "\033[1;32m"
	// 			  << "DEBUG::FILEPROCESS::testwriteFile:: "
	// 			  << "\033[0m" << buf_4KB << std::endl;
	// }
	return ;
}

bool FileProcess::testWriteResult()
{
	// disk_head.seekg(0, std::ios::beg);
	// char buf[1024 * 4];
	// disk_head.read(buf, 1024 * 4);
	// SuperBlock *super = (SuperBlock *)&buf;
	// std::cout << "super->s_block_num: " << super->s_block_num << std::endl;
	// std::cout << "super->s_block_size : " << super->s_block_size << std::endl;
	// std::cout << "super->s_blockBitmap_startAddr: " << super->s_blockBitmap_startAddr << std::endl;
	// using std::cout;
	// using std::endl;
	// cout << " super->s_blocks_per_group " << super->s_blocks_per_group << endl;
	// cout << "super->s_data_block_startAddr: " << super->s_data_block_startAddr << endl;
	// cout << "super->s_free_data_block_addr " << super->s_free_data_block_addr << endl;
	// cout << "super->s_free_Inode_num: " << super->s_free_Inode_num << endl;
	// cout << "super->s_Inode_size: " << super->s_Inode_size << endl;
	// cout << "super->s_Inode_startAddr: " << super->s_Inode_startAddr << endl;
	// cout << "super->s_InodeBitmap_startAddr: " << super->s_InodeBitmap_startAddr << endl;
	// cout << "super->s_superblock_size: " << super->s_superblock_size << endl;
	// cout << "super->s_superblock_startAddr: " << super->s_superblock_startAddr << endl;
	return true;
}

void FileProcess::testWriteInode(int inode_addr)
{
	// using std::cout;
	// using std::endl;
	// using std::ios;
	// disk_head.seekg(K_INODE_STARTADDR * K_BLOCK_SIZE + inode_addr * K_INODE_SIZE, ios::beg);
	// char buf[K_INODE_SIZE];
	// disk_head.read(buf, sizeof(Inode));
	// cout << "Inode size: " << sizeof(Inode) << endl;
	// Inode *temp_inode = (Inode *)buf;
	// cout << "i_Inode_num: " << temp_inode->i_Inode_num << endl;
	// cout << "i_mode: " << temp_inode->i_mode << endl;
	// cout << "i_counter: " << temp_inode->i_counter << endl;
	// cout << "i_uname: " << temp_inode->i_uname << endl;
	// cout << "i_gname: " << temp_inode->i_gname << endl;
	// cout << "i_size: " << temp_inode->i_size << endl;
	// cout << "i_ctime: " << temp_inode->i_ctime << endl;
	// cout << "i_mtime: " << temp_inode->i_mtime << endl;
	// cout << "i_atime: " << temp_inode->i_atime << endl;
	// for (int i = 0; i < 12; i++)
	// {
	// 	cout << "i_dirBlock[" << i << "]" << temp_inode->i_dirBlock[i] << endl;
	// }

	// cout << "i_indirBlock_1: " << temp_inode->i_indirBlock_1 << endl;
	// //读出位图
	// cout << "Inode is： " << inode_addr << endl;
	// disk_head.seekg(K_INODE_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	// disk_head.read(buf_4KB, 512);

	// for (int i = 0; i < 512; i++)
	// {
	// 	cout << (int)buf_4KB[i] << " ";
	// 	if ((i + 1) % 64 == 0)
	// 	{
	// 		cout << endl;
	// 	}
	// }

	// cout << "检查block bitmap： ——————————————————————" << endl;
	// disk_head.seekg(K_BLOCK_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	// disk_head.read(buf_4KB, 512);

	// for (int i = 0; i < 512; i++)
	// {
	// 	cout << (int)buf_4KB[i] << " ";
	// 	if ((i + 1) % 64 == 0)
	// 	{
	// 		cout << endl;
	// 	}
	// }
}

void FileProcess::coutBitmap()
{
	// using std::cout;
	// using std::endl;
	// using std::ios;
	// cout << "检查 Inode bitmap： ××××××××××××××××" << endl;
	// disk_head.seekg(K_INODE_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	// disk_head.read(buf_4KB, 512);

	// for (int i = 0; i < 512; i++)
	// {
	// 	cout << (int)buf_4KB[i] << " ";
	// 	if ((i + 1) % 64 == 0)
	// 	{
	// 		cout << endl;
	// 	}
	// }

	// cout << "检查block bitmap： ——————————————————————" << endl;
	// disk_head.seekg(K_BLOCK_BITMAP_STARTADDR * K_BLOCK_SIZE, ios::beg);
	// disk_head.read(buf_4KB, 512);

	// for (int i = 0; i < 512; i++)
	// {
	// 	cout << (int)buf_4KB[i] << " ";
	// 	if ((i + 1) % 64 == 0)
	// 	{
	// 		cout << endl;
	// 	}
	// }
}
