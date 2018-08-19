
#include<fstream>
#include<iostream>
#include "FileProcess.h"   
FileProcess::FileProcess() {
	//disk_head.open("")
	using std::ios;
	disk_head.open(K_SYSTEM_FILE, ios::in|ios::out|ios::binary);

}

FileProcess::~FileProcess() {
}
bool FileProcess::writeFile(char content[], int block_addr, int size_byte) {
	using std::fstream;
	using std::ios;
	using std::cout;
	using std::endl;
	//if(size_byte > 1024 * 4) {
	//	cout << "写入块过大，请重新调整， file process" << endl;
	//	return false;
	//}
	//else {
		//第一，定位文件地址
		disk_head.seekg(block_addr * 4 * 1024 ,ios::beg);
		disk_head.write(content, size_byte);
		if(disk_head.good()) {
			cout << "写入成功" << endl;
			return true;
		}
		else {
			cout << "写入失败" << endl;
			return false;
		}
		
	//}	
}
//为inode 置位
bool FileProcess::setInodeBitmap(int inode_addr) {

}
//为block 置位
bool FileProcess::setBlockBitmap(int block_addr) {
	using std::cout;
	using std::endl;
	using std::ios;
	disk_head.seekg(K_BLOCK_BITMAP_STARTADDR * 1024 * 4 + block_addr, ios::beg);
	bool flag = 1;
	//存储到系统中的值是 0x01
	disk_head.write((char*)&flag, 1);
	disk_head.seekp(K_BLOCK_BITMAP_STARTADDR * 1024 * 4 + block_addr, ios::beg);
	char *buf= new char[1];
	disk_head.read(buf, 1);
	
	cout << "test write result is: " ;
	for(int i =0; i < 8; i++) {
		int temp;
		temp = (buf[0] >> i) & 1;
		cout << temp; 
	}
	cout << endl;
	delete[] buf;
	if(disk_head.good()) {
		return true;
	}
	else {
		return false;
	}
}

