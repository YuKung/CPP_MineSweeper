#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <easyx.h>
#include <process.h>  //���߳�
#include <mmsystem.h>                   //������ý���豸�ӿ�ͷ�ļ�,һ������graphics.h����
#include <stdbool.h>
#pragma comment(lib,"winmm.lib")        //���ؾ�̬��

#define COL 20  // ����
#define ROW 15   // ��� 
#define SIZE 25	  //ÿ����ͼ��size
#define GRAPH_NUM 15

int map[ROW + 2][COL + 2];//��������ʱ��Ҫ�ж���Χ�˸����ڵĸ����Ƿ����ף�+2��ֹ�����߽����ʱ�������������
int count = 0;		 //�㿪���Ӽ���
int mineNum = 0;
int timer = 0; //��ʱȫ�ֱ���
bool godMode = 0;
bool timingStart = 0;  //���Ƽ�ʱ��ʼȫ�ֱ���
char timeRecord[10] = { 0 };
IMAGE img[GRAPH_NUM];     //�洢ͼƬ
IMAGE backgroud;

class Game {
public:
	int InitGame();   // ��ʼ����Ϸ �����ף�ÿ���ո�Ź�������Χ����������ÿ�����ӽ��м��ܴ���
	int play();			// ��������ֵ
};

class Graph {
public:
	void GraphLoading();
	void gameDraw();  // ������Ϸ��ͼ
};

class TimeCounter {
public:
	void TimeCounting(void* none); // ��ʱ������
	void Recording(int pass);    // ����Ϸ��¼д���ļ��ĺ���
};


void MainMenu(HWND window);
void ShowRecording();
void BlankOpen(int r, int c);
void boom();  //��ը��չʾ������
int print();   // ��ӡ����ǰʣ���׵�����
 

//��Ϸ������
int main()
{
	TimeCounter timecounter;
	_beginthread(timecounter.TimeCounting(), 0, NULL);
restart:
	timingStart = 0;// ��ʱ�رգ���ֹ�ڿ�ʼ�˵���ʾ��ʱ
	HWND window = initgraph(COL * SIZE + 220, ROW * SIZE);
	SetWindowText(window, "C++ɨ��С��Ϸ by �ԛ���");
	MainMenu(window);
	mciSendString("close BGM ", 0, 0, 0);
	mciSendString("open ./BGM1.MP3 alias BGM", NULL, 0, NULL);  //���ý���豸�ӿ�(mci)����(send)һ���ַ���(string)
	mciSendString("play BGM repeat", NULL, 0, NULL);
	putimage(0, 0, &backgroud); //�������뱳��ͼ
	Graph graph;
	graph.GraphLoading();
	Game MineSweeper;
	MineSweeper.InitGame(); //�����ף���ʼ��������
	while (1)
	{
		graph.gameDraw(); //ִ��һ��play���ظ�����ѭ��������ͼ
		print(); //��ӡ����ǰʣ���׵�����
		ShowRecording();
		if (MineSweeper.play() == -1) //play���������һֱ���н��յ�����ӵ���Ϣ����������Ӻ�ŷ���
		{
			boom();
			graph.gameDraw();  //�ȵ��׺�ȫ����������ͼչʾ������
			mciSendString("close BGM ", 0, 0, 0);
			mciSendString("open ./��ը��Ч.wav alias BGM", 0, 0, 0);
			mciSendString("play BGM", 0, 0, 0);
			timecounter.Recording(0); //ͨ��ʧ�ܲ���¼
			int is_ok = MessageBox(window, "�ȵ���������Ϸ����!\n���������Ҫ����һ�����Կ���", "", MB_OKCANCEL);
			if (is_ok == IDOK)
			{
				mciSendString("close BGM ", 0, 0, 0);
				godMode = 0;
				goto restart;
			}
			break;
		}
		else if (ROW * COL - mineNum == count)  //�ҵ��������׻��߱����������
		{
			mciSendString("close BGM ", 0, 0, 0);
			mciSendString("open ./ͨ����Ч.wav alias BGM", 0, 0, 0);
			mciSendString("play BGM", 0, 0, 0);
			Recording(1); //ͨ�سɹ�����¼
			int is_ok = MessageBox(window, "��ϲ��һ��ͨ�أ�\n��ô����Ҫ��Ҫ����һ��ѽ��", "", MB_OKCANCEL);
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
	mciSendString("open ./BGM.MP3 alias BGM", NULL, 0, NULL);  // ���ý���豸�ӿ�(mci)����(send)һ���ַ���(string)
	mciSendString("play BGM repeat", NULL, 0, NULL);       // ��������
	BeginBatchDraw();
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	loadimage(&backgroud, "./background.png", COL * SIZE + 220, ROW * SIZE);
	putimage(0, 0, &backgroud);       //����ͼƬ
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 30);
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 50, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 80);
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 100, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 130);
	fillrectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 150, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 180);
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 7, "��ģʽ");
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 57, "��ͨģʽ");
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 107, "����ģʽ");
	outtextxy((COL * SIZE + 200) / 2 - 100 + 65, SIZE * 6 + 157, "�ϵ�ģʽ");
	EndBatchDraw();

	ExMessage msg;
	int flag = 1;
	while (flag)
	{
	again: //�����������
		if (peekmessage(&msg, EX_MOUSE))
		{
			//��һ������
			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 && msg.y < SIZE * 6 + 30) //��һ������ 
			{

				BeginBatchDraw();   // ��ͣ��Ч
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
			else //�ƿ������ͣ��Ч
			{
				BeginBatchDraw();
				setlinecolor(WHITE);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 30);
				EndBatchDraw();
			}

			//�ڶ�������
			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 + 50 && msg.y < SIZE * 6 + 80) //�ڶ������� 
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
			else //�ƿ������ͣ��Ч
			{
				BeginBatchDraw();
				setlinecolor(WHITE);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 50, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 80);
				EndBatchDraw();
			}


			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 + 100 && msg.y < SIZE * 6 + 130) //���������� 
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
			else //�ƿ������ͣ��Ч
			{
				BeginBatchDraw();
				setlinecolor(WHITE);
				rectangle((COL * SIZE + 200) / 2 - 100, SIZE * 6 + 100, (SIZE * COL + 200) / 2 + 100, SIZE * 6 + 130);
				EndBatchDraw();
			}

			if (msg.x > (COL * SIZE + 200) / 2 - 100 && msg.x < (COL * SIZE + 200) / 2 + 100 && msg.y > SIZE * 6 + 150 && msg.y < SIZE * 6 + 180) //���ĸ����� 
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
					InputBox(passWord, 50, "�����뼤������");
					if (strcmp(passWord, rightWord) == 0)
					{
						flag = 0;
						mineNum = 30;
						godMode = 1;
						break;
					}
					else
					{
						MessageBox(window, "�������", "", MB_OK);
						goto again;
					}


				}

			}
			else //�ƿ������ͣ��Ч
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
		if (timingStart) {        // ��ʼ��ʱ���ж�
			//settextstyle(&f);// ����������ʽ
			settextcolor(BLACK);
			sprintf(timeRecord, "%2d:%2d", timer / 60, timer % 60); //��ʱ���ʽ��Ϊ�ַ���
			fillrectangle(580, 50, 580 + 100, 100);
			setlinecolor(BLACK);
			rectangle(580, 50, 580 + 100, 100);
			settextstyle(20, 0, "����");
			outtextxy(600, 70, timeRecord);
			Sleep(1000);  // 1000ms���ʱ������
			timer++;
		}
	}
}

void TimeCounter::Recording(int pass) //��¼����ʱ�䣬�Ƿ�ͨ��,��Ϸ��ʱ��д���ļ�    //��סÿ�����г�����ʾ  �ɶ�������Ȼ���жϵ�һ�в�Ϊ0�ĵط��������
{
	timingStart = 0;
	FILE* fp;
	fp = fopen("./recording.txt", "a");
	time_t endingTime = time(NULL); //����ʱ��
	fputs("��", fp);
	fputs(ctime(&endingTime), fp); //������ʱ��ת��Ϊ�ַ���д���ļ�
	if (pass)
		fprintf(fp, "�ɹ�ͨ�� ��ʱ:%s\n", timeRecord); //д��ͨ��״̬���з�����Ϊfputs��д�뻻�з�
	else
		fprintf(fp, "ͨ��ʧ�� ��ʱ:%s\n", timeRecord); //д��ͨ��״̬���з�����Ϊfputs��д�뻻�з�
	fclose(fp);
}

void ShowRecording()
{
	char text[1000][55] = { 0 };
	FILE* fp;
	fp = fopen("./recording.txt", "r");
	setbkcolor(TRANSPARENT);
	outtextxy(500, 170, "������������¼��");
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

//����ͼƬ
void Graph::GraphLoading()
{
	for (int i = 0; i <= GRAPH_NUM - 1; i++)
	{
		char fileName[20] = "";
		sprintf(fileName, "./image/%d.gif", i);
		loadimage(&img[i], fileName, SIZE, SIZE);
	}
}

													
int Game::InitGame()									//���ױ��Ϊ-1���޵��ױ��Ϊ0	//����time�������������ѡ����Ԫ�ء�
{
	timer = 0;
	timingStart = 1;
	count = 0;
	for (int i = 0; i < 10; i++)
	{
		timeRecord[i] = 0;    //��ʱ�ַ�����ʼ��
	}
	for (int i = 0; i < ROW + 2; i++)
	{
		for (int j = 0; j < COL + 2; j++)
		{
			map[i][j] = 0;                      //��ʼ����ͼ
		}
	}
	srand((unsigned)time(NULL));
	for (int n = 0; n < mineNum;)  //��������δ�ﵽҪ��ֵʱһֱ�������ɵ���
	{
		int row = rand() % ROW + 1;
		int col = rand() % COL + 1;
		if (map[row][col] == 0)    // ����Ԫ��Ϊ�յ�ʱ��������ף�
		{							// ��ֹ�ø����Ѿ������ס������������׵�����������

			{

				map[row][col] = -1;
				n++;

			}
		}
	}

	//�������飬���ҵ�����û���׵ĸ��ӣ��ٱ�������û���׵ĸ�����Χ�˸��ҵ���Χ�׵�������
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
							map[i][j]++;          // ��������Χ��λ�ü�һ
						}
					}
				}
			}
		}
	}

	//���� ��Ϊ����ͨ���Ƚ�����Ԫ�ص�ֵ��С������ͼ���ݣ�
	//�����׺�����ͨ����ÿ������Ԫ�ؼ���һ���ϴ�ֵ�ķ�ʽ�����ǽ��С����ܡ���ͳһ��ͼ
	for (int i = 1; i <= ROW; i++)
	{
		for (int j = 1; j <= COL; j++)
		{
			map[i][j] += 20;
		}
	}
	return 0;
}


//��ӡ��Ϸ��
void Graph::gameDraw()
{
	for (int i = 1; i <= ROW; i++)
	{
		for (int j = 1; j <= COL; j++)
		{
			//printf("%3d", map[i][j]); //��������
		   //������
			if (map[i][j] == -1)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[14]);
			}
			//������
			else if (map[i][j] >= 0 && map[i][j] <= 8)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[map[i][j]]);
			}
			//�����ܲ�
			else if (map[i][j] >= 19 && map[i][j] <= 28)
			{
				if (godMode)
				{
					if (map[i][j] == 19) //���ڵ���
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
			//���
			else if (map[i][j] >= 39 && map[i][j] <= 48)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[10]);
			}
			//���ʺ�
			else if (map[i][j] >= 59 && map[i][j] <= 68)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[12]);
			}
			//��ը��
			else if (map[i][j] == -2)
			{
				putimage((j - 1) * SIZE, (i - 1) * SIZE, &img[13]);
			}
		}
		printf("\n");
	}
}



//��������δ������ĸ���ʱ ����չ��
//��������Ϣ֪�����ҵ���ĸ�����һ���հ׸��ӣ���ô����������ʼ����Χ�İ˸����ӽ��б���������������հ׸�����ô�ͽ�����һ�α���
void BlankOpen(int r, int c)
{
	//�򿪸���
	if (map[r][c] >= 59 && map[r][c] <= 68) //���㿪�ʺű������ʱ��
	{
		map[r][c] -= 60;
	}
	else
	{
		map[r][c] -= 20;
	}
	count++;
	//�㿪������Ź���
	for (int m = r - 1; m <= r + 1; m++)
	{
		for (int n = c - 1; n <= c + 1; n++)
		{
			if (m >= 1 && m <= ROW && n >= 1 && n <= COL)			//��֤����Ϸ��
			{
				if (map[m][n] >= 19 && map[m][n] <= 28)				//����Ϊ�հ׸�
				{
					if (map[m][n] != 20)         // ��Χ�������ֱ�ǲ�Ϊ0��չ����ݹ����
					{
						map[m][n] -= 20;
						count++;
					}
					else                   // ��Χ�������ֱ��Ϊ0�Ļ��ظ���������
					{
						BlankOpen(m, n);
					}

				}

				if (map[m][n] >= 59 && map[m][n] <= 68)
				{
					if (map[m][n] != 60)                       // ��Χ�������ֱ�ǲ�Ϊ0��չ����ݹ����
					{
						map[m][n] -= 40;
						count++;
					}
					else     // ��Χ�������ֱ��Ϊ0�Ļ��ظ���������
					{
						BlankOpen(m, n);
					}

				}
			}
		}
	}
}

//���ʱ������и��ӽ���
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

//��ʾʣ������ ԭ��ÿ��������һ�ι��󣬶����ŵ�ͼ���б�������δ���ܵ�������ʾ������
int print()
{
	char num[10] = { 0 };
	setbkmode(TRANSPARENT);
	setbkcolor(WHITE);
	outtextxy(520, 50, "����:");
	sprintf(num, "%02d", mineNum);
	outtextxy(520, 80, num);
	return 0;
}

//���������
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
				if (r >= 1 && r <= ROW && c >= 1 && c <= COL) //��֤���������Ϸ���ڵĸ���
				{
					if (map[r][c] >= 19 && map[r][c] <= 28) // ��δ�㿪�ĸ���(�Ǻ�����ʺű��)
					{
						if (map[r][c] == 20) // ��������������ո�
						{
							BlankOpen(r, c); // ��Χչ��
							return map[r][c]; // ���ط�-1ֵ
						}
						else
						{
							map[r][c] -= 20;
							count++;
							return map[r][c];   // ����������-1
						}
					}

					if (map[r][c] >= 59 && map[r][c] <= 68)  // ���ʺű�ǵĸ���
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


				//�����ӣ�������
			case WM_RBUTTONDOWN:
				r = msg.y / SIZE + 1;
				c = msg.x / SIZE + 1;
				if (r >= 1 && r <= ROW && c >= 1 && c <= COL) //��֤���������Ϸ���ڵĸ���
				{
					if (map[r][c] >= 19 && map[r][c] <= 28)  //��һ�ε��  ������
					{
						map[r][c] += 20;
					}
					else if (map[r][c] >= 39 && map[r][c] <= 48)  //�ڶ��ε��  ���ʺ�
					{
						map[r][c] += 20;
					}
					else if (map[r][c] >= 59 && map[r][c] <= 68)  //�����ε��  ���δ���״̬
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