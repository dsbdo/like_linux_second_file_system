
#include <fstream>
#include <iostream>
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
		for (int i = 0; i < 512; i++)
		{
			cout << content[i] << " " << endl;
			if ((i + 1) % 32 == 0)
			{
				cout << endl;
			}
		}
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

	std::cout << "m_inode_bitmap check:   !!!" << std::endl;
	for (int i = 0; i < 512; i++)
	{
		std::cout << m_inode_bitmap[i];
		if ((i + 1) % 16 == 0)
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
		std::cout << m_block_bitmap[i];
		if ((i + 1) % 16 == 0)
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

void FileProcess::readInode(Inode *&inode_item, int inode_addr)
{
	using std::cout;
	using std::endl;
	using std::ios;
	disk_head.seekg(K_INODE_STARTADDR * K_BLOCK_SIZE + inode_addr * K_INODE_SIZE, ios::beg);
	//读取出512字节
	char buf[K_INODE_SIZE];
	disk_head.read(buf, K_INODE_SIZE);
	inode_item = (Inode *)buf;
	cout << "test read Inode: " << endl;
	cout << "test inode item inode num: " << inode_item->i_Inode_num << endl;
	cout << "test inode item inode indirBlock: " << inode_item->i_indirBlock_1 << endl;
}

void FileProcess::readBlock(char *&block_buf, int block_addr)
{
	//读出一个block
	disk_head.seekg(block_addr * K_BLOCK_SIZE, std::ios::beg);
	disk_head.read(block_buf, K_BLOCK_SIZE);
}
//创建目录
bool FileProcess::mkdir(int parent_inode_addr, char dir_name[])
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
	std::cout << "DEBUG test in call function: " << current_inode->i_Inode_num << std::endl;

	int dir_item_cnt = 0; //遍历到第几个目录item
	//目录项数 +1
	int cnt = current_inode->i_counter + 1;
	int posi = -1, posj = -1;
	//直接块12 个， 每块 28条目录项记录,找到最新可插入目录项，不想搞那么多了，就最多的目录项设置为28*12吧
	while (dir_item_cnt < 28 * 12)
	{
		int dir_block_num = dir_item_cnt / 28;
		if (current_inode->i_dirBlock[dir_block_num] == -1)
		{
			//i += 28;
			//遍历完整所有直接块
			break;
			//continue;
		}
		//取出直接块
		char *block_buf = new char[K_BLOCK_SIZE];
		readBlock(block_buf, current_inode->i_dirBlock[dir_block_num]);

		//没有办法强制转型，所以采用内存复制的方式
		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);

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
			else
			{
				//找到一个空闲记录，将新目录创建到这个位置
				//记录这个位置
				if (dir_item_cnt == (current_inode->i_counter + 1))
				{
					//已经遍历到这个目录项对应的最有的目录了
					posi = dir_block_num;
					posj = j; //这个块中的第几项
					break;
				}
			}
			dir_item_cnt++;
			//超过最大目录项数
			if(dir_item_cnt == 28 * 12) {
				posi = -2;
				break;
			}
		}
		if(posi != -1) {
			break;
		}
	}
	//posi == -2 证明 目录项已经超过最大目录项了，因为不想理那个简接块了， 所以最大就设置成28*16
	if(posi != -2) {
		char* block_buf = new char[K_BLOCK_SIZE];
		readBlock(block_buf, posi);
		memcpy(dirlist, block_buf, sizeof(DirItem) * 28);

		//创建目录名
		strcpy(dirlist[posj].itemName,dir_name);
		int dir_inode_addr = allocInode();
		if(dir_inode_addr == -1) {
			std::cout << "inode 分配失败" << std::endl;
			return false;
		}
		dirlist[posj].inode_addr = dir_inode_addr;
		//设置inode
		Inode* new_dir_inode = new Inode();
		new_dir_inode->i_Inode_num = dir_inode_addr;
		new_dir_inode->i_atime = time(NULL);
		new_dir_inode->i_ctime = time(NULL);
		new_dir_inode->i_mtime = time(NULL);
		strcpy(new_dir_inode->i_uname, g_current_user_name);
		strcpy(new_dir_inode->i_gname, g_current_group_name);
		new_dir_inode->i_counter = 2;
		
	}
	else {
		std::cout << "error in mkdir, over max dir item" << std::endl;
	}

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
		std::cout << "temp ->inode_addr： " << temp->inode_addr << std::endl;
		std::cout << "temp -> itemName: " << temp->itemName << std::endl;
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
		if ((i + 1) % 32 == 0)
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
		if ((i + 1) % 32 == 0)
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
		if ((i + 1) % 32 == 0)
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
		if ((i + 1) % 32 == 0)
		{
			cout << endl;
		}
	}
}
