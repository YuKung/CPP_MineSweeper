#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <easyx.h>
#include <process.h>  //多线程
#include <mmsystem.h>                   //包含多媒体设备接口头文件,一定放在graphics.h下面
#include <stdbool.h>
#pragma comment(lib,"winmm.lib")        //加载静态库

#define COL 20  // 长度
#define ROW 15   // 宽度 
#define SIZE 25	  //每张贴图的size
#define GRAPH_NUM 15

int map[ROW + 2][COL + 2];//翻开格子时候要判断周围八个以内的格子是否有雷，+2防止翻开边界格子时候发生数组溢出。
int count = 0;		 //点开格子计数
int mineNum = 0;
int timer = 0; //计时全局变量
bool godMode = 0;
bool timingStart = 0;  //控制计时开始全局变量
char timeRecord[10] = { 0 };
IMAGE img[GRAPH_NUM];     //存储图片
IMAGE backgroud;

class Game {
public:
	int InitGame();   // 初始化游戏 生成雷，每个空格九宫格内周围的雷数，对每个格子进行加密处理
	int play();			// 返回数组值
};

class Graph {
public:
	void GraphLoading();
	void gameDraw();  // 绘制游戏地图
};

class TimeCounter {
public:
	void TimeCounting(void* none); // 计时器函数
	void Recording(int pass);    // 将游戏记录写入文件的函数
};


void MainMenu(HWND window);
void ShowRecording();
void BlankOpen(int r, int c);
void boom();  //爆炸后展示所有雷
int print();   // 打印出当前剩余雷的数量
 

//游戏主函数
int main()
{
	TimeCounter timecounter;
	_beginthread(timecounter.TimeCounting(), 0, NULL);
restart:
	timingStart = 0;// 计时关闭，防止在开始菜单显示计时
	HWND window = initgraph(COL * SIZE + 220, ROW * SIZE);
	SetWindowText(window, "C++扫雷小游戏 by 赵浡屹");
	MainMenu(window);
	mciSendString("close BGM ", 0, 0, 0);
	mciSendString("open ./BGM1.MP3 alias BGM", NULL, 0, NULL);  //向多媒体设备接口(mci)发送(send)一个字符串(string)
	mciSendString("play BGM repeat", NULL, 0, NULL);
	putimage(0, 0, &backgroud); //重新载入背景图
	Graph graph;
	graph.GraphLoading();
	Game MineSweeper;
	MineSweeper.InitGame(); //生成雷，初始化，加密
	while (1)
	{
		graph.gameDraw(); //执行一次play后重复进入循环反复贴图
		print(); //打印出当前剩余雷的数量
		ShowRecording();
		if (MineSweeper.play() == -1) //play函数自身会一直运行接收点击格子的信息，点击到格子后才返回
		{
			boom();
			graph.gameDraw();  //踩到雷后全解密重新贴图展示所有雷
			mciSendString("close BGM ", 0, 0, 0);
			mciSendString("open ./爆炸音效.wav alias BGM", 0, 0, 0);
			mciSendString("play BGM", 0, 0, 0);
			timecounter.Recording(0); //通关失败并记录
			int is_ok = MessageBox(window, "踩到雷啦，游戏结束!\n别灰心啦，要再来一把试试看？", "", MB_OKCANCEL);
			if (is_ok == IDOK)
			{
				mciSendString("close BGM ", 0, 0, 0);
				godMode = 0;
				goto restart;
			}
			break;
		}
		else if (ROW * COL - mineNum == count)  //找到了所有雷或者标记了所有雷
		{
			mciSendString("close BGM ", 0, 0, 0);
			mciSendString("open ./通关音效.wav alias BGM", 0, 0, 0);
			mciSendString("play BGM", 0, 0, 0);
			Recording(1); //通关成功并记录
			int is_ok = MessageBox(window, "恭喜你一命通关！\n这么厉害要不要再来一把呀？", "", MB_OKCANCEL);
			if (is_ok == IDOK)
			{
				mciSendString("close BGM ", 0, 0, 0);
				godMode = 0;
				goto restart;
			}
			break;
		}
	}
	closegraph();
	return 0;
}

void MainMenu(HWND window)
{
	mciSendString("open ./BGM.MP3 alias BGM", NULL, 0, NULL);  // 向多媒体设备接口(mci)发送(send)一个字符串(string)
	mciSendString("play BGM repeat", NULL, 0, NULL);       // 播放音乐
	BeginBatchDraw();
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	loadimage(&backgroud, "./background.png", COL * SIZE + 220, ROW * SIZE);
	putimage(0, 0, &backgroud);       //加载图片
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 30);
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 50, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 80);
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 100, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 130);
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 150, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 180);
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 7, "简单模式");
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 57, "普通模式");
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 107, "困难模式");
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 157, "上帝模式");
	EndBatchDraw();

	ExMessage msg;
	int flag = 1;
	while (flag)
	{
	again: //密码输入错误
		if (peekmessage(&msg, EX_MOUSE))
		{
			//第一个按键
			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 && msg.y < SIZE * 6 + 30) //第一个按键 
			{

				BeginBatchDraw();   // 悬停特效
				setlinecolor(BLACK);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 30);
				EndBatchDraw();

				switch (msg.message)
				{
				case WM_LBUTTONDOWN:
					flag = 0;
					mineNum = 10;
					break;
				}

			}
			else //移开清除悬停特效
			{
				BeginBatchDraw();
				setlinecolor(WHITE);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 30);
				EndBatchDraw();
			}

			//第二个按键
			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 + 50 && msg.y < SIZE * 6 + 80) //第二个按键 
			{
				BeginBatchDraw();
				setlinecolor(BLACK);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 50, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 80);
				EndBatchDraw();

				switch (msg.message)
				{
				case WM_LBUTTONDOWN:
					flag = 0;
					mineNum = 30;
					break;
				}

			}
			else //移开清除悬停特效
			{
				BeginBatchDraw();
				setlinecolor(WHITE);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 50, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 80);
				EndBatchDraw();
			}


			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 + 100 && msg.y < SIZE * 6 + 130) //第三个按键 
			{

				BeginBatchDraw();
				setlinecolor(BLACK);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 100, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 130);
				EndBatchDraw();

				switch (msg.message)
				{
				case WM_LBUTTONDOWN:
					flag = 0;
					mineNum = 50;
					break;
				}

			}
			else //移开清除悬停特效
			{
				BeginBatchDraw();
				setlinecolor(WHITE);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 100, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 130);
				EndBatchDraw();
			}

			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 + 150 && msg.y < SIZE * 6 + 180) //第四个按键 
			{

				BeginBatchDraw();
				setlinecolor(BLACK);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 150, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 180);
				EndBatchDraw();

				switch (msg.message)
				{
				case WM_LBUTTONDOWN:
					char passWord[50];
					char rightWord[50] = "222021321102077";
					InputBox(passWord, 50, "请输入激活密码");
					if (strcmp(passWord, rightWord) == 0)
					{
						flag = 0;
						mineNum = 30;
						godMode = 1;
						break;
					}
					else
					{
						MessageBox(window, "密码错误！", "", MB_OK);
						goto again;
					}


				}

			}
			else //移开清除悬停特效
			{
				BeginBatchDraw();
				setlinecolor(WHITE);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 150, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 180);
				EndBatchDraw();
			}


		}

	}
}

void TimeCounter::TimeCounting(void* none)
{
	while (1) {
		if (timingStart) {        // 开始计时的判定
			//settextstyle(&f);// 设置字体样式
			settextcolor(BLACK);
			sprintf(timeRecord, "%2d:%2d", timer / 60, timer % 60); //将时间格式化为字符串
			fillrectangle(580, 50, 580 + 100, 100);
			setlinecolor(BLACK);
			rectangle(580, 50, 580 + 100, 100);
			settextstyle(20, 0, "宋体");
			outtextxy(600, 70, timeRecord);
			Sleep(1000);  // 1000ms后计时器自增
			timer++;
		}
	}
}

void TimeCounter::Recording(int pass) //记录游玩时间，是否通关,游戏耗时，写入文件    //记住每次运行程序显示  可读入数组然后判断第一行不为0的地方倒序输出
{
	timingStart = 0;
	FILE* fp;
	fp = fopen("./recording.txt", "a");
	time_t endingTime = time(NULL); //日历时间
	fputs("·", fp);
	fputs(ctime(&endingTime), fp); //将日历时间转换为字符串写入文件
	if (pass)
		fprintf(fp, "成功通关 用时:%s\n", timeRecord); //写入通关状态换行符，因为fputs不写入换行符
	else
		fprintf(fp, "通关失败 用时:%s\n", timeRecord); //写入通关状态换行符，因为fputs不写入换行符
	fclose(fp);
}

void ShowRecording()
{
	char text[1000][55] = { 0 };
	FILE* fp;
	fp = fopen("./recording.txt", "r");
	setbkcolor(TRANSPARENT);
	outtextxy(500, 170, "最近三次游玩记录：");
	int i;
	for (i = 0; i < 1000; i++)
	{
		fgets(text[i], 50, fp);
	}
	for (i = 0; i < 1000; i++)
	{
		if (text[i][0] == 0)
			break;
	}
	/*int j = i - 1;
	printf("%s",text[j]);*/
	int k = 0;
	for (int j = i - 1; j >= i - 6; j = j - 2)
	{
		outtextxy(500, 200 + 60 * k, text[j - 1]);
		outtextxy(500, 230 + 60 * k, text[j]);
		k++;
	}
	fclose(fp);
}

//加载图片
void Graph::GraphLoading()
{
	for (int i = 0; i <= GRAPH_NUM - 1; i++)
	{
		char fileName[20] = "";
		sprintf(fileName, "./image/%d.gif", i);
		loadimage(&img[i], fileName, SIZE, SIZE);
	}
}

													
int Game::InitGame()									//地雷标记为-1，无地雷标记为0	//利用time库生成随机数挑选数组元素。
{
	timer = 0;
	timingStart = 1;
	count = 0;
	for (int i = 0; i < 10; i++)
	{
		timeRecord[i] = 0;    //计时字符串初始化
	}
	for (int i = 0; i < ROW + 2; i++)
	{
		for (int j = 0; j < COL + 2; j++)
		{
			map[i][j] = 0;                      //初始化地图
		}
	}
	srand((unsigned)time(NULL));
	for (int n = 0; n < mineNum;)  //地雷数量未达到要求值时一直反复生成地雷
	{
		int row = rand() % ROW + 1;
		int col = rand() % COL + 1;
		if (map[row][col] == 0)    // 数组元素为空的时候才生成雷，
		{							// 防止该格子已经“有雷”，导致生成雷的数量不够。

			{

				map[row][col] = -1;
				n++;

			}
		}
	}

	//遍历数组，先找到所有没有雷的格子，再遍历所有没有雷的格子周围八格，找到周围雷的数量。
	for (int i = 1; i <= ROW; i++)
	{
		for (int j = 1; j <= COL; j++)
		{
			if (map[i][j] != -1)
			{
				for (int m = i - 1; m <= i + 1; m++)
				{
					for (int n = j - 1; n <= j + 1; n++)
					{
						if (map[m][n] == -1)
						{
							map[i][j]++;          // 并在雷周围的位置加一
						}
					}
				}
			}
		}
	}

	//加密 因为我们通过比较数组元素的值大小决定贴图内容，
	//生成雷后，我们通过给每个数组元素加上一个较大值的方式给他们进行“加密”，统一贴图
	for (int i = 1; i <= ROW; i++)
	{
		for (int j = 1; j <= COL; j++)
		{
			map[i][j] += 20;
		}
	}
	return 0;
}


//打印游戏区
void Graph::gameDraw()
{
	for (int i = 1; i <= ROW; i++)
	{
		for (int j = 1; j <= COL; j++)
		{
			//printf("%3d", map[i][j]); //测试数组
		   //贴地雷
			if (map[i][j] == -1)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[14]);
			}
			//贴数字
			else if (map[i][j] >= 0 && map[i][j] <= 8)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[map[i][j]]);
			}
			//贴加密层
			else if (map[i][j] >= 19 && map[i][j] <= 28)
			{
				if (godMode)
				{
					if (map[i][j] == 19) //对于地雷
					{
						putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[14]);
					}
					else
					{
						putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[9]);
					}
				}
				else
				{
					putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[9]);
				}

			}
			//标记
			else if (map[i][j] >= 39 && map[i][j] <= 48)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[10]);
			}
			//贴问号
			else if (map[i][j] >= 59 && map[i][j] <= 68)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[12]);
			}
			//贴炸雷
			else if (map[i][j] == -2)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[13]);
			}
		}
		printf("\n");
	}
}



//当左键点击未点击过的格子时 连续展开
//如果鼠标信息知道了我点击的格子是一个空白格子，那么将他翻开后开始对周围的八个格子进行遍历，如果遍历到空白格子那么就进行下一次遍历
void BlankOpen(int r, int c)
{
	//打开格子
	if (map[r][c] >= 59 && map[r][c] <= 68) //当点开问号标记区域时候
	{
		map[r][c] -= 60;
	}
	else
	{
		map[r][c] -= 20;
	}
	count++;
	//点开后遍历九宫格
	for (int m = r - 1; m <= r + 1; m++)
	{
		for (int n = c - 1; n <= c + 1; n++)
		{
			if (m >= 1 && m <= ROW && n >= 1 && n <= COL)			//保证是游戏区
			{
				if (map[m][n] >= 19 && map[m][n] <= 28)				//必须为空白格
				{
					if (map[m][n] != 20)         // 周围格子数字标记不为0，展开后递归结束
					{
						map[m][n] -= 20;
						count++;
					}
					else                   // 周围格子数字标记为0的话重复调用自身
					{
						BlankOpen(m, n);
					}

				}

				if (map[m][n] >= 59 && map[m][n] <= 68)
				{
					if (map[m][n] != 60)                       // 周围格子数字标记不为0，展开后递归结束
					{
						map[m][n] -= 40;
						count++;
					}
					else     // 周围格子数字标记为0的话重复调用自身
					{
						BlankOpen(m, n);
					}

				}
			}
		}
	}
}

//输的时候对所有格子解密
void boom()
{
	for (int r = 1; r <= ROW; r++)
	{
		for (int c = 1; c <= COL; c++)
		{
			if (map[r][c] == 19)
			{
				map[r][c] -= 21;
			}
			else if (map[r][c] == -1)
			{
				map[r][c] -= 1;
			}
			else if (map[r][c] == 59)
			{
				map[r][c] -= 41;
			}
		}
	}
}

//显示剩余雷数 原理每当左键点击一次过后，对整张地图进行遍历，把未解密的雷数显示出来。
int print()
{
	char num[10] = { 0 };
	setbkmode(TRANSPARENT);
	setbkcolor(WHITE);
	outtextxy(520, 50, "雷数:");
	sprintf(num, "%02d", mineNum);
	outtextxy(520, 80, num);
	return 0;
}

//鼠标点击开玩
int Game::play()
{
	ExMessage msg;
	int r = 0, c = 0;
	while (1)
	{
		if (peekmessage(&msg, EX_MOUSE))
		{
			switch (msg.message)
			{
			case WM_LBUTTONDOWN:
				r = msg.y / SIZE + 1;
				c = msg.x / SIZE + 1;
				if (r >= 1 && r <= ROW && c >= 1 && c <= COL) //保证点击的是游戏区内的格子
				{
					if (map[r][c] >= 19 && map[r][c] <= 28) // 对未点开的格子(非红旗和问号标记)
					{
						if (map[r][c] == 20) // 点击到了无雷区空格
						{
							BlankOpen(r, c); // 周围展开
							return map[r][c]; // 返回非-1值
						}
						else
						{
							map[r][c] -= 20;
							count++;
							return map[r][c];   // 有雷区返回-1
						}
					}

					if (map[r][c] >= 59 && map[r][c] <= 68)  // 对问号标记的格子
					{
						if (map[r][c] == 60)
						{
							BlankOpen(r, c);
							return map[r][c];
						}
						else
						{
							map[r][c] -= 60;
							count++;
							return map[r][c];
						}
					}
				}
				break;


				//插旗子，拔旗子
			case WM_RBUTTONDOWN:
				r = msg.y / SIZE + 1;
				c = msg.x / SIZE + 1;
				if (r >= 1 && r <= ROW && c >= 1 && c <= COL) //保证点击的是游戏区内的格子
				{
					if (map[r][c] >= 19 && map[r][c] <= 28)  //第一次点击  插旗子
					{
						map[r][c] += 20;
					}
					else if (map[r][c] >= 39 && map[r][c] <= 48)  //第二次点击  变问号
					{
						map[r][c] += 20;
					}
					else if (map[r][c] >= 59 && map[r][c] <= 68)  //第三次点击  变回未标记状态
					{
						map[r][c] -= 40;
					}
					return map[r][c];
				}
				break;
			}
		}
	}
}