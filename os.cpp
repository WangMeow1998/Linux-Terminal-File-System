#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<string>
#include<cstring>
#include<stack>
#pragma comment(linker, "/STACK:102400000,102400000") 		
using namespace std;

//文件结构体
struct file{
	string f_text;//存放文本
	unsigned int f_size;//文件大小
	string f_date;//文件日期
};
//I结点结构体
struct inode{ 
	struct inode *i_prev;
	struct inode *i_next;
	struct inode *i_extend;//延伸结点
	char i_type;//文件类型
	unsigned short i_flag;//文件访问,1可访问
	unsigned int i_mode;//文件权限
	unsigned short i_uid;//用户ID
	string i_name;//文件名字
	string i_date;//目录文件日期
	unsigned int i_size;//目录文件大小
	struct file *f;//文件指针
};
//用户结构体
struct user{
	unsigned int uid;//用户ID
	string username;//用户名
	string password;//用户密码
};
//创建用户结构体
struct crt_user{
	struct user *currentP;//当前用户指针
	struct crt_user *nextP;//下一用户指针
	struct crt_user *prevP;//上一用户指针
};

//文件系统类
class Fileos{
	public:
		void cd();
		void cat();
		void chmod();
		bool judgeMode(unsigned int mode);
		void cls();
		void InitNode();
		void createSuper();
		void help();
		void la();
		void ll();
		void outPermission(unsigned int mode);
		void ls();
		void login();
		void logout();
		void mv();
		void mkdir(string dirname,inode *preNode,unsigned int  mode,unsigned short i_uid);
		void userMkdir();
		void pwd();
		void su();
		void touch();
		void useradd();
		void vim();
		void rm();
		void rmdir();
		string getTime();
};
crt_user currentUser;//当前用户
crt_user *rootUser;//指向第一个用户的指针
user *up;//创建用户指针

inode *node;//当前节点
inode *rootNode;//根目录结点
inode *nextNode;//下一结点
inode *lastNode;//上一结点
inode *currentRootNode;//当前目录结点


string CURRENTUSER;//当前用户
unsigned int CURRENTUSERUID;//当前用户ID
string CURRENTDIR;//当前路径

char userTable[10][25]; //记录用户的表
unsigned int userCount = 0;//记录用户的个数

file *fp;
Fileos op;

stack<inode *> i_stack;
void pause()
{
	cout<<"请按任意键继续..."<<endl;
	fgetc(stdin);
}
void Fileos::cls()
{
	system("clear");
}
string Fileos::getTime()
{
	time_t t = time(NULL);
	char ch[64] = {0};
	strftime(ch, sizeof(ch) - 1, "%Y-%m-%d %H:%M:%S", localtime(&t));
	string localTime = ch;
	return localTime;
}
void Fileos::help()
{
	cout<<"----------------------------------------------------------------------"<<endl;
	cout<<"-     欢迎使用WangMeow's文件系统.你可以在本系统中使用下面的命令.     -"<<endl;
	cout<<"-                                cd                                  -"<<endl;
	cout<<"-                                cat                                 -"<<endl;
	cout<<"-                                chmod                               -"<<endl;
	cout<<"-                                clear                               -"<<endl;
	cout<<"-                                help                                -"<<endl;
	cout<<"-                                ll                                  -"<<endl;
	cout<<"-                                ls                                  -"<<endl;
	cout<<"-                                logout                              -"<<endl;
	cout<<"-                                mv                                  -"<<endl;
	cout<<"-                                mkdir                               -"<<endl;
	cout<<"-                                pwd                                 -"<<endl;
	cout<<"-                                rm                                  -"<<endl;
	cout<<"-                                rmdir                               -"<<endl;
	cout<<"-                                su                                  -"<<endl;
	cout<<"-                                touch                               -"<<endl;
	cout<<"-                                useradd                             -"<<endl;
	cout<<"-                                vim                                 -"<<endl;
	cout<<"----------------------------------------------------------------------"<<endl;
}
void Fileos::createSuper()
{
	string password1,password2;
	string name;
	up = new user;
	currentUser.nextP = NULL;
	currentUser.prevP = NULL;
	up->uid = 0;
	cout<<"                      --------------------------------------------------------"<<endl;
	cout<<"                      -             欢迎进入Wang Meow的文件系统~             -"<<endl;
	cout<<"                      -   您是第一次使用本系统，因此请您先创建超级权限用户.  -"<<endl;
	cout<<"                      -   超级用户:拥有系统的所有权限!请谨慎使用和保管!      -"<<endl;
	cout<<"                      --------------------------------------------------------"<<endl;
	cout<<"                          1.请出入您要创建的用户名:";
	cin>>name;
	up->username = name;
	cout<<"                          2.为确保系统的安全,请输入两次用户密码.             "<<endl;
	while(true)
	{
		cout<<"                          password 1:";
		system("stty -echo");
		cin>>password1;
		system("stty echo");
		cout<<endl;
		cout<<"                          password 2:";
		system("stty -echo");
		cin>>password2;
		system("stty echo");
		cout<<endl;
		if(password1 == password2) break;
		else
			cout<<"                          两次输入的密码不一致!请重新输入."<<endl;
	}
	getchar();//防止和fgetc碰撞，用getchar来读入回车
	up->password = password1;
	currentUser.currentP = up;
	strcpy(userTable[0],name.c_str());
	userCount++;
	cout<<endl;
	cout<<"                          用户"<<name<<"(root)"<<"创建成功!"<<endl;
	cout<<"                          即将进入文件系统..."<<endl;
	cout<<"                          ";
	pause();
	op.cls();
}
void Fileos::login()
{
	int times = 0;
	string username;
	string password;
	crt_user *nowLogin;
	crt_user *nextLogin;
	nowLogin = &currentUser;
	nextLogin = currentUser.nextP;
	cout<<"[Username]:";
	cin>>username;
	while(nowLogin!=NULL)
	{
		if(nowLogin->currentP->username == username)
			break;
		else
		{
			nowLogin = nextLogin;
			if(nextLogin!=NULL)
				nextLogin = nextLogin->nextP;
		}
	}
	if(nowLogin == NULL)
	{
		cout<<"不存在该用户,请输入正确的用户名."<<endl;
		exit(-1);
	}
	while(times<3)
	{
		cout<<"[Password]:";
		system("stty -echo");
		cin>>password;
		system("stty echo");
		if(password == nowLogin->currentP->password)
			break;
		times++;
		cout<<endl;
		cout<<"输错密码"<<times<<"次."<<"输错三次密码将自动退出程序!"<<endl;
	}
	if(times == 3)
		exit(-2);
	cout<<endl;
	cout<<"用户登录成功!欢迎"<<username<<"~"<<endl;
	CURRENTUSER = username;	
	getchar();//防止和fgetc碰撞，用getchar来读入回车
	pause();
}
void Fileos::mkdir(string dirname,inode *preNode,unsigned int mode,unsigned short uid)
{
	nextNode = new inode;
	preNode->i_next = nextNode;
	nextNode->i_prev = preNode;
	nextNode->i_next = NULL;
	nextNode->i_extend = NULL;
	nextNode->i_mode = mode;
	nextNode->i_flag = 1;
	nextNode->i_size = 4*1024;//一个目录占一个block,4KB
	nextNode->i_type = 'd';
	nextNode->i_uid = uid;
	nextNode->i_name = dirname;
	nextNode->i_date = op.getTime();
}
void Fileos::userMkdir()
{
	inode *p;
	string dirname;
	p = currentRootNode;
	cout<<"请输入要创建的目录名:";
	cin>>dirname;
	while(p->i_next!=NULL && p->i_name!=dirname)
		p = p->i_next;
	if(p->i_next == NULL && p->i_name!=dirname)
	{
		mkdir(dirname,p,755,CURRENTUSERUID);
		p->i_next = nextNode;
		cout<<"目录创建成功!"<<endl;
	}
	else
		cout<<"mkdir: "<<"无法创建目录"<<"\""<<dirname<<"\""<<": 文件已存在"<<endl;
}
void Fileos::logout()
{
	cout<<"注销成功! "<<"Bye, "<<CURRENTUSER<<"."<<endl;
	exit(0);
}
void Fileos::pwd()
{
	cout<<CURRENTDIR<<endl;
}
void Fileos::ls()
{
	int count = 1;
	inode *p;
	p = currentRootNode->i_next;
	while(p!=NULL)
	{
		count++;
		if(p->i_flag == 1)
			cout<<p->i_name<<"   ";
		else if(p->i_flag == 0)
		{
			count--;
		}
		if(count%11 == 0)
			cout<<endl;
		p = p->i_next;
	}
	cout<<endl;
}
void Fileos::outPermission(unsigned int mode)
{
	unsigned int n[3];
	n[0] = mode&4;
	n[1] = mode&2;
	n[2] = mode&1;
	if(n[0]>0) cout<<"r";
	else cout<<"-";
	if(n[1]>0) cout<<"w";
	else cout<<"-";
	if(n[2]>0) cout<<"x";
	else cout<<"-";
}
void Fileos::ll()
{
	inode *p;
	p = currentRootNode->i_next;
	while(p!=NULL)
	{
		unsigned int user = p->i_mode/100;
		unsigned int group = p->i_mode/10%10;
		unsigned int others = p->i_mode%10;
		cout<<p->i_type;
		op.outPermission(user);
		op.outPermission(group);
		op.outPermission(others);
		if(p->i_type == 'd' && (p->i_flag == 1 || p->i_flag == 0))
			printf("    %10s%10s%10d %s %s/\n",userTable[p->i_uid],userTable[p->i_uid],p->i_size,p->i_date.c_str(),p->i_name.c_str());
		else if(p->i_type != 'd' && (p->i_flag == 1 || p->i_flag == 0))
			printf("    %10s%10s%10d %s %s/\n",userTable[p->i_uid],userTable[p->i_uid],p->f->f_size,p->f->f_date.c_str(),p->i_name.c_str());
		p = p->i_next;
	}
}
bool Fileos::judgeMode(unsigned int mode)
{	
	unsigned int u,g,o;
	u = mode/100;
	g = mode/10%10;
	o = mode%10;
	if(u>=0 && u<=7 && g>=0 && g<=7 && o>=0 && o<=7)
		return true;
	else return false;
}
void Fileos::chmod()
{
	inode *p;
	string filename;
	unsigned int mode;
	cout<<"请输入要修改的文件的名字:";
	cin>>filename;
	p = currentRootNode;
	cout<<"请输入要修改的mode:";
	cin>>mode;
	if(judgeMode(mode))
	{
		while(p!=NULL)
		{
			if((p->i_name == filename && p->i_flag == 1) && (p->i_type == 'd' || p->i_type != 'd'))
			{
				if(CURRENTUSERUID!=0 && CURRENTUSERUID != p->i_uid)
				{
					cout<<"你的用户没有chmod权限!"<<endl;
					break;
				}
				p->i_mode = mode;
				p->i_date = op.getTime();
				cout<<"修改权限成功!"<<endl;
				break;
			}
			else
				p = p->i_next;
		}
		if(p == NULL)
			cout<<"chmod: 无法访问"<<"\'"<<filename<<"\'"<<": 没有这个文件或目录"<<endl;
	}
	else
		cout<<"错误的mode!"<<endl;
}
void Fileos::mv()//rename + hide
{
	inode *p,*q;
	string filename;
	string rename;
	char filenameFirstCh;
	char renameFirstCh;
	cout<<"请输入要重命名/隐藏的文件的名字:";
	cin>>filename;
	cout<<"请输出新的文件的名字:";
	cin>>rename;
	renameFirstCh = rename[0];
	filenameFirstCh = filename[0];
	p = currentRootNode->i_next;
	q = p;
	while(q!=NULL)
	{
		if(q->i_name == rename)
		{
			cout<<"无法重命名: 在该位置已经使用了名字"<<"\""<<q->i_name<<"\"."<<endl;
			cout<<"请使用其他名字!"<<endl;
			return;
		}
		q = q->i_next;
	}
	while(p!=NULL)
	{
		if(p->i_name == filename && (p->i_type == 'd' || p->i_type!='d'))
		{
			if(renameFirstCh !='.' && filenameFirstCh != '.' && p->i_flag == 1)
			{
				p->i_name = rename;
				p->i_date = op.getTime();
				cout<<"修改名字成功!"<<endl;
				break;
			}
			else if(renameFirstCh == '.' && filenameFirstCh !='.' && p->i_flag == 1)
			{
				p->i_name = rename;
				p->i_date = op.getTime();
				p->i_flag = 0;
				cout<<"修改名字成功!"<<endl;
				break;
			}
			else if(renameFirstCh != '.' && filenameFirstCh == '.' && p->i_flag == 0)
			{
				p->i_name = rename;
				p->i_date = op.getTime();
				p->i_flag = 1;
				cout<<"修改名字成功!"<<endl;
				break;
			}
			else if(renameFirstCh == '.' && filenameFirstCh == '.' && p->i_flag == 0)
			{
				p->i_name = rename;
				p->i_date = op.getTime();
				cout<<"修改名字成功!"<<endl;
				break;
			}
			else
			{
				cout<<"操作错误!"<<endl;
				break;
			}
		}
		else
			p = p->i_next;
	}
	if(p == NULL)
		cout<<"mv: 无法访问"<<"\'"<<filename<<"\'"<<": 没有这个文件或目录"<<endl;
}
void Fileos::useradd()
{
	string username;
	string password1,password2;
	crt_user *p;
	crt_user *newUser;
	user *newInformation;
	if(CURRENTUSERUID == 0)
	{
		p = rootUser;
		cout<<"请输入您要创建的用户名:";
		cin>>username;
		cout<<"为确保系统的安全,请输入两次用户密码."<<endl;
		while(true)
		{
			cout<<"password 1:";
			system("stty -echo");
			cin>>password1;
			system("stty echo");
			cout<<endl;
			cout<<"password 2:";
			system("stty -echo");
			cin>>password2;
			system("stty echo");
			cout<<endl;
			if(password1 == password2) break;
			else
				cout<<"两次输入的密码不一致!请重新输入."<<endl;
		}
		while(p->nextP!=NULL && p->currentP->username!=username)
			p = p->nextP;
		if(p->nextP == NULL && p->currentP->username != username)
		{
			newUser = new crt_user;
			newInformation = new user;
			strcpy(userTable[userCount],username.c_str());
			newInformation->uid = userCount;
			userCount++;
			newInformation->username = username;
			newInformation->password = password1;
			p->nextP = newUser;
			newUser->prevP = p;
			newUser->nextP = NULL;
			newUser->currentP = newInformation;
			cout<<"新用户创建成功!"<<endl;
		}
		else
			cout<<"[Username]:"<<username<<"不可用!"<<"创建新用户失败."<<endl;
	}
	else
		cout<<"你的用户没有useradd权限."<<endl;
}
void Fileos::su()
{
	int times = 0;
	string username;
	string password;
	crt_user *logUser;
	logUser = rootUser;
	cout<<"[Username]:";
	cin>>username;
	while(logUser!=NULL)
	{
		if(logUser->currentP->username == username)
			break;
		else
			logUser = logUser->nextP;
	}
	if(logUser == NULL)
		cout<<"不存在的用户:"<<username<<endl;
	else
	{
		while(times<3)
		{
			cout<<"[Password]:";
			system("stty -echo");
			cin>>password;
			system("stty echo");
			if(password == logUser->currentP->password)
				break;
			times++;
			cout<<endl;
			cout<<"输错密码"<<times<<"次."<<"输错三次密码将自动退出程序!"<<endl;
		}
		if(times<3)
		{
			cout<<endl;
			cout<<"用户登录成功!欢迎"<<logUser->currentP->username<<"~"<<endl;
			CURRENTUSER = logUser->currentP->username;
			CURRENTUSERUID = logUser->currentP->uid;
		}
	}
}
void Fileos::touch()
{
	string filename;
	inode *p;
	file *newFile;
	p = currentRootNode;
	cout<<"请输入要创建的文件的名字:";
	cin>>filename;
	while(p->i_next!=NULL && p->i_name != filename)
		p = p->i_next;
	if(p->i_next == NULL && p->i_name != filename)
	{
		nextNode = new inode;
		newFile = new file;
		lastNode = p;
		lastNode->i_next = nextNode;
		nextNode->i_prev = lastNode;
		nextNode->i_next = NULL;
		nextNode->i_extend = NULL;
		nextNode->i_uid = CURRENTUSERUID;
		nextNode->i_mode = 644;
		nextNode->i_flag = 1;
		nextNode->i_type = '-';
		newFile->f_size = 0;
		newFile->f_date = op.getTime();
		nextNode->f = newFile;
		nextNode->i_name = filename;
		lastNode = nextNode;
		cout<<"创建文件成功!"<<endl;
	}
	else
		cout<<"touch: "<<"无法创建新文件"<<"\""<<filename<<"\""<<": 文件已存在"<<endl;
}
void Fileos::vim()
{
	string filename;
	string text;
	inode *p = currentRootNode;
	cout<<"请输入要编辑的文件的名字:";
	cin>>filename;
	getchar();
	while(p->i_name!=filename && p->i_next!=NULL)
		p = p->i_next;
	if(p!=NULL && p->i_name == filename)
	{
		if(p->i_type == 'd')
			cout<<"错误: 目录不可写!"<<endl;
		else if(p->i_type != 'd')
		{
			if(CURRENTUSERUID!=0 && CURRENTUSERUID != p->i_uid && ((p->i_mode%10)&2) == 0)
				cout<<"你的用户没有写入文件内容的权限!"<<endl;
			else
			{
				cout<<"请输入文件的内容:";
				getline(cin,text);
				p->f->f_text = text;
				p->f->f_size = text.length() * sizeof(char);
				p->f->f_date = op.getTime();
				cout<<"编辑文件成功!"<<endl;
			}
		}
	}
	else
		cout<<"vim: 无法编辑"<<"\'"<<filename<<"\'"<<": 没有这个文件"<<endl;
}
void Fileos::cat()
{
	string filename;
	inode *p = currentRootNode;
	cout<<"请输入要查看的文件的名字:";
	cin>>filename;
	while(p->i_name!=filename && p->i_next!=NULL)
		p = p->i_next;
	if(p!=NULL && p->i_name == filename)
	{
		if(p->i_type == 'd')
			cout<<"错误: 目录不可读!"<<endl;
		else if(p->i_type != 'd')
		{
			if(CURRENTUSERUID !=0 && CURRENTUSERUID != p->i_uid && ((p->i_mode%10)&4) == 0)
				cout<<"你的用户没有读取文件内容的权限!"<<endl;
			else if(p->f->f_size == 0)
				cout<<endl;
			else
				cout<<p->f->f_text<<endl;
		}
	}
	else
		cout<<"cat: 无法查看"<<"\'"<<filename<<"\'"<<": 没有这个文件"<<endl;
}
void Fileos::rm()
{
	string filename;
	inode *p = currentRootNode;
	inode *Prev = new inode;
	cout<<"请输入要删除的文件的名字:";
	cin>>filename;
	while(p->i_name!=filename && p->i_next!=NULL)
		p = p->i_next;
	if(p!=NULL && p->i_name == filename)
	{
		if(p->i_type == 'd')
			cout<<"错误: rm命令不可对目录使用! 删除目录 ---- rmdir"<<endl;
		else if(p->i_type !='d')
		{
			if(CURRENTUSERUID != 0 && CURRENTUSERUID != p->i_uid)
				cout<<"你的用户没有删除文件的权限!"<<endl;
			else
			{
				Prev = p->i_prev;
				Prev->i_next = p->i_next;
				if(Prev->i_next != NULL)
					p->i_next->i_prev = p->i_prev;
				delete p;
				cout<<"文件删除成功!"<<endl;
			}
		}
	}
	else	
		cout<<"rm: 无法删除"<<"\'"<<filename<<"\'"<<": 没有这个文件"<<endl;
}
void Fileos::rmdir()
{
	string dirname;
	inode *p = currentRootNode;
	inode *Prev = new inode;
	cout<<"请输入要删除的目录的名字:";
	cin>>dirname;
	while(p->i_name != dirname && p->i_next!=NULL)
		p = p->i_next;
	if(p!=NULL && p->i_name == dirname)
	{
		if(p->i_type != 'd')
			cout<<"错误: rmdir命令不可对文件使用! 删除文件 ---- rm"<<endl;
		else if(p->i_type == 'd')
		{
			if(CURRENTUSERUID != 0 && CURRENTUSERUID != p->i_uid)
				cout<<"你的用户没有删除目录的权限!"<<endl;
			else
			{
				Prev = p->i_prev;
				Prev->i_next = p->i_next;
				if(Prev->i_next != NULL)
					p->i_next->i_prev = p->i_prev;
				delete p;
				cout<<"目录删除成功!"<<endl;
			}
		}
	}
	else
		cout<<"rmdir: 无法删除"<<"\'"<<dirname<<"\'"<<": 没有这个目录"<<endl;
}
void Fileos::cd()
{
	string dirname;
	inode *p;
	inode *newNode;
	p = currentRootNode;
	cout<<"请输入要访问的目录的名字:";
	cin>>dirname;
	while(p->i_next!=NULL && p->i_name != dirname)
		p = p->i_next;
	if(p!=NULL && p->i_name == dirname)
	{
		if(p->i_type != 'd')
			cout<<"错误: 文件无法使用cd命令访问!"<<endl;
		else if(CURRENTUSERUID != 0 && CURRENTUSERUID != p->i_uid && ((p->i_mode%10)&1) == 0)
			cout<<"你的用户没有访问目录的权限!"<<endl;
		else if(p->i_extend == NULL)
		{
			newNode = new inode;
			CURRENTDIR = CURRENTDIR+dirname;
			CURRENTDIR = CURRENTDIR+"/";
			newNode->i_prev = p;
			newNode->i_next = NULL;
			p->i_extend = newNode;
			i_stack.push(currentRootNode);
			currentRootNode = newNode;
		}
		else
		{
			CURRENTDIR = CURRENTDIR+dirname;
			CURRENTDIR = CURRENTDIR+"/";
			i_stack.push(currentRootNode);
			currentRootNode = p->i_extend;
		}
	}
	else if(p!=NULL && dirname  == "..")
	{
		if(i_stack.empty())
			cout<<"错误: 无法返回上一级目录! 当前目录已是根目录."<<endl;
		else
		{
			currentRootNode = i_stack.top();
			i_stack.pop();
			int len = CURRENTDIR.length();
			char tmp[105];
			strcpy(tmp,CURRENTDIR.c_str());
			for(int i = len-2;i>=0;i--)
			{
				if(tmp[i] == '/')
				{
					tmp[i+1] = '\0';
					CURRENTDIR = tmp;
					break;
				}
			}
		}
	}
	else
		cout<<"cd: 无法访问"<<"\'"<<dirname<<"\'"<<": 没有这个目录"<<endl;
}
void Fileos::InitNode()
{
	while(!i_stack.empty())
		i_stack.pop();
	node = new inode;
	node->i_prev = NULL;
	node->i_next = NULL;
	node->i_uid = 0;
	node->i_mode = 755;
	node->i_flag  = 1;
	node->i_type = 'd';
	node->i_name = "/";
	CURRENTDIR = node->i_name;
	rootNode = node;
	op.mkdir("bin",node,755,0);
	node = nextNode;
	op.mkdir("home",node,755,0);
	node = nextNode;
	op.mkdir("lib",node,755,0);
	node = nextNode;
	op.mkdir("media",node,755,0);
	node = nextNode;
	op.mkdir("proc",node,755,0);
	node = nextNode;
	op.mkdir("sbin",node,755,0);
	node = nextNode;
	op.mkdir("sys",node,755,0);
	node = nextNode;
	op.mkdir("var",node,755,0);
	node = nextNode;
	op.mkdir("boot",node,755,0);
	node = nextNode;
	op.mkdir("dev",node,755,0);
	node = nextNode;
	op.mkdir("lib64",node,755,0);
	node = nextNode;
	op.mkdir("mnt",node,755,0);
	node = nextNode;
	op.mkdir("root",node,755,0);
	node = nextNode;
	op.mkdir("snap",node,755,0);
	node = nextNode;
	op.mkdir("etc",node,755,0);
	node = nextNode;
	op.mkdir("usr",node,755,0);
	node = nextNode;
	lastNode = node;
	node = rootNode;
	currentRootNode = rootNode;
	rootUser = &currentUser;
}
void File_os()
{
	string command;
	op.createSuper();
	op.login();
	op.help();
	op.InitNode();
	op.cls();
	while(true)
	{
		if(CURRENTUSER == rootUser->currentP->username)
			cout<<CURRENTUSER<<"(root)@wangmeow-Inspiron-7560:"<<CURRENTDIR<<"$ ";
		else
			cout<<CURRENTUSER<<"@wangmeow-Inspiron-7560:"<<CURRENTDIR<<"$ ";
		cin>>command;
		if(command == "help")
			op.help();
		else if(command == "logout")
		{
			op.logout();
			break;
		}
		else if(command == "clear")
			op.cls();
		else if(command == "pwd")
			op.pwd();
		else if(command == "ls")
			op.ls();
		else if(command == "mkdir")
			op.userMkdir();
		else if(command == "ll")
			op.ll();
		else if(command == "chmod")
			op.chmod();
		else if(command == "mv")
			op.mv();
		else if(command == "useradd")
			op.useradd();
		else if(command == "su")
			op.su();
		else if(command == "touch")
			op.touch();
		else if(command == "vim")
			op.vim();
		else if(command == "cat")
			op.cat();
		else if(command == "rm")
			op.rm();
		else if(command == "rmdir")
			op.rmdir();
		else if(command == "cd")
			op.cd();
		else
		{
			cout<<"未找到"<<" '"<<command<<"' "<<"命令"<<endl;
			cout<<command<<": "<<"未找到命令"<<endl;
		}
	}
}
int main()
{
	File_os();
	return 0;
}
