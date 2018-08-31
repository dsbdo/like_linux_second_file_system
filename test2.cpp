#include<iostream>
#include<string>
#include<cstdlib>
#include<ctype.h>
#include<cstdio>
#include<fstream>
using namespace std;
int NumberCount=0;//数字个数
int CharCount=0;//字母个数
int PunctuationCount=0;//标点符号个数
int BlankCount=0;//空白符个数
 
class Node
{
public:
    string character;
    int cursor;
    int offset;
    Node* next;
    Node(){
        cursor=0;//每行的光标初始位置
        offset=0;//每行的初始偏移位置
        next=NULL;
    }
};
 
class TextEditor
{
private:
    Node* head;
    string name;
    int line;//可更改的行数
    int length;//行数
public:
    TextEditor();
    ~TextEditor();
    string GetName();
    void SetName(string name);
    int GetCursor();
    int MoveCursor(int offset);
    int SetCursor(int line,int offset);
    void AddText(const string s);
    void InsertText(int seat,string s);
    int FindText(string s);
    void DeleteText(string s);
    int GetLine();
    void Count();
    friend ostream& operator<<(ostream& out,TextEditor &text);
    Node* Gethead(){
        return head;
    }
};
 
TextEditor::TextEditor()
{
    head=NULL;
    name="test";//文件初始名
    //tail=NULL;
    line=1;
    length=0;
}
 
TextEditor::~TextEditor()
{
    Node* p=head;
    Node* q;
    while(p!=NULL){
        q=p->next;
        delete p;
        p=q;
    }
 
}
 
int TextEditor::GetLine()
{
    return line;
}
 
string TextEditor::GetName()
{
    return name;
}
 
void TextEditor::SetName(string name)
{
    this->name=name;
}
 
int TextEditor::GetCursor()
{
    Node *p=head;
    while(p->next!=NULL)
        p=p->next;
    return p->cursor;
}
 
int TextEditor::MoveCursor(int offset)
{
    Node *p=head;
    int i=1;
    if(length+1<line){
        cout<<"输入错误!"<<endl;
        exit(0);
    }
    else{
        while(p->next!=NULL&&i<line){
            p=p->next;
            i++;
        }
    }
    if(offset>p->character.length()){
        cout<<"移动位置太大!"<<endl;
        exit(0);
    }
    else
       p->cursor+=offset;
    //cout<<"p  ->cursor="<<p->cursor<<endl;
    return p->cursor;
}
 
int TextEditor::SetCursor(int line,int offset)
{
    this->line=line;
   //cout<<"line="<<this->line<<endl;
    return MoveCursor(offset);
}
 
void TextEditor::AddText(const string s)
{
    line=length+1;
    Node* p=new Node;
    Node* q=head;
    p->character=s;
    p->next=NULL;
    if(head==NULL)
        head=p;
    else{
        while(q->next!=NULL)
            q=q->next;
        q->next=p;
    }
 
        length++;
       // line++;
}
 
void TextEditor::InsertText(int seat,string s)
{
    Node *p=head;
    int i=1;
    if(length+1<line){
        cout<<"输入错误!"<<endl;
        exit(0);
    }
    else{
        while(p->next!=NULL&&i<line){
            p=p->next;
            i++;
        }
    }
    //MoveCursor(seat);
    //cout<<"p->cursor="<<p->cursor<<endl;
    string substr;
    for(int i=seat;i<s.length()+seat&&i<=p->character.length();i++)
    substr+=p->character[i];
 
    p->character.insert(p->cursor,s);
 
 
    cout<<"substr="<<substr<<endl;
    DeleteText(substr);//覆盖子串
    p->cursor=0;//光标清零
}
 
ostream& operator<<(ostream& out,TextEditor &text)
{
    int i=1;
    Node* p=text.Gethead();
    while(p!=NULL){
        out<<p->character<<endl;
        p=p->next;
    }
   // cout<<"length="<<text.GetLength()<<endl;
    return out;
}
 
int TextEditor::FindText(string P)
{
    Node* q=head;
    //int templine=1;
    line=1;
    int p=0;
    int t=0;
    int plen=P.length()-1;
    //cout<<"P="<<P<<endl;
    //cout<<"plen="<<plen<<endl;
    int tlen=q->character.length();
    while(q!=NULL){
            p=0;
            t=0;
        tlen=q->character.length();
        if(tlen<plen){
            line++;
             q=q->next;
        }
 
        while(p<plen&&t<tlen){
            if(q->character[t]==P[p]){
                t++;
                p++;
            }
            else{
                t=t-p+1;
                p=0;
            }
        }
      //  cout<<"P="<<P<<endl;
      //  cout<<"p="<<p<<endl;
      //  cout<<"plen="<<plen<<endl;
        if(p>=plen){
 
            return t-plen+1;
        }
 
 
        else{
            line++;
            q=q->next;
        }
 
    }
    return -1;
}
 
void TextEditor::DeleteText(string s)
{
    Node *p=head;
    int i=1;
    int k=FindText(s);
    if(k==-1)
        cout<<"未出现该字符串!"<<endl;
    else{
        while(p!=NULL&&i<line){
            p=p->next;
           // cout<<p->character<<endl;
            i++;
        }
 
        p->character.erase(k-1,s.length());
        cout<<"删除成功！"<<endl;
    }
}
 
void TextEditor::Count()
{
    Node *p=head;
    NumberCount=0;
    CharCount=0;
    PunctuationCount=0;
    BlankCount=0;
    while(p!=NULL){
            for(int i=0;i<p->character.length();i++){
                if(p->character[i]>='0'&&p->character[i]<='9')
                    NumberCount++;
                else if(p->character[i]>'a'&&p->character[i]<'z'||p->character[i]>'A'&&p->character[i]<'Z')
                    CharCount++;
                else if(ispunct(p->character[i]))
                    PunctuationCount++;
                else if(p->character[i]==' ')
                    BlankCount++;
            }
            p=p->next;
    }
}
 
int main()
{
    int i,j,k,n=2;
    string s,t,name;
    TextEditor text;
    cout<<"---------------------------------------"<<endl;
    cout<<"1.添加字符"<<endl;
    cout<<"2.设置文档名字"<<endl;
    cout<<"3.获取文档名字"<<endl;
    cout<<"4.显示光标位置"<<endl;
    cout<<"5.设置光标位置，在光标位置处插入文本"<<endl;
    cout<<"6.在文档中查找文本"<<endl;
    cout<<"7.在文档中删除文本"<<endl;
    cout<<"8.统计字母、数字、标点符号、空白符号及总字符个数"<<endl;
    cout<<"9.输入文本"<<endl;
    cout<<"0.退出"<<endl;
    while(n){
       // cout<<endl;
        cout<<endl;
        cout<<"---------------------------------------"<<endl;
        cout<<"请输入:";
        cin>>n;
        getchar();
        switch(n){
            case 1: cout<<"请输入字符:"; getline(cin,s,'\n'); text.AddText(s); break;
            case 2: cout<<"请输入文档名字:"; cin>>name; text.SetName(name); break;
            case 3: cout<<text.GetName()<<endl; break;
            case 4: cout<<"光标在第"<<text.GetLine()<<"行，第"<<text.GetCursor()<<"个字符前!"<<endl; break;
            case 5:{
                cout<<"输入行数:";
                cin>>i;
                cout<<"光标在第"<<text.GetCursor()<<"个字符前!"<<endl;
                cout<<"输入移动位数:";
                cin>>j;
                cout<<"输入插入字符:";
                getchar();
                getline(cin,s);
                text.InsertText(text.SetCursor(i,j),s); break;
            }
            case 6: {
                cout<<"输入查找的字符串:";
                getline(cin,s);
                int k=text.FindText(s);
                if(k==-1)
                    cout<<"查找失败!"<<endl;
                else
                    cout<<"所查找文本首次出现在:"<<text.GetLine()<<"行，第"<<k<<"个字符处!"<<endl;
                    break;
            }
            case 7: cout<<"输入要删除的字符串:"; getline(cin,s); text.DeleteText(s); break;
            case 8: {
                text.Count();
                cout<<"文档中共有:"<<endl;
                cout<<NumberCount<<"个数字"<<endl;
                cout<<CharCount<<"个字母"<<endl;
                cout<<PunctuationCount<<"个标点符号"<<endl;
                cout<<BlankCount<<"个空白字符"<<endl;
                cout<<"共有"<<NumberCount+CharCount+PunctuationCount+BlankCount<<"个字符!"<<endl;
                break;
            }
            case 9: cout<<text; break;
            case 0:{
                string ss=text.GetName();
                ss+=".txt";
                cout<<ss<<endl;
                ofstream outFile(ss.c_str());
                Node* p=text.Gethead();
                while(p!=NULL){
                    outFile<<p->character<<endl;
                    p=p->next;
                }
                exit(0);
                break;
            }
            default: cout<<"输入错误，请重新输入！"<<endl; break;
        }
 
    }
}
