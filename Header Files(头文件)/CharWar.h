#pragma warning(disable:6031)
#pragma warning(disable:6054)
#pragma warning(disable:6385)

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#include <prime.h>
#include <fstream>

const int map_wide = 640 * 1.2;	//地图or窗口大小，游玩时可以自行更改
const int map_high = 480 * 1.2; //很多字体呈现没有做适配，请不要随意改成更小尺寸
#define Block  0xC8C8FF
#define 数据 PlayersData

int Play = 1, v0 = 10, Sigma = 45, Players = -1, selfround, View = -1, Storge = 1;	//公共变量，当前玩家，玩家数，自己回合，收数据, 局域网或者本地, 目前视图, 自动使用技能
//veiw=-1>>主菜单；view=-2>>设置界面；view=-3>>个人界面；view=-4>>抽卡界面；view=-5>>开始游戏界面；view=0>>>>对战界面；view=1；帮助界面
int  Auto = 0; //Auto::0默认，-1读取存档，-2回放，1人机模式使用技能
int OnlOrLoc = 0;
const char* sever_ip = "127.0.0.1";
int port = 1234, ret;
SOCKET serverSocket{};

char AllPath[1024] = "";
char PATH[1024] = "D:\\CharWar\\CharWar.png";

FILE* Datafile;
TCHAR textrow[100];
char Data[1024] = "D:\\CharWar\\SaveGame\\Save.txt"; //PlayerData
const char* SaveName = "Save";
TCHAR MapFileLoca[MAX_PATH] = { 0 };
IMAGE CharWar;
IMAGE img;

int keystr[] = { '!', '!', '!', '!' };
int Charkey[] = { 65, 68, 87, 83, 81, 107, 109, 106, 111, 110 };	//A，D， W， S， Q， +， -， *， /， .`
const TCHAR* PlayerChar[] = { L"我", L"你", L"他", L"她", L"它", L"吾", L"私", L"俺", L"僕" };

char RecvBuffer[256];
float PlayersData[22][24] = { 0 };	//10个玩家23个技能  !!!注意：改了这个所有的ROWS和COLS都得改(包括服务器)
const int Mines = 10;			 //越多就会越少
float MinesArry[Mines * 3 + 1] = { 0 };  //中雷大雷超大雷

void Ranmap();		//生成随机地图
void LoadMap();		//加载存储地图
void playchar();	//控制及游玩主函数
void drawUI();		//绘画各个界面UI
void DrawMan();		//绘画

//获取县城最后一次的错误信息
TCHAR* ErrorMsg(DWORD dwError)
{
	TCHAR* errorMessage = new TCHAR[256];

	// 使用 FormatMessage 获取错误描述
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwError, 0, errorMessage, 256, NULL);

	return errorMessage;
}

//保存指定字符到keystr数组
void gettkey(char keyinput) {
	for (int i = 0; i <= 3; i++)
		keystr[i] = keystr[i];

	if (keystr[0] == '!') {
		keystr[0] = keyinput;
	}
	else if (keystr[1] == '!') {
		keystr[1] = keyinput;
	}
	else if (keystr[2] == '!') {
		keystr[2] = keyinput;
	}
	else if (keystr[3] == '!') {
		keystr[3] = keyinput;
	}

	if (keyinput == '#') keystr[0] = '#';

	if (OnlOrLoc == 1) {
		sprintf(RecvBuffer, "|%d, %d, %d, %d", keystr[0], keystr[1], keystr[2], keystr[3]);
		send(serverSocket, RecvBuffer, strlen(RecvBuffer), NULL);
	}
}

//分割存储字符串
void ArraytoMat(char str[], int Row, int Col, int Direct)
{
	const char delimiter[] = ", ";
	char* token; //存储每个分隔的字符串
	double float_array[10]; //存储浮点数

	// 使用strtok函数分割字符串
	token = strtok(str, delimiter);
	int i = 0;
	while (token != NULL) {
		// 使用strtod函数将分隔出来的字符串转换成浮点数，并存储到数组中
		float_array[i++] = strtod(token, NULL);
		token = strtok(NULL, delimiter);
	}
}

//返回主菜单
void CloseAndMenu()
{
	View = -1; OnlOrLoc = 0;
	closesocket(serverSocket);
	WSACleanup();
}

//发送地图
BOOL sendmap()
{
	size_t  bytesRead;
	long int fileSize;
	BOOL Retn = 1;

	Datafile = fopen(PATH, "rb");
	if (Datafile == NULL) {
		printf("文件读取失败: %s\n", PATH);
		CloseAndMenu();
		return FALSE;
	}

	//发送和文件大小
	fseek(Datafile, 0, SEEK_END);
	fileSize = ftell(Datafile);
	fseek(Datafile, 0, SEEK_SET);
	sprintf(RecvBuffer, "%ld", fileSize);	send(serverSocket, RecvBuffer, strlen(RecvBuffer), NULL);

	char* buffer = (char*)malloc(fileSize);
	if (buffer == NULL) {
		printf("分配缓冲区！\n");
		fclose(Datafile);
		CloseAndMenu();
		return FALSE;

	}
	bytesRead = fread(buffer, 1, fileSize, Datafile);
	fclose(Datafile);

	if (bytesRead != fileSize) {
		printf("读取整个文件失败\n");
		free(buffer);
		CloseAndMenu();
		return FALSE;
	}

	size_t bytesSent = 0;
	int result = 0;
	Sleep(50);
	while (bytesSent < fileSize) {
		result = send(serverSocket, buffer + bytesSent, fileSize - bytesSent, 0);
		if (result == SOCKET_ERROR) {
			printf("发送失败: %d\n", WSAGetLastError());
			free(buffer);
			CloseAndMenu();
			return FALSE;
		}
		bytesSent += result;
	}
	free(buffer);
	return TRUE;

}

//接收地图
BOOL recvmap()
{
	//接收大小
	ret = recv(serverSocket, RecvBuffer, 255, NULL);	RecvBuffer[ret] = '\0';

	long int fileSize = atol(RecvBuffer);
	char* buffer = (char*)malloc(fileSize);
	if (buffer == NULL) { CloseAndMenu(); return -1; }
	size_t bytesRect = 0;
	while (bytesRect < fileSize)
	{
		ret = recv(serverSocket, buffer + bytesRect, fileSize - bytesRect, 0);
		if (ret <= 0) { CloseAndMenu(); return -1; }
		bytesRect += ret;
	}

	FILE* file = fopen(PATH, "wb");
	if (file == NULL) {
		printf("无法打开文件保存地图！\n");
		CloseAndMenu(); return -1;
	}
	fwrite(buffer, 1, fileSize, file);
	fclose(file);
	free(buffer);

	LoadMap();
}

//对战提示标题
void DrawTitle()
{
	setorigin(0, 0);

	TCHAR strtemp[50] = _T("夢元");
	int txtwide = textwidth(strtemp);
	int txtleft = (map_wide - txtwide) / 2;
	int txttop = (map_high) / 2 - textheight(strtemp) * 10;

	
	const TCHAR* Keylist[] = { L"空格键", L"T键(Test)", L"Tab键", L"M键(Map)", L"D键(Draw)", L"L键(Load)" };
	const TCHAR* titlelist[] = { L"开始游戏", L"测试游戏", L"更换地图", L"选用本地地图", L"自订地图", L"载入存档" };
	int listnum = sizeof(titlelist) / sizeof(titlelist[0]);

	swprintf(strtemp, L"%s %s   %s %s", titlelist[0], Keylist[0], titlelist[1], Keylist[1]);
	txtleft = (map_wide - textwidth(strtemp)) / 2;

	for (int i = 1, j = 0; i <= listnum; i++) {
		swprintf(strtemp, L"%s ", Keylist[i - 1]);
		TEXTOUT(strtemp, txtleft, txttop, 5, _T("等线"), 50, 150, 255);
		txtleft += textwidth(strtemp);

		swprintf(strtemp, L"%s   ", titlelist[i - 1]);
		TEXTOUT(strtemp, txtleft, txttop, 5, _T("等线"), 255, 0, 0);
		txtleft += textwidth(strtemp);

		if (i % 2 == 0 && i < listnum) {
			j++;
			swprintf(strtemp, L"%s %s   %s %s", titlelist[j + 1], Keylist[j + 1], titlelist[j + 2], Keylist[j + 2]);
			txtleft = (map_wide - textwidth(strtemp)) / 2;
			txttop = (map_high) / 2 - textheight(strtemp) * (10 - j * 3);
		}
	}

}

//捡到包后处理
void DrawPack(int i)
{
	TEXTOUT(_T("包"), PlayersData[10][9], PlayersData[10][10], 2, _T("幼圆"), 255, 255, 255);
	int WhatPack = PlayersData[10][7];
	TCHAR strrr[50] = { 0 };
	Color Add;
	Add.red = 0;
	Add.green = 0;
	Add.blue = 0;

	switch (WhatPack)
	{
	case 1:
		swprintf(strrr, L"+300");
		if (数据[i][9] + 300 <= 4800)
			PlayersData[i][9] += 300;
		else {
			swprintf(strrr, L"+%.0f", 300 - 4800 + 数据[Play][9]);
			数据[i][9] = 4800;
		}
		Add.red = 50;
		Add.green = 255;
		Add.blue = 50;
		break;

	case 2:
		swprintf(strrr, L"+200");
		if (数据[i][10] + 200 <= 4800)
			PlayersData[i][10] += 200;
		else {
			swprintf(strrr, L"+%.0f", 200 - 4800 + 数据[Play][10]);
			数据[i][10] = 4800;
		}
		Add.red = 50;
		Add.green = 50;
		Add.blue = 255;
		break;
	case 3:
		swprintf(strrr, L"+3");
		数据[i][8] += 3;
		Add.red = 50;
		Add.green = 200;
		Add.blue = 255;
		break;
	case 4:
		swprintf(strrr, L"+15");
		数据[i][6] += 15;
		Add.red = 250;
		Add.green = 200;
		Add.blue = 200;
		break;
	case 6:
		swprintf(strrr, L"+2");
		数据[i][8] += 2;
		Add.red = 50;
		Add.green = 200;
		Add.blue = 255;
		break;
	case 7:
		swprintf(strrr, L"盾!");
		数据[i][11] = 1;
		Add.red = 150;
		Add.green = 180;
		Add.blue = 255;
		break;
	case 8:
		swprintf(strrr, L"血包");
		数据[i + 10][0] += 1;
		Add.red = 240;
		Add.green = 180;
		Add.blue = 255;
		break;

	default:
		swprintf(strrr, L"中大奖!");
		break;
	}

	TEXTOUT(strrr, 数据[i][1] + 8 - textwidth(strrr) / 2, 数据[i][2] - 15, 2, _T("幼圆"), Add.red, Add.green, Add.blue);
	PlayersData[10][7] = 0;
	PlayersData[10][9] = 9999;
	PlayersData[10][10] = 9999;

	drawUI();
}

//判断坠落和捡包判定
void FallDown(float Speed = 10)
{

	//炮台
	if (PlayersData[10][14] > 0 &&
		getpixel(数据[10][15] - 8, 数据[10][16] + 8 + 2) != Block &&
		getpixel(数据[10][15], 数据[10][16] + 8 + 2) != Block &&
		getpixel(数据[10][15] + 8, 数据[10][16] + 8 + 2) != Block)
	{
		TEXTOUT(L"枪", 数据[10][15] - 8, 数据[10][16] - 8, 3, L"幼圆", 255, 255, 255);

		if (getpixel(数据[10][15] - 8, 数据[10][16] + 8 + 2 + 8) != Block &&
			getpixel(数据[10][15], 数据[10][16] + 8 + 2 + 8) != Block &&
			getpixel(数据[10][15] + 8, 数据[10][16] + 8 + 2 + 8) != Block)

			数据[10][16] += Speed;
		else
			数据[10][16] += 1;

		TEXTOUT(L"枪", 数据[10][15] - 8, 数据[10][16] - 8, 3, L"幼圆", PlayersData[(int)数据[10][13]][3], PlayersData[(int)数据[10][13]][4], PlayersData[(int)数据[10][13]][5]);

	}

	//包
	if (PlayersData[10][7] > 0 &&
		getpixel(数据[10][9], 数据[10][10] + 14 + 2) != Block &&
		getpixel(数据[10][9] + 14, 数据[10][10] + 14 + 2) != Block &&
		getpixel(数据[10][9] + 8, 数据[10][10] + 14 + 2) != Block)
	{
		数据[10][10] += 2;

		if (PlayersData[10][10] >= map_high / 2 + 20)
		{
			PlayersData[10][7] = 0;
			PlayersData[10][9] = 9999; PlayersData[10][10] = 9999;
		}

		//包内容不为空
		if (PlayersData[10][7] > 0)
			for (int i = 1; i <= Players; i++)
				if (fabs(数据[i][1] + 8 - 数据[10][9] - 6) <= 14 && fabs(数据[i][2] + 8 - 数据[10][10] - 6) <= 14)
				{
					DrawPack(i);
					break;
				}

		TEXTOUT(L"包", PlayersData[10][9], PlayersData[10][10], 2, L"幼圆", 100, 255, 200, 255, 255, 255);
	}

	//人 down
	for (int i = 1; i <= Players; i++)
		if (PlayersData[i][9] > 0)
			if (getpixel(PlayersData[i][1] + 2, PlayersData[i][2] + 16 + 2) != Block &&
				getpixel(PlayersData[i][1] + 16 - 2, PlayersData[i][2] + 16 + 2) != Block &&
				getpixel(PlayersData[i][1] + 8, PlayersData[i][2] + 16 + 2) != Block)
			{
				int Flyyy = 0;
				Prime_RECT(PlayersData[i][1], PlayersData[i][2], PlayersData[i][1] + 14, PlayersData[i][2] + 14);

				if (getpixel(PlayersData[i][1] + 2, PlayersData[i][2] + 16 + 8) != Block &&
					getpixel(PlayersData[i][1] + 16 - 2, PlayersData[i][2] + 16 + 8) != Block &&
					getpixel(PlayersData[i][1] + 8, PlayersData[i][2] + 16 + 8) != Block)

					PlayersData[i][2] += Speed;
				else
					PlayersData[i][2] += 1;

				TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);
				if (PlayersData[i][2] >= map_high / 2.0)
					if (View == 0)
						while (1)
						{
							Prime_RECT(PlayersData[i][1] - 21, PlayersData[i][2] + 15, PlayersData[i][1] + 40, PlayersData[i][2] + 18);
							TEXTOUT(L"+  +", PlayersData[i][1] - 6, PlayersData[i][2] - 18, 3, L"幼圆", 200, 220, 50, 255, 255, 255);
							TEXTOUT(L"翅     翅", PlayersData[i][1] - 20 + Flyyy % 3, PlayersData[i][2] - 5, 1, L"幼圆", 180, 190, 255, 255, 255, 255);
							TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5], 255, 255, 255);
							PlayersData[i][2] -= 5.0;
							Flyyy++;
							if (PlayersData[i][2] <= -map_high / 2.0 - 20)
							{
								PlayersData[i][1] = RANDOM(-map_wide / 2.0 - 30, map_wide / 2.0 - 30);
								PlayersData[i][2] = -map_high / 2.0 - 20;
								PlayersData[i][9] *= 0.75;
								if (Storge > 0) {
									Datafile = fopen(Data, "a");
									if (Datafile == NULL)
										Storge = -1;
									if (Storge > 0) {
										fprintf(Datafile, "@%d,%f,", i, PlayersData[i][1]);
										fclose(Datafile);
									}
								}
								LoadMap();
								drawUI();

								for (int i = 1; i <= Players; i++)
									if (PlayersData[i][9] > 0)
										TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);

								break;
							}
							Sleep(1);

						}
					else //非对战界面直接复活
					{
						PlayersData[i][1] = RANDOM(-map_wide / 2.0 - 30, map_wide / 2.0 - 30);
						PlayersData[i][2] = -map_high / 2.0 - 20;
					}

				//包内容不为空
				if (PlayersData[10][7] > 0)
					if (fabs(数据[i][1] + 8 - 数据[10][9] - 6) <= 14 && fabs(数据[i][2] + 8 - 数据[10][10] - 6) <= 14)
					{
						DrawPack(i);
						break;
					}

			}

}

//数字键盘0-9和退格键和*(换技能组)
int GetSkill()
{
	int Key = -99;

	//数字0-9
	for (int i = '0'; i <= '9'; i++)
		if (GetAsyncKeyState(i) & 0x8000) {
			Key = i;
			break;
		}

	//小键盘0-9
	for (int i = 96; i <= 105; i++)
		if (GetAsyncKeyState(i) & 0x8000) {
			Key = i - 48;
			break;
		}

	if ((GetAsyncKeyState(Charkey[7]) & 0x8000)) Key = 42;
	if ((GetAsyncKeyState(Charkey[8]) & 0x8000)) Key = 47;
	if ((GetAsyncKeyState(Charkey[9]) & 0x8000)) Key = 46;

	if ((GetAsyncKeyState(VK_BACK) & 0x8000) || (GetAsyncKeyState(VK_ESCAPE) & 0x8000)) Key = 8;

	return Key;
}

//按键检测及返回
int KeyBack(int NanNoKey = 0)
{
	//移动电信AD
	if (GetAsyncKeyState(Charkey[0])) { gettkey('A'); delay(1);  NanNoKey = Charkey[0]; }
	if (GetAsyncKeyState(Charkey[1])) { gettkey('D'); delay(1);  NanNoKey = Charkey[1]; }

	//加减力度shif ctrl
	if (GetAsyncKeyState(Charkey[5])) { gettkey('+'); delay(80);  NanNoKey = Charkey[5]; }
	if (GetAsyncKeyState(Charkey[6])) { gettkey('-'); delay(80);  NanNoKey = Charkey[6]; }

	//跳跃
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { gettkey('&');  NanNoKey = 38; }

	//回炮
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) { gettkey('#'); delay(100); NanNoKey = 35; }

	//方向及角度WSQ
	if (GetAsyncKeyState(Charkey[2]) || GetAsyncKeyState(Charkey[3]) || GetAsyncKeyState(Charkey[4]))
	{
		if (GetAsyncKeyState(Charkey[2])) { gettkey('W'); NanNoKey = Charkey[2]; }
		if (GetAsyncKeyState(Charkey[4])) { gettkey('Q'); NanNoKey = Charkey[4]; }
		if (GetAsyncKeyState(Charkey[3])) { gettkey('S'); NanNoKey = Charkey[3]; }
		if (Sigma >= 45 || Sigma <= -30) delay(40);
		else delay(20);
	}

	return NanNoKey;
}

//联机模式下接收信息线程，注意会吞掉一些消息
DWORD WINAPI RecvMsg(LPVOID lpParam)
{
	while (1) {
		ret = recv(serverSocket, RecvBuffer, 255, NULL);
		RecvBuffer[ret] = '\0';
		if (strcmp(RecvBuffer, "SendArray") == 0)
		{
			ret = recv(serverSocket, (char*)PlayersData, sizeof(PlayersData), 0);
		}
		if (strcmp(RecvBuffer, "SendMap") == 0)
		{
			recvmap();
		}


		//ret = 6060;
	}
}

//分割数组
void PartArray(int Nums = 4, const char delimiter[] = ", ")
{
	char* token; //存储每个分隔的字符串
	char recv_array[10]{ 0 };

	//使数组前移一位以覆盖第一位标识符
	for (int i = 0; i < strlen(RecvBuffer) - 1; ++i)
		RecvBuffer[i] = RecvBuffer[i + 1];

	token = strtok(RecvBuffer, delimiter);
	int i = 0;
	while (token != NULL) {
		recv_array[i] = strtod(token, NULL);
		token = strtok(NULL, delimiter);
		i++;
	}

	for (int i = 0; i <= Nums - 1; i++)
		keystr[i] = recv_array[i];

}

//绘画UI
void drawUI()
{
	//main UI 主菜单  || 开始界面
	if (View == -1 || View == -5)
	{
		Prime_RECT();
		setorigin(0, 0);
		TEXTOUT(L"C h a r W a r", 100, 50, 40, L"华文细黑", 255, 200, 200);
		TEXTOUT(L"Ver.C.1.0", 2, getheight() - 18);
		TEXTOUT(L"按R可以重新刷新", 100, getheight() - 18);

		button(L"设置", getwidth() - 45, 0, 40, 40, 4, L"方正姚体", 255, 200, 200);
		for (int i = 0; i < 2; i++) Prime_RECT(getwidth() - 45 + i, 45 + i, getwidth() - 5 - i, 85 - i, 0, 0, 0, 255, 255, 254);
		for (int i = 0; i < 3; i++)
			for (int a = 0; a < 3; a++)
				line(getwidth() - 40, 52 + i * 12 + a, getwidth() - 10, 52 + i * 12 + a);
		Prime_RECT(getwidth() - 20, 52, getwidth() - 15, 80);
		button(L"抽卡", getwidth() - 45, 90, 40, 40, 4, L"方正姚体", 255, 200, 200, 50, 200, 200);

		if (View == -1) {
			Prime_RECT(getwidth() / 2 - 77, getheight() / 2 - 22, getwidth() / 2 + 78, getheight() / 2 + 23, 255, 200, 200, 255, 200, 200);
			Prime_RECT(getwidth() / 2 - 77, getheight() / 2 + 68, getwidth() / 2 + 78, getheight() / 2 + 113, 255, 200, 200, 255, 200, 200);
			button(L"开始叉沃", getwidth() / 2 - 70, getheight() / 2 - 15, 140, 30, 5, L"幼圆", 255, 150, 100);
			button(L"退出游戏", getwidth() / 2 - 70, getheight() / 2 + 75, 140, 30, 5, L"幼圆", 255, 50, 200);
		}
		if (View == -5)
		{
			button(L"返回", 5, 5, 50, 30, 5);
			Prime_RECT(getwidth() / 2 - 77, getheight() / 2 - 67, getwidth() / 2 + 78, getheight() / 2 - 22, 255, 200, 200, 255, 200, 200);
			Prime_RECT(getwidth() / 2 - 77, getheight() / 2 + 23, getwidth() / 2 + 78, getheight() / 2 + 68, 255, 200, 200, 255, 200, 200);
			button(L"本地叉沃", getwidth() / 2 - 70, getheight() / 2 - 60, 140, 30, 5, L"幼圆", 100, 255, 150);
			button(L"联机叉沃", getwidth() / 2 - 70, getheight() / 2 + 30, 140, 30, 5, L"幼圆", 255, 150, 50);
		}

		int QRsize = 5;
		int QRraw = sizeof(QRArry) / sizeof(QRArry[0]);
		Color QRQ;
		QRQ.red = 0;
		QRQ.green = 0;
		QRQ.blue = 0;
		TEXTOUT(L"添加作者微信：", getwidth() - QRraw * QRsize, getheight() - QRraw * QRsize - 20);

		for (int i = 0; i < QRraw; i++)
			for (int f = 0; f < QRraw; f++)
				if (QRArry[i][f] == 1) //黑色
				{
					if (i <= 2)
					{
						QRQ.red = 255;
						QRQ.green = 200;
						QRQ.blue = 200;
					}
					Prime_RECT(getwidth() - QRraw * QRsize + f * QRsize, getheight() - QRraw * QRsize + i * QRsize, getwidth() - QRraw * QRsize + (f + 1) * QRsize, getheight() - QRraw * QRsize + QRsize + i * QRsize, QRQ.red, QRQ.green, QRQ.blue);
				}

		setorigin(map_wide / 2, map_high / 2);

		//button(L"开始叉沃", getwidth()/2-70, getheight()/2-15, 200, 40, 7, "方正姚体", 255, 200, 200);
		//playchar();Lmlb
	}	//main UI 主菜单

	//设置界面
	if (View == -2)
	{
		float UIleft = -getwidth() / 2;
		float UItop = -getheight() / 2;

		const TCHAR* setlist[] = { L"文件夹位置", L"音量大小", L"设置大小", L"按键设置", L"回合设置" };
		int listnum = sizeof(setlist) / sizeof(setlist[0]);
		Prime_RECT();
		setorigin(0, 0);
		settextstyle(50, 0, L"幼圆");
		TEXTOUT(L"C h a r W a r 设置", getwidth() / 2 - textwidth(L"C h a r W a r 设置") / 2, 30, 20, L"幼圆", 255, 200, 200);
		TEXTOUT(L"Ver.C.1.0", 2, getheight() - 18);
		TEXTOUT(L"按R可以重新刷新", 100, getheight() - 18);

		button(L"返回", 5, 5, 50, 30, 5);

		for (int i = 0; i < listnum; i++)
			button(setlist[i], 20, 80 + 30 * i, textwidth(L"文件夹位置"), 25, 4, L"方正姚体", 255, 200, 200);

		setorigin(map_wide / 2, map_high / 2);

	}

	//对战界面
	if (View == 0)
	{
		float UIleft = -map_wide / 2;
		float UItop = -map_high / 2;
		int WordWide = textwidth(L"生命：");
		TCHAR strtmp[50];

		button(L"当前技能:", getwidth() - 210, 55, 65, 20, 3, L"等线", 0, 0, 100, 20, 200, 255);
		setorigin(map_wide / 2, map_high / 2);

		TEXTOUT(L"生命:", -UIleft - 210, UItop + 3, 4, L"等线", 200, 120, 120);
		swprintf(strtmp, L"%.2f", PlayersData[Play][9]);
		TEXTOUT(strtmp, -UIleft - 210 + textwidth(L"生命:"), UItop + 3, 4, L"等线", 50 + int((4800 - PlayersData[Play][9]) / 24), 240 - int((4800 - PlayersData[Play][9]) / 20), 50, 255, 255, 255);
		Prime_RECT(-UIleft - 210, UItop + 25, -UIleft - 6, UItop + 25 + 25, 255, 255, 255, 200, 0, 0);
		Prime_RECT(-UIleft - 210 + 2, UItop + 27, -UIleft - 210 + 2 + PlayersData[Play][9] / 24, UItop + 23 + 25, 50 + int((4800 - PlayersData[Play][9]) / 24), 240 - int((4800 - PlayersData[Play][9]) / 20), 50);

		WordWide = textwidth(L"护盾:");
		TEXTOUT(L"护盾:", -UIleft - 100, UItop + 3, 4, L"等线", 120, 130, 255);
		swprintf(strtmp, L"%.2f", PlayersData[Play][10]);
		TEXTOUT(strtmp, -UIleft - 100 + WordWide, UItop + 3, 4, L"等线", 50, 50, 200, 255, 255, 255);

		if (PlayersData[Play][10] > 0) {
			Prime_RECT(-UIleft - 210 + 2, UItop + 27 + 5, -UIleft - 210 + 2 + PlayersData[Play][10] / 24, UItop + 27, 0, 0, 255);
			Prime_RECT(-UIleft - 210 + 2, UItop + 25 + 23, -UIleft - 210 + 2 + PlayersData[Play][10] / 24, UItop + 25 + 23 - 5, 0, 0, 255);
			if (PlayersData[Play][10] >= 100)
				Prime_RECT(-UIleft - 210 + 2, UItop + 27, -UIleft - 210 + 2 + 25 / 6, UItop + 25 + 23, 0, 0, 255);
			if (PlayersData[Play][10] >= 4700)
				Prime_RECT(-UIleft - 210 + 2 + 200, UItop + 27, -UIleft - 210 + 2 + 4700 / 24, UItop + 25 + 23, 0, 0, 255);
		}
		//setfillcolor(AlphaPixel(120, RGB(0, 0, 255), RGB(50,240,50) ));
		//fillrectangle(-UIleft-210+2, UItop+27, -UIleft-210+2+PlayersData[Play][9]/24, UItop+23+25);


	   //TEXTOUT(L"当前技能:", -UIleft-210, UItop+55, 4, L"等线",	 0,0,100);


		TEXTOUT(L"FLUE", UIleft + 2, UItop + 2, 3, L"等线");
		Prime_RECT(UIleft + 5, UItop + 18, UIleft + 5 + 20, UItop + 18 + 63, 255, 255, 255, 120, 0, 120);
		Prime_RECT(UIleft + 7, UItop + 20 + (1 - PlayersData[Play][7] / 15) * 60, UIleft + 3 + 20, UItop + 16 + 65, int(255 - PlayersData[Play][7] * 40 / 3), int(PlayersData[Play][7] * 17), int(PlayersData[Play][7] * 20 / 3));

		TEXTOUT(L"力度：     角度：", UIleft + 40, UItop + 2, 3, L"等线", 20, 55, 20);
		Prime_RECT(UIleft + 35, UItop + 18, UIleft + 35 + 150, UItop + 18 + 15, 255, 255, 255, 120, 120, 255);
		Prime_RECT(UIleft + 37, UItop + 20, UIleft + 33 + (v0 - 5) * 7.5, UItop + 16 + 15, v0 * 10, 255 - v0 * 9, v0 * 4);
		swprintf(strtmp, L"%d", Sigma);
		TEXTOUT(strtmp, UIleft + 45 + 100, UItop + 1, 3, L"等线", 255, 255, 255, 120, 120, 120);

		TEXTOUT(L"总燃油剩下：", UIleft + 35, UItop + 37, 3, L"等线", 200, 150, 150);
		swprintf(strtmp, L"%.1f", PlayersData[Play][6]);
		TEXTOUT(strtmp, UIleft + 35 + 80, UItop + 37, 3, L"等线", 180, 120, 120, 255, 255, 255);

		TEXTOUT(L"技力点:", UIleft + 35, UItop + 39 + 16, 5, L"等线", 120, 150, 255);
		swprintf(strtmp, L"%.0f", PlayersData[Play][8]);
		TEXTOUT(strtmp, UIleft + 35 + 64, UItop + 39 + 16, 6, L"Technic", 0, 100, 255, 255, 255, 255);

		TEXTOUT(L"Round:", -50, UItop + 10, 5, L"等线", 150, 150, 240);

		swprintf(strtmp, L"%.0f", PlayersData[10][1]);
		TEXTOUT(strtmp, 20, UItop + 8, 7, L"Technic");

		const TCHAR* Wind[] = {
		L"<<<< 风", L" <<< 风", L"  << 风", L"   < 风", // 负数区间
		L"     风", L"     风 >", L"     风 >>", L"     风 >>>", L"     风 >>>>" // 正数区间
		};
		const double thresholds[] = { -4, -3, -2, -1, -0.2, 0.2, 1, 2, 3, 4 };

		for (int i = 0; i < 9; i++)
			if (thresholds[i] <= PlayersData[10][5] && PlayersData[10][5] <= thresholds[i + 1]) {
				swprintf(strtmp, L"%s", Wind[i]);
				TEXTOUT(strtmp, -50, UItop + 30, 3, L"幼圆", 0, 150, 230);
				break;
			}

		swprintf(strtmp, L"%.2f%%", fabs(PlayersData[10][5] * 25));//风速
		TEXTOUT(strtmp, -20, UItop + 45, 3, L"幼圆", 0, 150, 230);

		if (数据[10][7] > 0)
			TEXTOUT(L"包", PlayersData[10][9], PlayersData[10][10], 2, L"幼圆", 100, 255, 200);

	}
	else
		saveimage(PATHcTow(PATH));

	//绘画抛射物保护
	for (int i = 1; i <= Players; i++)
		if (PlayersData[i][11] != 0) {
			setlinecolor(RGB(50, 60, 255));
			arc(PlayersData[i][1] - 5, PlayersData[i][2] - 5, PlayersData[i][1] + 21, PlayersData[i][2] + 21, -40 * (pi / 180), 220 * (pi / 180));
		}

	if (数据[10][14] > 0) {
		TEXTOUT(L"枪", 数据[10][15] - 8, 数据[10][16] - 8, 3, L"幼圆", 数据[(int)数据[10][13]][3], 数据[(int)数据[10][13]][4], 数据[(int)数据[10][13]][5]);
		Prime_RECT(数据[10][15] - 8, 数据[10][16] - 16, 数据[10][15] + 8, 数据[10][16] - 10, 255, 255, 255, 0, 0, 0);
		Prime_RECT(数据[10][15] - 8 + 2, 数据[10][16] - 16 + 2, 数据[10][15] - 6 + 数据[10][14] * 0.024, 数据[10][16] - 10 - 2, 90, 255, 130, 0, 0, 0);
	}
}

//生成随机地图
void Ranmap()
{
	float x, y = map_high / 2;
	double random;
	setorigin(0, 0);//set world center  世界以我为原点

	Prime_RECT();
	//Prime_RECT(50,50,100,100, 255,255,255 ,150,0,0);
	setlinecolor(RGB(255, 200, 200));
	for (x = 0; x < map_wide; x++)
	{
		random = RANDOM(-3, 3);
		random = TRUNCATE(random, 2);
		y += random;
		line(x, y, x, map_high);
	}
	x = RANDOM(20, map_high / 2);
	y = RANDOM(20, map_high / 2);
	setfillcolor(Block);
	POINT pts1[] = { {x, y}, {x * 2, y / 1.5}, {x * 3 + 30, y * 1.2 - 30}, {x - 20, y * 2 - 60} };
	solidpolygon(pts1, 4);
	x = RANDOM(20, 180);
	POINT pts2[] = { {x / 1.2, y * 1.2}, {x * 2,y * 1.4}, {x * 3,y / 1.5}, {x / 1.6,y * 2} };
	solidpolygon(pts2, 4);

	/*
	x = getpixel(150,200);
	TCHAR str[80];
	_stprintf(str, _T("%d"), x);
	outtextxy(10, 60, str);
	*/

	saveimage(PATHcTow(PATH));
	DrawTitle();
}

//加载地图
void LoadMap() {
	setorigin(0, 0);
	loadimage(&CharWar, PATHcTow(PATH), map_wide, map_high, true);
	putimage(0, 0, &CharWar);

	setorigin(map_wide / 2, map_high / 2);   //set world center  世界以我为中心
	//setaspectratio(1, -1); //asix dict       我换世界轴向   别换世界轴向了字都倒过来成甲骨文了
}

//绘画人和物
void DrawMan()
{
	for (int i = 1; i <= Players; i++)
		if (PlayersData[i][9] > 0)
			TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);

	if (数据[10][14] > 0)
		TEXTOUT(L"枪", 数据[10][15] - 8, 数据[10][16] - 8, 3, L"幼圆", 数据[(int)数据[10][13]][3], 数据[(int)数据[10][13]][4], 数据[(int)数据[10][13]][5]);

	if (数据[10][7] > 0)
		TEXTOUT(L"包", PlayersData[10][9], PlayersData[10][10], 2, L"幼圆", 100, 255, 200);

}

//睡大觉
void Slp() 
{
	TEXTOUT(L"程序已挂起(按键唤醒)", -100, -map_high / 2 + 60, 5);

	ExMessage m;
	BOOL Slp = 1;
	flushmessage();
	while (Slp) {
		m = getmessage(EX_MOUSE | EX_KEY);

		switch (m.message) {
		case WM_LBUTTONDOWN:
			Slp = 0;
			break;
		case WM_KEYDOWN:
			Slp = 0;
			break;
		}

		Sleep(1);
	}
}

//打开弹窗
HRESULT CharWindows(int ToSend)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	TCHAR commandLine[256];
	swprintf(commandLine, sizeof(commandLine) / sizeof(wchar_t), L"%s\\CharDows.exe %d", PATHcTow(AllPath), ToSend);

	// 启动弹窗
	BOOL bSuccess = CreateProcess(NULL, commandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	if (!bSuccess) {
		DWORD dwError = GetLastError();
		TCHAR* Err = ErrorMsg(dwError);
		MessageBox(NULL, Err, L"CharWar", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		return HRESULT_FROM_WIN32(dwError);  // 返回相应的错误码
	}

	system("pause"); //勾石的不加暂停主程序就自己飞走了

	DWORD dwWaitResult = WaitForSingleObject(pi.hProcess, INFINITE);

	if (dwWaitResult != WAIT_OBJECT_0) {
		DWORD dwError = GetLastError();
		TCHAR* Err = ErrorMsg(dwError);
		MessageBox(NULL, Err, L"CharWar", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		return HRESULT_FROM_WIN32(dwError);  // 返回相应的错误码
	}

	// 关闭进程和线程句柄
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 1;
}

//技能帮助菜单
int SkillMenu()
{
	TCHAR strrr[50]{ 0 };

	const TCHAR* Titleccc[] = { L"快捷键", L"顺序", L"点数", L"次数", L"名称" };
	const TCHAR* SkillNames[] = { L"血包", L"跳回合",
 L"伤害提升", L"瞬移", L"皇嘉穿甲弹", L"战姬轰炸", L"天姬,参上", L"爆炸飞羊", L"Fox3飞弹", L"护盾充能", L"字符斩", L"移动到平台",
 L"上勾拳", L"裁雨留虹", L"抛物线保护", L"南蛮冲撞", L"堡垒机关枪", L"电弧陷阱", L"钻钻钻钻井", L"半程追踪弹", L"散装弹", L"战墙",
 L"充能左轮枪", L"爆炸网球" };

	char SkillDes[24][255] = {
{"回复r400血(换回合后生效)"},
{"跳过本回合，并b+2点数，o+5油"},
{"伤害r*2"},
{"瞬移到光标处"},
{"基础伤害r300，伤害半径p+1.5，无视风，对护盾伤害r*1.3"},
{"固定伤害r450，伤害半径p+2，无视风，往光标处投掷p1p枚炮弹。战姫撞墙后会对周围造成r550固定伤害，并取消投弹(若没有抛出)。"},
{"固定伤害r1000，伤害半径p+7，激光从上至下检索障碍物，遇到障碍物时轰炸"},
{"固定伤害r850，伤害半径p+5，在当前攻击方向放一只羊（移动方向p随p机），按下b回b车后向上方起飞，之后方向操控(bW或bA逆时针旋转)，超过p15p秒后(自动起飞或)消失"},//bwd是吧bA是吧
{"根据水平距离计算伤害，发射p10p枚，p随p机改变抛射角与力度"},
{"回复b300护盾值(换回合后生效)"},
{"固定伤害r1000，斩击目标处"},
{"瞬移到光标处，然后脚下生成平台"},
{"固定伤害r200，在当前攻击方向把p身p边的敌人抛高高"},
{"固定伤害r85一枚雨，伤害半径p-8，在光标范围处降下p17p枚雨，击中有血条目标时回复b50护盾值"},//wmc
{"被动，当炮弹即将对自己造成伤害时p弹p开（后面带弹字的都生效），之后失效"},
{"基础伤害r200，伤害半径p+2，在攻击方向放一头牛(移动方向p固p定)，撞到玩家时，造成r200伤害并随机p向p后抛出，遇到障碍物会爆炸并对附近造成r120伤害，一定次数后消失"},
{"在附近架一台o500血的机炮，当有敌人位置p大p幅变化时自动发射p6p枚子炮(直线，每颗r35伤害，结束后本回合不再射击)，血条清空时对附近玩家造成基础伤害r220，伤害半径p-1"},
{"固定伤害r250，伤害半径p+1，p直p线投出，被击中玩家下回合不能使用技能(但是可以o跳o回o合和使用r血r包)，不能p移p动，伤害p8p折(无论是什么技能)"},
{"固定伤害r380，伤害半径p+2，从上往下钻井，直至打中目标或钻井时间耗尽"},
{"基础伤害r420，抛出p1秒后以p直p线飞向光标处"},
{"基础伤害r200，伤害半径p+1，击中目标/障碍物后在炮弹处再散发p7p枚炮弹"},
{"在攻击方向放一堵墙"},
{"伤害半径p-6.5，原始伤害r63每枪，总共p6p发，每开两枪需再次瞄准(直线射击)，可以使用护盾为枪充能（按b+b键与b-b键），每次充能支付b100护盾，伤害加r40，再次瞄准后变回r63"},
{"基础伤害r400，向预瞄线抛出，具有球的特性，按bShift可调整爆炸延迟"}
	};

	const TCHAR* PointsStr[] = { L"0", L"0", L"3", L"2", L"4", L"5", L"10", L"8", L"7", L"2", L"10", L"3", L"2", L"6", L"2", L"6", L"6", L"3", L"4", L"4", L"6", L"2", L"4", L"4" };

	setorigin(map_wide / 2, map_high / 2);   //set world center  世界以我为中心
	int UIleft = -map_wide / 2, UItop = -map_high / 2;
	int Cur = 0, CurTemp = 1;
	int AutoCur = 9;
	char strtemp[1024]{};

	ExMessage m;
	int lopoppop = 1;

	Prime_RECT();

	TEXTOUT(L"技能描述", UIleft + 355, UItop + 35);
	TEXTOUT(L"→", UIleft + 312, UItop + 48, 15);
	Prime_RECT(UIleft + 355, UItop + 55, -UIleft - 1, -UItop - 300, 255, 255, 255, 0, 0, 0);
	TEXTOUT(L"基础伤害：可以造成暴击伤害", UIleft + 355 + 150, UItop + 35);

	button(L"退出", getwidth() - 50, 0, 50, 24);
	button(L"技能演示", 355, getheight() - 290, 80, 25, 4, L"等线", 0, 0, 100, 20, 200, 255);
	button(L"选用该技能", 355 + 80 + 5, getheight() - 290, 100, 25, 4, L"等线", 255, 0, 0, 20, 200, 255);
	setorigin(map_wide / 2, map_high / 2);

	while (lopoppop == 1)
	{
		if (AutoCur != 9)
		{
			if (AutoCur < Cur) Cur--;
			if (AutoCur > Cur) Cur++;
			if (AutoCur == Cur || Cur == -22) AutoCur = 9;
			Sleep(20);
		}
		else
		{
			m = getmessage(EX_KEY | EX_MOUSE);

			switch (m.message)
			{
			case WM_MOUSEWHEEL:
			{

				if (m.wheel < 0) {
					if (Cur >= -21)
						Cur--;
				}
				else
					if (Cur <= 0)
						Cur++;
			}
			break;

			case WM_LBUTTONDOWN: {
				//自动到顶
				if (0 < m.x && m.x < 310) {
					AutoCur = -(m.y + 15 - UItop - Cur * 30) / 30 + 12;
					if (0 < m.y && m.y < 25)
						AutoCur = 1;
				}
				//不准超出
				if (AutoCur <= -23 || AutoCur >= 2) AutoCur = 9;
				//退出
				if (getwidth() - 50 <= m.x && m.x <= getwidth() &&
					0 <= m.y && m.y <= 24)
					lopoppop = 0;

				//技能演示和选用
				if (getheight() - 290 <= m.y && m.y <= getheight() - 290 + 25)
				{
					//选用技能
					if (355 + 80 + 5 <= m.x && m.x <= 355 + 80 + 5 + 100)
						lopoppop = -1;
					//技能演示
					if (355 <= m.x && m.x <= 355 + 80)
						lopoppop = -2;
				}
			}
			break;

			case WM_KEYDOWN:
			{
				if (m.vkcode == VK_ESCAPE) lopoppop = 0;

				if (Cur <= 0 && (m.vkcode == VK_UP || m.vkcode == 'W')) Cur++;
				if (Cur >= -21 && (m.vkcode == VK_DOWN || m.vkcode == 'S')) Cur--;

			}
			break;
			}
		}

		if (CurTemp != Cur) {
			Prime_RECT(UIleft, UItop, UIleft + 310, -UItop);

			for (int i = 0; i < 24; i++)
			{
				if (i <= 9)
					swprintf(strrr, L"%d", i - 1);
				else if (i <= 19)
					swprintf(strrr, L"*和%d", i - 11);
				else
					swprintf(strrr, L"**和%d", i - 21);
				if (i == 0)
					swprintf(strrr, L".");

				TEXTOUT(strrr, UIleft + 2 + 28 - textwidth(strrr) / 2, UItop + 30 + (Cur + i) * 30, 5, L"等线", 200, 120, 90);
				swprintf(strrr, L"%d", i - 1);
				TEXTOUT(strrr, UIleft + 2 + 64 + 16 - textwidth(strrr) / 2, UItop + 30 + (Cur + i) * 30, 5, L"等线", 70, 190, 160);
				TEXTOUT(PointsStr[i], UIleft + 2 + 112 + 16 - textwidth(PointsStr[i]) / 2, UItop + 30 + (Cur + i) * 30, 5, L"等线", 80, 80, 255);

				if (PlayersData[Play + 10][i] < 0)
					swprintf(strrr, L"无限");
				else
					swprintf(strrr, L"%.0f", PlayersData[Play + 10][i]);
				TEXTOUT(strrr, UIleft + 2 + 160 + 16 - textwidth(strrr) / 2, UItop + 30 + (Cur + i) * 30, 5, L"等线", 180, 30, 255);
				TEXTOUT(SkillNames[i], UIleft + 2 + 208, UItop + 30 + (Cur + i) * 30, 5, L"等线", 60, 80, 230);
			}
			Prime_RECT(UIleft, UItop, UIleft + 310, UItop + 25);
			Prime_RECT(UIleft, UItop, UIleft + 310, UItop + 25, 255, 255, 255, 0, 0, 0);
			TEXTOUT(L"快捷键", UIleft + 2, UItop + 3, 5, L"等线", 200, 120, 90);
			TEXTOUT(L"顺序", UIleft + 2 + 64, UItop + 3, 5, L"等线", 70, 190, 160);
			TEXTOUT(L"点数", UIleft + 2 + 112, UItop + 3, 5, L"等线", 80, 80, 255);
			TEXTOUT(L"次数", UIleft + 2 + 160, UItop + 3, 5, L"等线", 180, 30, 255);
			TEXTOUT(L"名称", UIleft + 2 + 208, UItop + 3, 5, L"等线", 80, 80, 180);
			TEXTOUT(L"←点击以返回顶部", UIleft + 2 + 312, UItop + 3, 4);

			Prime_RECT(UIleft + 355 + 1, UItop + 55 + 1, -UIleft - 1 - 1, -UItop - 300 - 1);
			Prime_RECT(UIleft, UItop + 55, UIleft + 310, UItop + 85, 255, 255, 255, 255, 50, 150);

			int Rowww = 0;
			int Colll = 4;
			BOOL Retract = 0;
			Color colorr;
			colorr.red = 0;
			colorr.green = 0;
			colorr.blue = 0;

			for (int i = 0; SkillDes[-Cur + 1][i] != '\0'; i++) {

				switch (SkillDes[-Cur + 1][i])
				{
				case 'r':
					colorr.red = 255;
					colorr.green = 0;
					colorr.blue = 0;
					i++;
					break;
				case 'b':
					colorr.red = 0;
					colorr.green = 0;
					colorr.blue = 255;
					i++;
					break;
				case 'o':
					colorr.red = 255;
					colorr.green = 150;
					colorr.blue = 100;
					i++;
					break;
				case 'p':
					colorr.red = 200;
					colorr.green = 50;
					colorr.blue = 255;
					i++;
					break;

				default:
					if ((int)SkillDes[-Cur + 1][i] < 0) {
						colorr.red = 0;
						colorr.green = 0;
						colorr.blue = 0;
					}
					break;
				}

				if ((int)SkillDes[-Cur + 1][i] < 0)
				{
					sprintf(strtemp, "%c%c", SkillDes[-Cur + 1][i], SkillDes[-Cur + 1][i + 1]);
					i++;

					if (Retract == 0)
						Colll += 2;
					else
						Colll++;
					Retract = 0;
				}
				else {
					if (Retract == 0)
						Colll += 2;
					else
						Colll++;
					Retract = 1;
					sprintf(strtemp, "%c", SkillDes[-Cur + 1][i]);
				}

				if (-UIleft - 20 <= UIleft + 360 + Colll * 10)
				{
					Colll = 0;
					Rowww++;
				}

				TEXTOUT(PATHcTow(strtemp), UIleft + 360 + Colll * 10, UItop + 60 + Rowww * 30, 4, L"幼圆", colorr.red, colorr.green, colorr.blue);


			}

			CurTemp = Cur;
		}

		swprintf(strrr, L"%d|%d ", -AutoCur, -Cur);
		TEXTOUT(strrr, UIleft + 256, UItop + 8, 3, L"幼圆", 120, 120, 120, 255, 255, 255);
	}

	Sleep(50);
	LoadMap();
	drawUI();
	DrawMan();

	if (lopoppop == -2)
		CharWindows(-Cur);

	if (lopoppop == -1) return Cur;
	else return -99;
}

//应部分要求还原绘画炮弹
void drawshell(int pat, int x, int y, LPCTSTR Put = L"炮", int size = 14)
{
	if (pat == 1)
	{
		settextstyle(size + 2, 0, L"等线");
		settextcolor(RGB(255, 255, 255));
		setbkmode(OPAQUE);
		setbkcolor(RGB(255, 255, 255));
		outtextxy(x, y, Put);
	}
	if (pat == 2)
	{
		setbkmode(TRANSPARENT);
		settextstyle(size+2, 0, L"等线");
		settextcolor(RGB(150, 150, 150));
		outtextxy(x - 1, y - 1.5, Put);

		settextstyle(size, 0, L"等线");
		settextcolor(RGB(0, 0, 0));
		outtextxy(x, y, Put);
	}
}

//保存大数据到文件
int SaveData(int Storge, int ROWS, int COLS, int rounds)
{
	char str[1024]{ 0 };
	int ReRe = 0;  //Re:Re:

	//存储至文件中
	while (Storge != 0)
	{
		if (rounds == 1)
			Datafile = fopen(Data, "w+");
		else
			Datafile = fopen(Data, "a");

		if (Datafile == NULL)
		{
			int FeedBack = MessageBox(NULL, L"存储数据失败！\n取消：剩下对局不存储任何数据\n重试：重新尝试存储(再次失败可换位置存储)\n继续：跳过本次存储", L"CharWar(程序错误)", MB_CANCELTRYCONTINUE | MB_ICONWARNING | MB_SYSTEMMODAL);
			if (FeedBack == IDCANCEL) return 0;
			if (FeedBack == IDTRYAGAIN)
			{
				ReRe++;
				if (ReRe == 2) {
					TCHAR* DataTemp = PATHcTow(Data);
					FileDialog(DataTemp, 1, L"ChalloWar～(∠・ω< )⌒★");
					SaveName = getFileName(DataTemp);
					sprintf(Data, "%s", TCHAR_to_char(DataTemp));
					Storge = 2;
				}
			}
			Sleep(100);
		}
		else
		{
			fprintf(Datafile, "\n%d->\n", rounds);

			MinesArry[0] = Mines;
			//保存地雷数据
			for (int i = 0; i < sizeof(MinesArry) / sizeof(MinesArry[0]); i++) {
				if (floNum(MinesArry[i]) == 0)
					fprintf(Datafile, "%.0f\t", MinesArry[i]);
				else
					fprintf(Datafile, "%f\t", MinesArry[i]);
			}
			fprintf(Datafile, "\n");


			// 将二维数组转换为字符并写入文件
			for (int i = 1; i < ROWS; i++) {
				if (i == Players + 1)
					i = 10;
				if (i == 10 + Players + 1)
					i = 21;
				if (i == 21 && PlayersData[10][14] <= 0)
					break;
				for (int j = 1; j < COLS; j++) {
					if (floNum(PlayersData[i][j]) == 0)
						fprintf(Datafile, "%.0f", PlayersData[i][j]);
					else
						fprintf(Datafile, "%f", PlayersData[i][j]);
					if (j != COLS - 1)
						fprintf(Datafile, "\t");
				}
				fprintf(Datafile, "\n");
			}

			fprintf(Datafile, ">>");
			fclose(Datafile);
			return Storge;
		}
	}

}

//保存操作
int SaveMove(int ReKey, int Dirr = 0)
{
	Datafile = fopen(Data, "a");
	if (Datafile == NULL)
		return -1;

	float Ppprint = ReKey;

	if (ReKey == 'A' || ReKey == 'D')
		Ppprint = PlayersData[Play][1];
	if (ReKey == '+' || ReKey == '-')
		Ppprint = v0;
	if (ReKey == 'W' || ReKey == 'S')
		Ppprint = Sigma;
	if (Dirr != 0)
		Ppprint = Dirr;

	if (floNum(Ppprint) == 0)
		fprintf(Datafile, "%c:%.0f,", (char)ReKey, Ppprint);
	else
		fprintf(Datafile, "%c:%f,", (char)ReKey, Ppprint);
	fclose(Datafile);
}

//保存X，Y数据，嫌一坨直接堆在这里了
int SavXY(float X, float Y)
{
	if (View == 0 && Storge > 0 && Auto == 0) {
		Datafile = fopen(Data, "a");
		if (Datafile == NULL) {
			Storge = -1;
			return -1;
		}
		if (Storge > 0) {
			fprintf(Datafile, "!:%f,%f,", X, Y);
			fclose(Datafile);
		}
	}

}

//进入联机模式
void Onl(int Choise = -1)
{
	TCHAR str[50] = {};


	//联机驶入对战准备
	while (OnlOrLoc == 1 && Choise <= 0 && View == -5)
	{
		setorigin(0, 0);
		Prime_RECT(5, 5, 55, 35);
		View = 0;
		char IncharTemp[50] = "127.0.0.1";
		LPTSTR Inchar = PATHcTow(IncharTemp);
		char strtemp[20] = {};
		BOOL Kakunin = FALSE;

		Kakunin = InputBox(Inchar, 16, L"输入服务器IP", L"联机模式", L"127.0.0.1", 0, 0, false);
		if (Kakunin == FALSE) { CloseAndMenu(); break; }
		swprintf(str, L"%s", L"服务器IP：");
		TEXTOUT(str, 100, 160);
		TEXTOUT(Inchar, 100 + textwidth(str), 160, 3, L"幼圆", 255, 100, 20);
		swprintf(PATHcTow(strtemp), L"%s", Inchar);

		Kakunin = InputBox(Inchar, 6, L"输入服务器端口", L"联机模式", L"9123", 0, 0, false);
		if (Kakunin == FALSE) { CloseAndMenu(); break; }
		swprintf(str, L"%s", L"服务器端口：");
		TEXTOUT(str, 100, 180);
		TEXTOUT(Inchar, 100 + textwidth(str), 180, 3, L"幼圆", 255, 100, 20);
		port = atoi(TCHAR_to_char(Inchar));

		Kakunin = InputBox(Inchar, 2, L"输入总人数\n提示：只需要第一个人输入即可\n默认为2个", L"联机模式", L"2", 0, 0, false);
		if (Kakunin == FALSE) { CloseAndMenu(); break; }
		swprintf(str, L"%s", L"总人数：");
		TEXTOUT(str, 100, 200);
		TEXTOUT(Inchar, 100 + textwidth(str), 200, 3, L"幼圆", 255, 100, 20);
		Players = atoi(TCHAR_to_char(Inchar));
		sever_ip = strtemp;



		/*
		TEXTOUT(L"(按F1为粘贴), Esc退出，Enter下一个选项", 164, 130, 3, L"幼圆", 255, 100, 20);
		strncpy_s(Inchar, INPUT_char(100, 160, "联机游戏", "房主服务器IP："), 15);
		if (GetAsyncKeyState(VK_ESCAPE)) { CloseAndMenu(); break; }

		/*
		Datafile = fopen(Data, "r+");
		if (Datafile == NULL) {
			MessageBox(NULL, "找不到PlayerData.txt文件！", "CharWar", MB_OK);
			View = -1; OnlOrLoc = 0; break;
		}

		fseek(Datafile, 0, SEEK_SET);//文件头
		while (fgets(textrow, sizeof(textrow), Datafile)) {
			if (strstr(textrow, "#InputIP:") != NULL) {
				char* IP_pos = strchr(line, ':') + 1;
				fseek(Datafile, IP_pos - line, SEEK_CUR); // 将文件指针移动到WINS的值之后
				fprintf(Datafile, " %s\n", Inchar); // 写入新的WINS值
				break;
			}
		}*\/

		sever_ip = Inchar;
		port = atof(INPUT_char(100, 190, "NULL", "房主服务器端口：", 5));
		if (GetAsyncKeyState(VK_ESCAPE)) { CloseAndMenu(); break; }

		/*
		fseek(file, 0, SEEK_SET);
		while (fgets(textrow, sizeof(textrow), file)) {
			if (strstr(textrow, "#InputPortMain:") != NULL) {
				fprintf(tempFile, "#InputPortMain:%d\n", port);
			}
			else {
				fprintf(tempFile, "%s", textrow);
			}
		}

		fclose(file);	fclose(tempFile);
		remove(Data);	rename(tempFilename, Data);
		*\/

		Players = atof(INPUT_char(100, 220, "NULL", "游玩人数：", 1));
		if (GetAsyncKeyState(VK_ESCAPE)) { CloseAndMenu(); break; }


		/*
		sever_ip = INPUT_char();
		outtextxy(50, 350, sever_ip);
		Sleep(500);
		*/



		//1 确定协议版本
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		{
			TEXTOUT(L"确定协议版本失败~", 100, 280, 4, L"等线");
			//printf("确定协议版本失败!\n");
			CloseAndMenu(); Sleep(150);
			break;
		}
		else {
			TEXTOUT(L"确定协议版本成功~", 100, 280, 4, L"等线");
			//printf("确定协议版本成功!\n");
		}
		//2 创建socket
		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (SOCKET_ERROR == serverSocket)
		{
			TEXTOUT(L"创建socket失败~", 100, 300, 4, L"等线");
			outtextxy(300, 300, swprintf(str, L"%d", GetLastError()));
			//printf("创建socket失败:%d\n", GetLastError());
			CloseAndMenu(); Sleep(150);
			break;
		}
		else {
			TEXTOUT(L"创建socket成功~", 100, 300, 4, L"等线");
			//printf("创建socket成功!\n");
		}

		//3 获取服务器协议地址簇
		SOCKADDR_IN addr = { 0 };
		addr.sin_family = AF_INET;//协议地址族
		addr.sin_addr.S_un.S_addr = inet_addr(sever_ip);//这里填写服务器IP地址
		addr.sin_port = htons(port);//服务器的端口


		//4 连接服务器
		ret = connect(serverSocket, (sockaddr*)&addr, sizeof addr);
		if (ret == -1)
		{
			TEXTOUT(L"连接服务器失败~", 100, 320, 4, L"等线");
			//printf("连接服务器失败:%d\n", GetLastError());
			CloseAndMenu();  Sleep(1500);
			break;
		}
		else {
			TEXTOUT(L"连接服务器成功~", 100, 320, 4, L"等线");
			//printf("连接服务器成功!\n");
		}

		int Allplayers = 2;
		int NowPlayers = 0;
		int ValPlayers = 0;

		TEXTOUT(L"正在等待玩家~", getwidth() - 200, 150, 4, L"等线", 200, 50, 255);

		// int select(int n, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
		while (View == 0 && OnlOrLoc == 1)
		{
			swprintf(PATHcTow(RecvBuffer), L"%d", Players);	send(serverSocket, RecvBuffer, 255, NULL);  //发送总人数

			ret = recv(serverSocket, RecvBuffer, 255, NULL);//获取总玩家数
			if (ret <= 0) { CloseAndMenu(); break; }	//服务器掉线就返回主菜单
			RecvBuffer[ret] = '\0';
			if (RecvBuffer[1] == '=') {  //Players
				Allplayers = atoi(RecvBuffer);
				swprintf(str, L"%d", Allplayers);
				Prime_RECT(getwidth() - 100, 180, getwidth() - 70, 200);
				TEXTOUT(str, getwidth() - 100, 180, 7, L"等线", 200, 50, 255);
				outtextxy(getwidth() - 150, 180, L"/");
			}

			ret = recv(serverSocket, RecvBuffer, 255, NULL);//获取现在玩家数
			if (ret <= 0) { CloseAndMenu(); break; }	//服务器掉线就返回主菜单
			RecvBuffer[ret] = '\0';
			if (RecvBuffer[1] == '=') {  //Allconnet
				NowPlayers = atoi(RecvBuffer);
				swprintf(str, L"%d", NowPlayers);
				Prime_RECT(getwidth() - 200, 180, getwidth() - 170, 200);
				TEXTOUT(str, getwidth() - 200, 180, 7, L"等线", 200, 50, 255);
			}

			if (Allplayers == NowPlayers) {
				Players = Allplayers;	View = 0;
				while (1) {
					ret = recv(serverSocket, RecvBuffer, 255, NULL);//获取己方回合
					if (ret == 0) { CloseAndMenu(); break; }  //服务器掉线就掉线
					RecvBuffer[ret] = '\0';
					if (RecvBuffer[2] == 'R') break;
				}
				selfround = atoi(RecvBuffer);

				break;
			}
			if (GetAsyncKeyState(VK_ESCAPE)) { CloseAndMenu(); break; }
		}//wait for players

		setorigin(0, 0);
		swprintf(str, L"%d", selfround);

		TEXTOUT(L"你是   号位", getwidth() - 200, 210, 4, L"等线", 200, 50, 255);
		outtextxy(getwidth() - 200 + textwidth(L"你是 "), 210, str);

		if (selfround > 1)
		{
			TEXTOUT(L"等待房主选择地图", getwidth() - 200, 230, 4, L"等线", 50, 200, 255);

			while (1)
			{
				ret = recv(serverSocket, RecvBuffer, 255, NULL);	RecvBuffer[ret] = '\0';
				if (ret <= 0) { CloseAndMenu(); break; }

				if (strcmp(RecvBuffer, "You are alive?") == 0) { sprintf(RecvBuffer, "%s", "alive");  send(serverSocket, RecvBuffer, 255, NULL); }
				if (strcmp(RecvBuffer, "Start") == 0) {
					setorigin(map_wide / 2, map_high / 2);   //set world center  世界以我为中心
					Choise = Players;
					break;
				}
				if (strcmp(RecvBuffer, "GoodBye") == 0) { CloseAndMenu(); break; }  //芜湖，直接爆掉吧
				if (strcmp(RecvBuffer, "Char!map") == 0) {

					ret = recv(serverSocket, RecvBuffer, 255, NULL);	RecvBuffer[ret] = '\0';

					sprintf(PATH, "%s", RecvBuffer);
					LoadMap();
					swprintf(PATHcTow(PATH), L"%s", PATHcTow(PATH));
					saveimage(PATHcTow(PATH));

				}
				if (strcmp(RecvBuffer, "Char!Ranmap") == 0)
				{
					recvmap();
				}
			}
		}//自己不是房主
		else//自己是房主
		{
			setorigin(0, 0);
			TEXTOUT(L"M键选择本地地图", getwidth() - 200, 230, 4, L"等线", 50, 200, 255);
			TEXTOUT(L"TAB随机地图", getwidth() - 200, 250, 4, L"等线", 50, 200, 255);

			while (1)
			{
				//开始游戏
				if (GetAsyncKeyState(VK_SPACE)) {
					sprintf(RecvBuffer, "%s", "Start");	send(serverSocket, RecvBuffer, 255, NULL);  //要开始了唷
					setorigin(map_wide / 2, map_high / 2);   //set world center  世界以我为中心
					Choise = Players;
					break;
				}

				//本地地图
				if (GetAsyncKeyState('M'))      //Enter to change map
				{
					sprintf(RecvBuffer, "%s", "Char!map");	send(serverSocket, RecvBuffer, 255, NULL);
					//FileDialog(PATH, 0);
					Sleep(100);
					LoadMap();

					sprintf(RecvBuffer, "%s", PATH);	send(serverSocket, RecvBuffer, strlen(RecvBuffer), NULL);
					sprintf(PATH, "%s", PATH);
					saveimage(PATHcTow(PATH));

					DrawTitle();

					system("pause");

					//sendmap();
					//直接转发本地文件，不用上传服务器了
				}

				//随机地图
				while (GetAsyncKeyState(VK_TAB))
				{
					sprintf(RecvBuffer, "%s", "Char!Ranmap");	send(serverSocket, RecvBuffer, 255, NULL);
					Ranmap();
					if (sendmap() == FALSE) break;

					Sleep(200);
				}

				//退出游戏
				if (GetAsyncKeyState(VK_ESCAPE))   //Esc to Esc
				{
					sprintf(RecvBuffer, "%s", "Goodbye");	send(serverSocket, RecvBuffer, 255, NULL);
					CloseAndMenu(); break;
				}//爆掉吧

				/*/聊天
				if (GetAsyncKeyState('T'))
				{
					~CharChat;
					strncpy_s(Inchar, INPUT_char(100, 160, "NULL", "你说："), 50);
					swprintf(PATHcTow(RecvBuffer), L"%s", "CharChat1");	send(serverSocket, RecvBuffer, 255, NULL);
					swprintf(PATHcTow(RecvBuffer), L"%s", Inchar);	send(serverSocket, RecvBuffer, 255, NULL);

				}
				*/

				Sleep(200);
			}//等待开始游戏

		}//自己是房主

	} //OnlOrLoc == 1

}

//绘图程序
void draw()
{
	POINT mouse;
	int X, Y;

	Prime_RECT();
	//setfillcolor(RGB(255, 10, 10));
	//fillcircle(150, 150, 10);
	while (1) {

		GetCursorPos(&mouse);
		ScreenToClient(GetHWnd(), &mouse);
		X = mouse.x;  Y = mouse.y;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
		{
			setlinecolor(RGB(255, 10, 10));
			setfillcolor(RGB(255, 10, 10));
			fillcircle(X, Y, 15);
		}


		Sleep(1);
	}

	system("pause");
}

//保存地图并复制到SaveMap
void SaveAnSave()
{
	saveimage(PATHcTow(PATH));
	if (Storge > 0 && View == 0) {
		char PathTemp[1024];
		sprintf(PathTemp, "%s\\SaveMap\\%s.png", AllPath, SaveName);
		CopyFile(PATHcTow(PATH), PATHcTow(PathTemp), FALSE);
	}
}
