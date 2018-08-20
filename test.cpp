#include<fstream>
#include<iostream>
using namespace std;

int main() {
    char buf[512]= {0};
    buf[509] = '5';
    buf[508] = '5';
    buf[510] = 'A';
    buf[511] = 'A';
    fstream test("test.txt", ios::in | ios::out| ios::binary);
    if(!test) {
        cout << "打开文件失败" << endl;
    }
    test.write(buf, 1024);
    test.close();

    int a[5];
    a[0] = 0x30;
    a[1] = 0x31;
    a[2] = 0x32;
    a[3] = 0x33;
    a[4] = 0x34;
    char* arr = (char*)a;

    cout << arr<<" : " << arr[1] << endl; cout << arr[2] << " : "<<arr[3] << " : "<< arr[4] << endl;

    //char 数组 转 int
     char b[8];
     b[0] = 0x01;
     b[1] = 0x00;
     b[2] = 0x00;
     b[3] = 0x00;
     b[4] = 0x00;
     b[5] = 0x01;
     b[6] = 0x00;
     b[7] = 0x00; 
     int* brr = (int *)b;
     cout << brr[0]<< " "<< brr[1] << endl;


     struct DirItem {
	char itemName[128];
	int inode_addr;
     };
     cout << sizeof(DirItem);
    return 0;
}