#include <CharWar.h>

HWND hWndForeground = GetForegroundWindow();
HWND hWndMain = FindWindow(NULL, L"CharWar");

int main()
{
	char str[50] = {};
	int Choise = 0;
	srand((unsigned)time(NULL));

	initgraph(map_wide, map_high);

	getcwd(AllPath, 1024);
	sprintf(PATH, "%s\\CharWar.png", AllPath);
	sprintf(Data, "%s\\SaveGame\\Save.txt", AllPath);

	sprintf(PATH, "%s\\CharWar.png", AllPath);
	SaveAnSave();


	/*  全屏化，但是还没找到优化算法，因为地图大小可能会超大(14Mb)
	HWND hwnd = GetHWnd();
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) - WS_CAPTION);
	SetWindowPos(hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CXSCREEN), SWP_SHOWWINDOW);
	*/

	//draw();

	ViewBack:

	while ((View != 0 && OnlOrLoc == 0) || Choise >= 0) {
		drawUI();
		playchar();
		if (OnlOrLoc == 0 && View == 0)  //单机准备开始界面
		{
			CHOOSE(Choise, L"要开始了唷", L"幼圆", L"双人模式", L"三人模式", L"单刷人机", L"调试模式");
			if (Choise <= 0)
				View = -1;
			else {
				Ranmap();

				ExMessage m;

				while (1)   // Enter to Enter
				{
					m = getmessage(EX_KEY);

					//开始游戏
					if (m.vkcode == VK_SPACE) {
						break;
					}

					//随机地图
					if (m.vkcode == VK_TAB)      //Tab to change map
					{
						Ranmap();
						//Sleep(150);
					}
					
					//指定地图
					if (m.vkcode == 'M')
					{
						TCHAR* Ptemp = PATHcTow(PATH);
						FileDialog(Ptemp, 0, L"打开地图");
						//sprintf(PATH, "%s", TCHAR_to_char(Ptemp));
						CopyFile(Ptemp, PATHcTow(PATH), FALSE);
						LoadMap();

						sprintf(RecvBuffer, "%s", TCHAR_to_char(Ptemp));
						//sprintf(PATH, L"%s", PATH);
						SaveAnSave();
					}

					//回放存档
					if (m.vkcode == 'V') {
						Auto = -2;
						break;
					}

					//读取存档
					if (m.vkcode == 'L') {
						Auto = -1;
						break;
					}


					//退出至主菜单
					if (m.vkcode == VK_ESCAPE)   //Esc to Esc
					{
						View = -1;
						break;
					}

					//Others such as save map & load map, draw map, save data(txt) & load data
				}

				switch (Choise)
				{
				case 1:
					Players = 2;
					break;
				case 2:
					Players = 3;
					break;


				default:
					Players = 3;
					break;
				}

				selfround = 1;
				break;
			}
		}

		//联机驶入对战准备
		Onl(Choise);

	}//main menu  looop

	playchar();
	if (View != 0)
		goto ViewBack;

	//6 关闭socket
	closesocket(serverSocket);
	//7 清理协议版本信息
	WSACleanup();

	return 0;
}

void playchar() {

	//清空数组
	for (int i = 0; i < 22; ++i)
		for (int j = 0; j < 24; ++j)
			PlayersData[i][j] = 0;

	//字符组洗牌
	for (size_t i = 8; i > 0; i--) {
		size_t j = rand() % (i + 1);
		const TCHAR* temp = PlayerChar[i];
		PlayerChar[i] = PlayerChar[j];
		PlayerChar[j] = temp;
	}

	BOOL KeyReal[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	int  Dir = 1, Direct = 1, key = 0, Hittt = 0, Shields = 0, STimes = 0, FeedKey = 0;						//-1结束游戏，攻击方向，决定刷新，风向，自己的回合
	int LoadRud = 3; //读取的回合
	float xp, yp, vx, vy, tp = 0, jumpt, x0 = 0, y0 = 0, mx, my, X, Y, a = 0;							//初始力度，炮弹x，炮弹y，炮弹水平速度，竖直速度，抛出时间，发射初始x，初始y，风速a
	BOOL JUmp = FALSE, VxVy = TRUE, SuaStop = TRUE;
	float UIleft = -map_wide / 2.0, UItop = -map_high / 2.0;

	const int ROWS = 21 + 1;		 //一般不会更改
	const int COLS = 23 + 1;		 //与44行同步

	double PrDistce[10]{ 0 }; //d1, d2, d3...
	float DataTemp[3][24] = { 0 };

	//float PlayersData[10][9] = 9999, PlayersData[10][10] = 9999, d9 = 9999; int PlayersData[10][8] = 3, PlayersData[10][7] = 0;								//包x，包y，炮距包距离，回合出现包，包内容

	float Inj = 125, AllInj = 0, r = 0, AllHit[11]{ 0 };												//伤害，忘了，忘了，伤害半径   好像是用于非水平的风
	int Round = 1, Bomb = 1, Cost = 0, Skill = 0, SkillKey = -99, SkillGroup = 0, Rep = 0, Repeat = 0, times = 0, Comp = 0;		//回合，决定炮弹是否有，技能花费，哪个技能，玩家数，决定某些循环技能，决定炮弹循环，某些技能次数，电脑玩家存在
	double FireTemp[] = { Skill, Inj, r, times, x0, y0, tp, Dir };  //Skill, Inj, r, times, x, y, tp, Dir
	unsigned int RunTime = 0;
	POINT mouse;
	HANDLE hRecvThread = nullptr;
	ExMessage m;
	int LMouseWasPressed = 0;
	char str[1024];    //各种临时转换

	const int NUM_COLORS = 10;//反正最多9个人
	Color colors[NUM_COLORS];

	if (Auto >= 0) {
		//生成并存储颜色
		for (int i = 0; i < NUM_COLORS; ++i) {
			Color newColor;
			//生成随机颜色，直到找到与已有颜色不相近的颜色为止
			do {
				newColor = generateRandomColor();
			} while (isColorNear(newColor, colors, i));
			colors[i] = newColor;
		}

		for (int i = 1; i < NUM_COLORS; ++i) {
			PlayersData[i][3] = colors[i].red;
			PlayersData[i][4] = colors[i].green;
			PlayersData[i][5] = colors[i].blue;
		}

		if (3 <= Players && Players <= 4 && RANDOM(0, 9) >= 8.8)
		{
			for (int i = 1; i <= 4; i++)
			{
				PlayersData[i][3] = 0;
				PlayersData[i][4] = 0;
				PlayersData[i][5] = 0;
			}
			PlayersData[1][5] = 255;
			PlayersData[2][3] = 255;
			PlayersData[2][4] = 150;
			PlayersData[3][3] = 255;
			PlayersData[4][3] = 255;
			PlayersData[4][4] = 100;
			PlayersData[4][5] = 200;
			PlayerChar[0] = L"凉";
			PlayerChar[1] = L"虹";
			PlayerChar[2] = L"喜";
			PlayerChar[3] = L"波";
		}

		//必要初始定义
		{
			Play = 1;
			v0 = 10; Sigma = 45;					//力度:  20~100	角度:  -60~100
			Dir = 1;								//攻击方向-1/1
			a = 0;								//风速-4~4
			x0 = 0; y0 = 9999;
			PlayersData[10][1] = Round;
			PlayersData[10][2] = Play;
			PlayersData[10][3] = Players;
			PlayersData[10][4] = OnlOrLoc;
			PlayersData[10][5] = a;
			PlayersData[10][6] = Direct;
			PlayersData[10][8] = (int)RANDOM(3, 5);  //第一个包出现回合
			PlayersData[10][9] = 9999; PlayersData[10][10] = 9999;
		}

		//初始化颜色，位置
		if (OnlOrLoc == 0)	//与非联机初始化
		{
			selfround = 1;
			if (View != 0)
				Players = 3;

			X = getwidth() / 2 - 30;
			for (int i = 1; i <= Players; i++)
			{
				PlayersData[i][1] = RANDOM(-X, X);
				PlayersData[i][2] = -map_high / 2.0 - 20;
			}
		}
		else {
			for (int i = 1; i <= Players; i++)
				PlayersData[i][2] = -map_high / 2.0 - 20;

			//发送和接收初始值
			if (selfround == 1)
			{
				X = getwidth() / 2 - 30;
				for (int i = 1; i <= Players; i++)
					PlayersData[i][1] = RANDOM(-X, X);


				char output[200];
				output[0] = '\0';

				//组合发送各个X
				Sleep(20);
				for (int i = 0; i < Players; i++) {
					// 将浮点数格式化为字符串，追加到输出字符串末尾
					sprintf(output + strlen(output), "%.3f", PlayersData[i + 1][1]);

					if (i < Players - 1) {
						strcat(output, ", ");
					}
				}
				sprintf(RecvBuffer, "%s", output);	send(serverSocket, RecvBuffer, 255, NULL);

				Sleep(20);
				for (int i = 0; i < Players; i++) output[0] = '\0';//清空
				//组合发送颜色
				for (int i = 0; i < Players; i++) {
					sprintf(output + strlen(output), "%d, %d, %d", (int)PlayersData[i + 1][3], (int)PlayersData[i + 1][4], (int)PlayersData[i + 1][5]);

					if (i < Players - 1) {
						strcat(output, ", ");
					}
				}
				sprintf(RecvBuffer, "%s", output);	send(serverSocket, RecvBuffer, 255, NULL);

			}
			else {

				const char delimiter[] = ", L";
				char* token; //存储每个分隔的字符串
				double recv_array[10] = {};

				//接收X坐标
				ret = recv(serverSocket, RecvBuffer, 255, NULL);	RecvBuffer[ret] = '\0';
				token = strtok(RecvBuffer, delimiter);
				int i = 1;
				while (token != NULL) {
					recv_array[i++] = strtod(token, NULL);
					token = strtok(NULL, delimiter);
				}
				for (i = 1; i <= Players; i++)
					PlayersData[i][1] = recv_array[i];

				//接收颜色
				ret = recv(serverSocket, RecvBuffer, 255, NULL);	RecvBuffer[ret] = '\0';
				token = strtok(RecvBuffer, delimiter);
				i = 1;
				while (token != NULL) {
					recv_array[i++] = strtod(token, NULL);
					token = strtok(NULL, delimiter);
				}
				for (i = 0; i <= Players; i++) {
					PlayersData[i + 1][3] = recv_array[3 * i + 1];
					PlayersData[i + 1][4] = recv_array[3 * i + 2];
					PlayersData[i + 1][5] = recv_array[3 * i + 3];
				}

			}

			/*	//设置recv为非阻断模式
			u_long mode = 1; // 1 表示非阻塞模式，0 表示阻塞模式
			int result = ioctlsocket(serverSocket, FIONBIO, &mode);
			if (result != NO_ERROR) {
				CloseAndMenu(); Play = -1;
			}*/


			/*  //创造接收消息线程
			hRecvThread = CreateThread(NULL, 0, RecvMsg, NULL, 0, NULL);
			if (hRecvThread == NULL) {
				CloseAndMenu(); Play = -1;
			}*/
		}

		//各固定数据初始化
		for (int i = 1; i <= Players; i++) {
			PlayersData[i][6] = 60;		//初始化生命值
			PlayersData[i][7] = 15;		//初始化生命值
			PlayersData[i][8] = 332;		//初始化生命值
			PlayersData[i][9] = 4800;	//初始化生命值
		}PlayersData[1][8]++;//点数自加1

		int SkillRow[] = { 3,-1,-1,-1,-1,2,1,2,1,6,1,4,-1,2,-1,1,2,3,3,4,2,4,3,4 };
		if (View != 0) {
			for (int i = 0; i < COLS; i++) {
				SkillRow[i] = -1;
			}
			for (int i = 1; i <= Players; i++)
				PlayersData[i][8] = 0721;
		}

		for (int i = 0; i < 9; i++) // 总共10人
			for (int j = 0; j < COLS; j++)
				PlayersData[11 + i][j] = (float)SkillRow[j];

		//非对战界面避免画UI，与地雷初始化
		if (View != 0) {
			x0 = PlayersData[1][1] = -40;
			setorigin(map_wide / 2, map_high / 2);
		}	//其他界面
		else
		{
			LoadMap();

			//地雷数据初始化
			for (int i = 1; i <= Mines; i++)
			{
				MinesArry[3 * i - 2] = 1;
				MinesArry[3 * i - 1] = RANDOM(-map_wide / 2.0 + 20, map_wide / 2.0 - 20);
				MinesArry[3 * i] = -map_high / 2.0 - 20;
			}

			TCHAR* Datatemp = PATHcTow(Data);
			int Feed = FileDialog(Datatemp, 1, L"ChalloWar 一般存放在SaveGame里", L"存档.txt\0*.txt\0只能选上面这些类型\0*.txt\0要不然会爆炸的唷\0*.txt\0\0");

			if (Feed == 1)
				Storge = SaveData(Storge, ROWS, COLS, Round);
			else
				Storge = 0;
			if (Storge == 1)
			{
				sprintf(Data, "%s", TCHAR_to_char(Datatemp));

				SaveData(Storge, ROWS, COLS, Round);
				SaveName = getFileName(Datatemp);

				char PathTemp[1024];
				SaveAnSave();
				sprintf(PathTemp, "%s\\Replay\\%s.png", AllPath, SaveName);
				CopyFile(PATHcTow(PATH), PATHcTow(PathTemp), FALSE);
			}

			//强制掉掉掉
			for (int i = 1; i <= Players; i++)
				if (PlayersData[i][9] > 0)
					while (getpixel(PlayersData[i][1] + 2, PlayersData[i][2] + 16 + 2) != Block &&
						getpixel(PlayersData[i][1] + 16 - 2, PlayersData[i][2] + 16 + 2) != Block)
					{
						PlayersData[i][2] += 1;
						Prime_RECT(PlayersData[i][1], PlayersData[i][2], PlayersData[i][1] + 14, PlayersData[i][2] + 14);
						TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);
						if (PlayersData[i][2] >= map_high / 2.0)
						{
							PlayersData[i][1] = RANDOM(-map_wide / 2.0 - 30, map_wide / 2.0 - 30);
							PlayersData[i][2] = -map_high / 2.0 - 20;
							if (OnlOrLoc == 1) {
								sprintf(RecvBuffer, "%f玩家%d重生坐标", PlayersData[i][1], i);
								send(serverSocket, RecvBuffer, strlen(RecvBuffer), 0);
							}
						}
						delay(1);
					}

			//掉雷
			TEXTOUT(L"投放地雷中...正在: ", -map_wide / 2 + 10, -map_high / 2 + 20, 3, L"幼圆", 50, 50, 120, 255, 255, 255);
			for (int i = 1; i <= Mines * 3; i += 3)
			{
				sprintf(str, "%d", (i - 1) / 3 + 1);
				TEXTOUT(PATHcTow(str), -map_wide / 2 + 160, -map_high / 2 + 20, 3, L"幼圆", 50, 50, 120, 255, 255, 255);
				while (getpixel(MinesArry[i + 1] + 4, MinesArry[i + 2] + 14) != Block && getpixel(MinesArry[i + 1] + 8, MinesArry[i + 2] + 14) != Block)
				{
					MinesArry[i + 2] += 1.0;
					TEXTOUT(L"雷", MinesArry[i + 1], MinesArry[i + 2], 1, L"幼圆", 50, 30, 60, 255, 255, 255);
					if (MinesArry[i + 2] + 14 >= 300) { MinesArry[i] = 0; break; }
					delay(1);
				}

			}
			Prime_RECT(-map_wide / 2 + 10, -map_high / 2 + 20, -map_wide / 2 + 10 + 180, -map_high / 2 + 20 + 20);
			drawUI();
		}

	}

	//自动模式/读取存档
	while (Auto == -2 || Auto == -1)
	{
		TCHAR* Ptemp = PATHcTow(Data);
		FileDialog(Ptemp, 0, L"ChalloWar 请狗修金撒麻选择要打开的存档~", L"txt\0*.txt;\0只能选上面这些类型\0*.txt\0要不然会爆炸的唷\0*.txt\0\0");
		SaveName = getFileName(Ptemp);
		FILE* file = fopen(TCHAR_to_char(Ptemp), "r");
		if (file == NULL) {
			Auto = 0;
			View = -1;
			Play = -1;
			MessageBox(NULL, L"打开文件失败/取消", L"CharWar", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			break;
		}

		int row = 0, line = 0, LsRud = 0;  //矩阵行，文件行，
		int strbl = 0; //控制写入
		if (Auto == -1) {
			LoadRud = 0;
			strbl = -1;
		}

		LoadBack:
		while (fgets(str, sizeof(str), file) != NULL)
		{
			// 分割行并存入二维数组
			char* token = strtok(str, "\t\n");  // 按制表符和换行符分割
			int col = 0;
			line++;

			if (str[col] == '\n') //跳过空行
				continue;

			//判断段尾
			if (str[col] == '>' && str[col + 1] == '>') {
				if (strbl == 3)
					break;  //春促借宿
				if (strbl == -1) {
					strbl = -2; //捕捉到>>就下一行就是你
					continue;
				}
			}

			//判断段头  一坨石
			if (strbl == 0)
				if (atof(token) == LoadRud) {
					while (strbl == 0)
					{
						if (str[col] == '-' && str[col + 1] == '>')
						{
							char strtemp[512] = { 0 };

							itoa(LoadRud, strtemp, 10);
							for (int i = 0; i < col; i++)
								if (str[i] == strtemp[i])
									strbl = 1;
								else
								{
									strbl = 0;
									break;
								}

							//转到下一行
							break;
						}
						col++;
					}
				}
				else
					continue;
			
			if (strbl == -2) {
				LsRud = atof(token);
				strbl = -1;
			}
			if (strbl == 1)
			{
				strbl++;
				continue;
			}


			//收集MinesArray
			col = 0;
			while (token != NULL && strbl == 2)
			{
				MinesArry[col] = atof(token);
				col++;
				token = strtok(NULL, "\t\n");
				if (col > sizeof(MinesArry) / sizeof(MinesArry[0]))
					break;
			}

			//收集PlayerData
			if (strbl == 2)
				strbl++;
			col = 1;
			while (token != NULL && strbl == 3)
			{
				PlayersData[row][col] = atof(token);  // 转换为浮点数并存入数组
				col++;
				token = strtok(NULL, "\t\n");
				if (col > COLS)
					break;
			}

			row++;  // 进入下一行
			if (row == 4) //跳到第10行
				row = 10;
		}
		if (strbl == -2) {
			LoadRud = LsRud;
			strbl = 0;
			line = 0;
			row = 0;
			fseek(file, 0, SEEK_SET);
			goto LoadBack;
		}

		Round = PlayersData[10][1];
		selfround = Play = PlayersData[10][2];
		Players = PlayersData[10][3];
		OnlOrLoc = PlayersData[10][4];
		a = PlayersData[10][5];
		Direct = PlayersData[10][6];
		if (Direct != -1 && Direct != 1)
			Direct = 1;

		if (Round <= 0 || Players < 2){
			Auto = 0;
			View = -1;
			Play = -1;
			MessageBox(NULL, L"存档可能出错\n读取到不可能的值", L"CharWar", MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
			break;
		}

		sprintf(str, "%s\\SaveMap\\%s.png", AllPath, SaveName);
		CopyFile(PATHcTow(str), PATHcTow(PATH), FALSE);
		fclose(file);  // 关闭文件
		LoadMap();
		drawUI();
		break;
	}

	//对战主函数，主界面复用
	while (Play != -1)
	{
		RunTime++;
		Sleep(1);
		GetCursorPos(&mouse);
		ScreenToClient(GetHWnd(), &mouse);
		X = mouse.x;  Y = mouse.y;

		//时停代码
		hWndForeground = GetForegroundWindow();//比较前置窗口
		hWndMain = FindWindow(NULL, L"CharWar");

		//对战界面点击按钮
		if ((View == 0 && LMouseWasPressed == 1) || (GetAsyncKeyState(VK_TAB) & 0x8000))
		{
			//点击技能
			if ((getwidth() - 210 <= X && X <= getwidth() - 210 + 75 && 55 <= Y && Y <= 55 + 18) ||
				(GetAsyncKeyState(VK_TAB) & 0x8000))
				SkillKey = SkillMenu();

			if (-90 < SkillKey && SkillKey <= -20)
				SuaStop = FALSE;

			Sleep(100);
			LMouseWasPressed = 0;
		}

		if ((hWndForeground != hWndMain || RunTime >= 6000 || SuaStop == 0) && OnlOrLoc == 0) {
			//MessageBox(NULL, L"裤裆起火，男娘撅我", L"CharWar(程序挂起)", MB_OK | MB_ICONASTERISK);
			Slp();

			//system("pause");
			//TEXTOUT(L"程序已挂起(按键唤醒)", -100, -map_high / 2 + 60, 5, L"幼圆", 255, 255, 255);
			LoadMap();
			drawUI();
			DrawMan();
			RunTime = 0;
			SuaStop = TRUE;
		}

		//左鼠标按下松开与否
		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			if (!LMouseWasPressed)
				LMouseWasPressed = 1;
		}
		else {
			if (LMouseWasPressed)
				LMouseWasPressed = -1;
		}

		//其他界面
		if (View != 0 && LMouseWasPressed == -1 && hWndForeground == hWndMain)
		{
			RunTime = 0;

			//主菜单和开始游戏
			if ((View == -1 || View == -5) && LMouseWasPressed == -1) {

				//右边栏目
				if (getwidth() - 45 < X && X < getwidth() - 5)
				{
					Prime_RECT(0, 200, 100, 220);
					if (0 < Y && Y < 40)  View = -2;	//TEXTOUT(L"你点到了设置", 0, 200)
					if (45 < Y && Y < 85)  TEXTOUT(L"你点到了个人", 0, 200);
					if (90 < Y && Y < 130) TEXTOUT(L"你点到了抽卡", 0, 200);
					if (View != -1 && View != -5) { Play = -1; break; }
				}

				//中间部分
				if (getwidth() / 2 - 77 < X && X < getwidth() / 2 + 78)
				{
					Prime_RECT(0, 200, 100, 220);
					if (View == -1) {
						if (getheight() / 2 - 22 < Y && Y < getheight() / 2 + 23) { TEXTOUT(L"你点到了开始", 0, 200); Play = -1; View = -5; break; }
						if (getheight() / 2 + 68 < Y && Y < getheight() / 2 + 113)  exit(0);  //逃出游戏
					}

					if (View == -5)
					{
						if (getheight() / 2 - 67 < Y && Y < getheight() / 2 - 32) { Play = -1; View = 0; break; }  //单稽游戏  single
						if (getheight() / 2 + 23 < Y && Y < getheight() / 2 + 68) { Play = -1; OnlOrLoc = 1; break; }  //联稽游戏  online
					}
				}
			}

			//设置界面
			if (View == -2)
			{
				const TCHAR* setlists[] = { L"文件夹位置", L"音量大小", L"设置大小", L"按键设置", L"回合设置" };
				int listnums = sizeof(setlists) / sizeof(setlists[0]);
				int listPosi = 0;

				if (20 <= X && X <= 20 + textwidth(L"文件夹位置"))
					for (int i = 0; i < listnums; i++)
					{
						if (80 + 30 * i <= Y && Y <= 80 + 25 + 30 * i) listPosi = i + 1;
					}
				Prime_RECT(-200, 100, -130, 120, 255, 255, 254);
				sprintf(str, "%d", listPosi);
				TEXTOUT(PATHcTow(str), -180, 100);

			}

			//返回主菜单  main menu
			if (View != -1 && 5 < X && X < 55 && 5 < Y && Y < 30) { View = -1; Play = -1; break; }

			Prime_RECT(-200, 50, -130, 70, 255, 255, 254);
			sprintf(str, "%.0f", X);
			TEXTOUT(PATHcTow(str), -200, 50);
			sprintf(str, "%.0f", Y);
			TEXTOUT(PATHcTow(str), -160, 50);


			LMouseWasPressed = 0;
		}

		//按键
		if (selfround == Play) {

			//按住左键调整方向角度
			/*
			if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
					GetCursorPos(&mouse);
				ScreenToClient(GetHWnd(), &mouse);

				mx = mouse.x-map_wide/2.0;
				my = mouse.y-map_high/2.0;

				if (-61 <= Sigma && Sigma <= 81) {
					if (-60 >= Sigma) Sigma = -60;
					if (Sigma >= 80) Sigma = 80;

					setlinecolor(RGB(255, 255, 255));
					line(x0 + 8, y0, x0 + 8 + Dir * 10, y0 - tan(Sigma * pi / 180) * 10);

					Sigma = atan2(y0-my, mx-x0)*(180.0 / pi);

					setlinecolor(RGB(0, 0, 0));
					line(x0 + 8, y0, x0 + 8 + Dir * 10, y0 - tan(Sigma * pi / 180) * 10);
				}
			}
			*/

			//放开检测
			if (View == 0 && Storge > 0 && FeedKey != 0 && (GetAsyncKeyState(FeedKey) & 0x8000) == 0)
				if (FeedKey == Charkey[4])
					SaveMove(FeedKey, Dir);
				else
					SaveMove(FeedKey);

			FeedKey = KeyBack();
			if (View == 0 && Storge > 0 && FeedKey == '#')
				SaveMove(FeedKey);

			//刷新界面
			if (GetAsyncKeyState('R') && View != 0)
			{
				setorigin(0, 0); Sleep(100); drawUI();
				setorigin(map_wide / 2, map_high / 2);   //set world center  世界以我为中心

				DrawMan();
			}

			//重载
			if (keystr[0] != '!') RunTime = 0;

			//技能按键
			if (SkillKey == -99)
				SkillKey = GetSkill();
			else
				if (-22 - 50 <= SkillKey && SkillKey <= 1 - 50) {
					Skill = -SkillKey - 50;

				}

		}

		//联机模式下 接收消息   
		//阻断接收消息导致人卡在天上   尝试过开另一个线程来recv    但是会莫名其妙吞数据  详见348行RecvMsg函数线程
		//也尝试把坠落函数另开一个线程   bug更多(((
		//也尝试过把recv设置成非阻断模式   详见1067行	但是开炮后接收数组失败
		if (OnlOrLoc == 1 && selfround != Play)
		{
			ret = recv(serverSocket, RecvBuffer, 255, NULL);

			if (ret > 0) {
				//分割按键
				if (RecvBuffer[0] == '|')
					PartArray();
			}

		}

		//部分技能与事件回卷头部
	FireBack:

		FallDown();  //掉落判定

		//机关炮台 美丽的宝贝儿！ 来点炮声吧！
		if (数据[10][14] > 0 && 数据[10][17] == 0)
		{
			//见2365处各数据说明
			float GunsTData[21]{ 0 };   //现在相对位置和是否移动
			int MovePlayers = 0;
			float ShootX = 0, ShootY = 0;
			int Master = 数据[10][13];

			for (int i = 1; i <= Players; i++)
			{
				GunsTData[i] = TRUNCATE(fabs(数据[i][1] - 数据[i][2]), 2);

				//现在位置不等于初始位置且移动人非炮台主人
				if (fabs(GunsTData[i] - 数据[21][i]) >= 3 && i != Master)
				{
					GunsTData[10 + i] = 1;
					MovePlayers++;
				}
			}

			if (MovePlayers > 0)
			{

				int Who = (int)RANDOM(1, MovePlayers + 1);  //随机找一个移动的折磨
				int Pos = 0;

				TEXTOUT(L"枪", 数据[10][15] - 8, 数据[10][16] - 8, 3, L"幼圆", PlayersData[Master][3], PlayersData[Master][4], PlayersData[Master][5]);

				for (int i = 1; i <= 9; i++)
					if (GunsTData[10 + i] == 1)
					{
						Pos++;
						if (Who == Pos)
						{
							ShootX = PlayersData[i][1] + 16;
							ShootY = PlayersData[i][2] + 16;
						}
					}

				double Shootangle = atan2(ShootY - 数据[10][16] - 8, 数据[10][15] + 8 - ShootX) * (180 / pi);  //弧度制
				int ShootT = 0, ShootV = 10;
				float Spx = 数据[10][15], Spy = 数据[10][16], Svx = ShootV * cos(Shootangle * (pi / 180)), Svy = ShootV * sin(Shootangle * (pi / 180));
				float Injure = 35;
				BOOL Lopp = 1;

				while (Lopp)

				{
					ShootT += 1;

					Spx = 数据[10][15] - Svx * ShootT * 0.4;
					Spy = 数据[10][16] + Svy * ShootT * 0.4;

					for (int i = 1; i <= Players; i++)
						if (fabs(Spx - 数据[i][1] - 8) <= 15 && fabs(Spy - 数据[i][2] - 8) <= 15)
						{
							AllHit[i] += Injure;
							STimes++;
							ShootT = 0;

							if (PlayersData[i][10] >= Injure)
								PlayersData[i][10] = PlayersData[i][10] - Injure;
							else {
								PlayersData[i][9] += PlayersData[i][10] - Injure;
								PlayersData[i][10] = 0;
							}

							if ((AllHit[i] - (int)AllHit[i]) * 100 > 0) {
								if ((AllHit[i] - (int)AllHit[i]) * 10 > 0)
									sprintf(str, "%.1f", AllHit[i]);
								else
									sprintf(str, "%.2f", AllHit[i]);
							}
							else
								sprintf(str, "%.0f", AllHit[i]);

							TEXTOUT(PATHcTow(str), PlayersData[i][1] + 8 - textwidth(PATHcTow(str)) / 2.0, PlayersData[i][2] - 10, 2, L"华文新魏", 150, 50, 0, 255, 255, 255);

							Lopp = FALSE;
							break;
						}

					if (Spx <= UIleft - 20 || -UIleft + 20 <= Spx || Spy <= UItop - 20 || -UItop + 20 <= Spy)
					{
						ShootT = 0;
						STimes++;
						break;
					}

					if (getpixel(Spx, Spy) == Block ||     // 左上
						getpixel(Spx + 8, Spy) == Block ||     // 右上
						getpixel(Spx, Spy + 8) == Block ||     // 左下
						getpixel(Spx + 8, Spy + 8) == Block     // 右下
						)
					{
						STimes++;
						ShootT = 0;

						LoadMap();
						setbkcolor(RGB(255, 255, 255));
						clearcircle(Spx, Spy, 13);
						//circle(ShellmidX, ShellmidY, 10);

						if (View == 0)
						{
							SaveAnSave();
							drawUI();
						}

						break;
					}

					if (STimes == 6) {
						数据[10][17] = 1;
						STimes = 0;

						for (int i = 1; i <= Players; i++)
							if (PlayersData[i][9] > 0)
								TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);
						Lopp = 0;
						break;
					}

					TEXTOUT(L"炮", Spx - 8, Spy - 8, 0);

					delay(1);

					TEXTOUT(L"炮", Spx - 8, Spy - 8, 0, L"幼圆", 255, 255, 255);


				}

			}

		}

		//fire  Enter LMLB   保存还有很多未完成
		if (keystr[0] == '#' || keystr[1] == '#' || keystr[2] == '#' || keystr[3] == '#' || key == 30) //Enter
		{
			JUmp = FALSE;
			float Xx = X, Yy = Y;  //初始化部分技能需要的初始坐标
			int SkillRep = 0;
			tp = 0; //使炮弹初始时间为0，战姬复用此变量

			if (Skill != 0) {
				FireTemp[0] = Skill;
				FireTemp[7] = Dir;
			}

			//非自动模式操作及存储
			if (Auto >= 0 && SkillRep <= 1 && times == 0) {
				//炮台存储
				if (数据[10][14] > 0)	//枪血大于0
					for (int i = 1; i <= Players; i++)
						if (PlayersData[i][9] > 0)
							数据[21][i] = TRUNCATE(fabs(数据[i][1] - 数据[i][2]), 2);  //你动关我枪什么事
						else
							数据[21][i] = -1;

				//Data数组转存至存储数据至
				if (Round >= 2)
				{
					Round = PlayersData[10][1];
					Play = PlayersData[10][2];
					Players = PlayersData[10][3];
					OnlOrLoc = PlayersData[10][4];
					a = PlayersData[10][5];
					Direct = PlayersData[10][6];
				}

				//联机模式下同步数据
				if (OnlOrLoc == 1)
				{
					if (selfround == Play)
					{
						Sleep(80);

						ret = send(serverSocket, (char*)PlayersData, sizeof(PlayersData), 0);
						if (ret <= 0) {
							CloseAndMenu(); Play = -1; break;
						}

						Sleep(20);

						ret = send(serverSocket, (char*)MinesArry, sizeof(MinesArry), 0);
						if (ret <= 0) {
							CloseAndMenu(); Play = -1; break;
						}
					}
					else
					{	//接收图片仍然有问题
						//recvmap();

						ret = recv(serverSocket, (char*)PlayersData, sizeof(PlayersData), 0);
						ret = recv(serverSocket, (char*)MinesArry, sizeof(MinesArry), 0);

						//Sleep(400);

						//转换数组存储
						{
							Round = PlayersData[10][1];
							Play = PlayersData[10][2];
							Players = PlayersData[10][3];
							OnlOrLoc = PlayersData[10][4];
							a = PlayersData[10][5];
							Direct = PlayersData[10][6];
							Skill = PlayersData[10][11];
							Cost = PlayersData[10][12];
						}

					}
				}

				//保存XY
				if (View == 0 && Storge > 0) {
					Datafile = fopen(Data, "a");
					if (Datafile == NULL)
						Storge = -1;

					if (Storge > 0)
						fprintf(Datafile, "x:%f,y:%f,Skill:%d,",PlayersData[Play][1], PlayersData[Play][2], Skill);
					fclose(Datafile);
				}
			}

			//初始化x0,y0
			if (r != -4 && r != 6.1) {//排除一些指定炮弹位置
				y0 = PlayersData[Play][2];
				x0 = PlayersData[Play][1];
			}


		SkillBack:	//技能循环函数头部，需要指定位置时用到
			LoadMap();
			DrawMan();

			//返回至此以指定位置
			if (SkillRep <= -1 || (Skill == 21 && (SkillRep - 1) % 3 == 0 && Skill != 0))
			{
				Sleep(150);
				int DirKey = 0;

				if (Skill == 16 || Skill == 21)
				{
					X = 数据[Play][1] + 8 + cos(Sigma * (pi / 180)) * 70;
					Y = 数据[Play][2] + 8 - sin(Sigma * (pi / 180)) * 70;
				}

				while (key != 30)
				{
					GetCursorPos(&mouse);
					ScreenToClient(GetHWnd(), &mouse);

					mx = mouse.x - map_wide / 2.0;
					my = mouse.y - map_high / 2.0;

					//换方向
					if ((Skill == 4 || Skill == 16 || Skill == 21) &&
						GetAsyncKeyState(Charkey[4]))
					{
						Dir = -Dir;
						DirKey = 1;
						Sleep(100);
					}

					//判断是否移动鼠标
					if (Xx != mx || Yy != my ||
						(GetAsyncKeyState(VK_LBUTTON) & 0x8000) ||
						DirKey == 1 ||
						GetAsyncKeyState(Charkey[0]) || GetAsyncKeyState(Charkey[1]) || //AD
						GetAsyncKeyState(Charkey[2]) || GetAsyncKeyState(Charkey[3]))  //WS
					{
						if (DirKey == 1 ||
							GetAsyncKeyState(Charkey[0]) || GetAsyncKeyState(Charkey[1]) || //AD
							GetAsyncKeyState(Charkey[2]) || GetAsyncKeyState(Charkey[3]))  //WS
						{
							//指定式和环绕式
							if (Skill != 16 && Skill != 21) {
								if (GetAsyncKeyState(Charkey[0]))   X -= 1;
								if (GetAsyncKeyState(Charkey[1]))   X += 1;
								if (GetAsyncKeyState(Charkey[2]))   Y -= 1;
								if (GetAsyncKeyState(Charkey[3]))   Y += 1;
							}
							else
							{
								if (GetAsyncKeyState(Charkey[2]) && Y >= 数据[Play][2] - 65)   Y -= 1;
								if (GetAsyncKeyState(Charkey[3]) && Y <= 数据[Play][2] + 65)   Y += 1;

								X = 数据[Play][1] + 8 + Dir * sqrt(pow(70, 2) - pow(数据[Play][2] - Y, 2));

								Sigma = atan2((数据[Play][2] - Y), (X - 数据[Play][1]) - 8) * (180 / pi);
							}

							DirKey = 0;
							delay(5);
						}
						else {
							Xx = mx;
							Yy = my;

							//16和21为直线
							if (Skill == 16 || Skill == 21) {
								if (sqrt((数据[Play][1] + 8 - mx) * (数据[Play][1] + 8 - mx) + (数据[Play][2] - my) * (数据[Play][2] - my)) <= 70)
								{
									Sigma = atan2((数据[Play][2] - my), (mx - 数据[Play][1]) - 8) * (180 / pi);

									X = 数据[Play][1] + 8 + cos(Sigma * (pi / 180)) * 70;
									Y = 数据[Play][2] - sin(Sigma * (pi / 180)) * 70;
								}
							}
							else {
								X = Xx = mx;
								Y = Yy = my;
							}


							if (GetAsyncKeyState(VK_LBUTTON)) key = -30;
						}

						LoadMap();
						DrawMan();

						//不能超出屏幕边界
						if (Skill != 16 && Skill != 21) {

							if (X <= UIleft) X = UIleft;
							if (-UIleft <= X)	X = -UIleft;
							if (Y <= UItop)  Y = UItop;
							if (-UItop <= Y)	Y = -UItop;
						}

						//炮台只能放在玩家附近
						if (Skill == 15)
						{
							if (X + 8 - PlayersData[Play][1] - 8 > 56)  X = PlayersData[Play][1] + 56;
							if (X + 8 - PlayersData[Play][1] - 8 < -40)  X = PlayersData[Play][1] - 40;
							if (Y + 8 - PlayersData[Play][2] - 8 > 56)  Y = PlayersData[Play][2] + 56;
							if (Y + 8 - PlayersData[Play][2] - 8 < -40)  Y = PlayersData[Play][2] - 40;

						}

						//显示方式
						switch (Skill)
						{
						case 4:	//战姬
						{
							if (Dir == -1)
								TEXTOUT(L"↙↙↙", X - 25, Y - 10, 3, L"幼圆", 30, 130, 255);
							else
								TEXTOUT(L"↘↘↘", X - 25, Y - 10, 3, L"幼圆", 30, 130, 255);
						}
						break;

						case 5:	//天姬
						{
							TEXTOUT(L"天姬", X - 19, UItop, 4, L"幼圆", 210, 100, 255);

							for (int yyyyy = UItop + 20; yyyyy <= -UItop + 5; yyyyy++)
							{
								if (getpixel(X, yyyyy) == Block)
								{
									TEXTOUT(L"↓", X - 22, yyyyy - 45, 18, L"幼圆", 200, 180, 255);
									break;
								}
								putpixel(X, yyyyy, RGB(200, 80, 255));
							}
						}
						break;

						case 9: //字符斩
						{
							TEXTOUT(L"斩", X - 8, Y - 8, 4, L"华文楷体", 255, 0, 0);
							TEXTOUT(L"→    ←", X - 30, Y - 10, 3, L"幼圆", 255, 0, 0);
							TEXTOUT(L"↗", X - 28, Y + 8, 3, L"幼圆", 255, 0, 0);
							TEXTOUT(L"↙", X + 10, Y - 25, 3, L"幼圆", 255, 0, 0);
						}
						break;

						case 10://瞬移到大平台
						{
							setlinecolor(RGB(50, 80, 255));
							circle(X, Y, 12);
							//Prime_RECT(X-20, Y);
							TEXTOUT(L"大平台", X - 35, Y + 12, 8, L"等线", 150, 0, 150, 255, 200, 200);
						}
						break;

						case 12://掉雨
						{
							TEXTOUT(L"↓↓↓↓↓↓↓↓↓", X - 75, UItop, 3, L"幼圆", 200, 180, 255);
						}
						break;

						case 15://炮台
						{
							TEXTOUT(L"↓", X - 12, Y - 35, 7, L"幼圆", 200, 200, 255);
							TEXTOUT(L"枪", X - 8, Y - 8, 3, L"幼圆", 255, 200, 100);
						}
						break;

						case 16://电弧陷阱
						{
							setlinecolor(RGB(0, 0, 0));
							circle(X, Y, 10);
							line(数据[Play][1] + 8, 数据[Play][2], X, Y);
							setlinecolor(RGB(0, 255, 200));
							line(X - 6, Y, X + 6, Y);
							line(X, Y - 6, X, Y + 6);
						}
						break;

						case 17://钻井弹
						{
							for (int i = UItop; i <= -UItop + 20; i += 30)
								TEXTOUT(L"↓", X - 15, i, 10);
						}
						break;

						case 18://半程追踪弹
						{
							setlinecolor(RGB(0, 255, 200));
							line(X - 10, Y, X + 10, Y);
							line(X, Y - 10, X, Y + 10);

						}
						break;

						case 21:
						{
							setlinecolor(RGB(0, 0, 0));
							circle(X, Y, 10);
							line(数据[Play][1] + 8, 数据[Play][2], X, Y);
							setlinecolor(RGB(0, 255, 200));
							line(X - 6, Y, X + 6, Y);
							line(X, Y - 6, X, Y + 6);

						}
						break;
						default:
							setlinecolor(RGB(50, 80, 255));
							circle(X, Y, 12);
							break;
						}

					}

					if ((GetAsyncKeyState(VK_RETURN) & 0x8000) || key == -30)	//-30是鼠标左键
					{
						key = 30;
						if (Skill == 2 || Skill == 10 || Skill == 15)	//瞬移  机枪
							if (getpixel(X, Y) == Block || getpixel(X - 6, Y - 6) == Block ||
								getpixel(X - 6, Y + 6) == Block || getpixel(X + 6, Y + 6) == Block ||
								getpixel(X, Y - 6) == Block || getpixel(X, Y + 6) == Block)
							{
								key = 0;

								if (Skill == 2 || Skill == 10)
									TEXTOUT(L"不能瞬移至这里", X - 35, Y - 20, 1);
								if (Skill == 15)
									TEXTOUT(L"不能放置至这里", X - 35, Y - 20, 1);

							}
						if (X < UIleft || -UIleft < X || Y < UItop || -UItop < Y)	key = 0;

						if (key == 30)
						{
							if (Skill != 21)
								SkillRep--;

							if (Skill == 12) SkillRep = 1;
							break;
						}
					}
					delay(1);
				}

			}

			LoadMap();
			DrawMan();

			//技能判断与初始化与部分技能动画
			switch (Skill)
			{
				//回血
			case -2:
			{
				Bomb = 0;

				sprintf(str, "+300");

				if (数据[Play][9] + 300 <= 4800)
					PlayersData[Play][9] += 300;
				else {
					sprintf(str, "+%.0f", 300 - 4800 + 数据[Play][9]);
					数据[Play][9] = 4800;
				}
				TEXTOUT(PATHcTow(str), PlayersData[Play][1] - 5, PlayersData[Play][2] - 12, 1, L"幼圆", 250, 80, 50);
				Sleep(100);
				break;
			}
			//跳回合
			case -1:
			{
				PlayersData[Play][6] += 5;
				PlayersData[Play][8] += 1;
				TEXTOUT(L"+5oil", PlayersData[Play][1] - 12, PlayersData[Play][2] - 12, 2, L"幼圆", 100, 100, 250);
				Sleep(250);
				Bomb = 0;

				break;
			}
			//不用技能，这里包括一些回溯技能
			case 0: {
				if (r == 0) { Repeat = 0; Bomb = 1; }
				break;
			}
				  //伤害提升
			case 1:
			{
				Inj *= 2; r = 0;
				Repeat = 0; Bomb = 1;

				break;
			}
			//进行瞬移
			case 2:
			{
				Repeat = Bomb = 0;

				if (SkillRep == 0) {
					SkillRep = -1;
					goto SkillBack;	//返回循环部分，以指定瞬移位置
				}
				//记录目标点
				Xx = X - 7;
				Yy = Y - 7;
				SavXY(X, Y);

				//缓慢瞬移
				while (fabs(PlayersData[Play][1] - Xx) > 0.1 || fabs(PlayersData[Play][2] - Yy) > 0.1)
				{
					if (PlayersData[Play][1] <= Xx) PlayersData[Play][1] += RANDOM(-0.6, 1);
					if (PlayersData[Play][1] > Xx) PlayersData[Play][1] += RANDOM(-1, 0.6);
					if (PlayersData[Play][2] <= Yy) PlayersData[Play][2] += RANDOM(-0.6, 1);
					if (PlayersData[Play][2] > Yy) PlayersData[Play][2] += RANDOM(-1, 0.6);
					if (PlayersData[Play][1] <= UIleft - 20 || -UIleft + 20 <= PlayersData[Play][1] || PlayersData[Play][2] >= -UItop + 20) break;

					TEXTOUT(PlayerChar[Play - 1], PlayersData[Play][1], PlayersData[Play][2], 3, L"幼圆", PlayersData[Play][3], PlayersData[Play][4], PlayersData[Play][5], 255, 255, 255);
					delay(1);
				}

				PlayersData[Play][1] = X - 6;
				PlayersData[Play][2] = Y - 6;
				LoadMap();
				for (int i = 1; i <= Players; i++)
					if (PlayersData[i][9] > 0)
						TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);

				Sleep(200);

				break;
			}
			//皇嘉穿甲弹
			case 3:
			{
				Repeat = 0; Bomb = 1;
				Inj = 300; a = 0; r = 1.5;

				break;
			}
			//战姬轰炸
			case 4:
			{
				Repeat = 0; Bomb = 1;
				Inj = 450; a = 0; r = 2;

				if (SkillRep == 0) {
					SkillRep--;
					goto SkillBack;	//返回循环部分，以指定轰炸位置
				}
				SavXY(X, Y);

				//超出抛射点而且爆姬了，返回此处抛弹
				if (SkillRep == 1)
				{
					x0 = FireTemp[4];
					y0 = FireTemp[5];
					Sigma = -30;  v0 = 20;
					SkillRep = -3;
				}

				//战姬进场
				if (SkillRep == -2) {
					float FPx = X - (map_wide + 200) * Dir;
					float FPy = Y - map_high + 200;
					float Ft = 0, ax = 20, ay = 8;
					vx = 40; vy = 14;
					while (Ft <= 1.5)
					{
						Ft += 0.01;

						Prime_RECT(FPx, FPy, FPx + 36, FPy + 16);

						if (Ft >= 0.72) { ay += 4; ax -= 3; }
						if (Ft >= 0.74 && Ft <= 0.75)
						{
							FireTemp[4] = x0 = FPx;
							FireTemp[5] = y0 = FPy + 16;
							Sigma = -30;  v0 = 20;
						}

						FPx += Dir * (vx * Ft - ax * pow(Ft, 2));
						FPy += vy * Ft - ay * pow(Ft, 2);
						TEXTOUT(L"战机", FPx, FPy, 3, L"幼圆", 150, 140, 255);

						//撞姬
						if (getpixel(FPx, FPy) == Block || getpixel(FPx + 32, FPy) == Block ||
							getpixel(FPx, FPy + 16) == Block || getpixel(FPx + 32, FPy + 16) == Block ||
							getpixel(FPx, FPy + 8) == Block || getpixel(FPx + 32, FPy + 8) == Block)
						{
							if (Ft > 0.75)
								SkillRep = 1;

							Repeat = 1;
							Inj = 550; r = 5;

							x0 = FPx;
							y0 = FPy + 16;

							setlinecolor(RGB(255, 50, 0));
							for (int f = 0; f <= (int)Inj * 2 / 25; f++)
							{
								line(x0, y0, x0 + RANDOM(-4, 4) * 7, y0 + RANDOM(-4, 4) * 7);
							}

							Sleep(80);
							break;
						}


						delay(5);
					}
				}

				break;
			}
			//天姬,参上
			case 5:
			{
				Repeat = 1; Bomb = 1;
				Inj = 1000; a = 0; r = 7;

				if (SkillRep == 0) {
					SkillRep--;
					goto SkillBack;	//返回循环部分，以指定参上位置
				}
				SavXY(X, Y);

				float ttttt = 0;
				int yyyyy = UItop + 20;
				for (; yyyyy <= -UItop + 10; yyyyy++) {
					if (getpixel(X, yyyyy) == Block)
					{
						Y = yyyyy;
						break;
					}

					if (yyyyy >= -UItop) { Repeat = Bomb = 0; break; }
				}

				Prime_RECT(X - 2, UItop, X + 2, yyyyy, 220, 200, 255, 255, 255, 255);
				while (Y < -UItop)
				{
					ttttt += 0.1;
					Prime_RECT(X - ttttt * 10, UItop, X + ttttt * 10, yyyyy + 10, 220, 200, 255, (int)220 + ttttt * 23, (int)100 + ttttt * 100, (int)200 + 36 * ttttt);
					if (ttttt >= 1.5)
					{
						x0 = X - 8; y0 = Y;
						break;
					}
					Sleep(1);
				}


				break;
			}
			//爆炸飞羊
			case 6:
			{
				Repeat = 1; Bomb = 1;

				int ttt = 0;
				float SleepX = x0, SleepY = y0;  //ShlelelelppspspsllellepsllslhhsllepSslepse
				int DirShp = Dir;

				Inj = 850; r = 5;
				Sleep(150);  //SHEEPSLEEP150ms

				//地面跑步阶段
				while (1)
				{
					ttt++;
					delay(1);

					if (ttt >= 8 * 1000)
					{
						sprintf(str, " %.1f ", (float)((15 * 1000 - ttt) / 1000.0));
						TEXTOUT(PATHcTow(str), SleepX - 5, SleepY - 12, 1, L"幼圆", 50, 80, 255, 255, 255, 255);
					}

					if (ttt % 1000 == 0) DirShp = (int)RANDOM(-2, 2);
					if (DirShp <= 0 && getpixel(SleepX - 2, SleepY + 5) != Block) SleepX -= 0.1;
					if (DirShp > 0 && getpixel(SleepX + 17, SleepY + 5) != Block) SleepX += 0.1;

					if (getpixel(SleepX + 5, SleepY + 17) != Block && getpixel(SleepX + 11, SleepY + 17) != Block)
						SleepY += 0.1;
					if (getpixel(SleepX, SleepY + 17) == Block || getpixel(SleepX + 17, SleepY + 17) == Block)
						SleepY -= 0.2;

					if (ttt == 15 * 1000 || SleepY >= -UItop + 20 ||
						(GetAsyncKeyState(VK_LBUTTON) & 0x8000) || GetAsyncKeyState(VK_RETURN) & 0x8000)
					{
						if (SleepY >= -UItop + 20) Bomb = 0;
						break;
					}


					//绘画像
					for (int i = 1; i <= Players; i++)
						if (PlayersData[i][9] > 0)
							TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);

					TEXTOUT(L"羊", SleepX, SleepY, 3, L"幼圆", 255, 50, 50, 255, 255, 255);
				}

				ttt = 0;
				v0 = 2;
				float ax = 0, ay = 0, SHeepSiagme = 90;

				//火箭升空阶段
				while (1)
				{
					ttt++;
					delay(1);

					if (ttt >= 8 * 1000)
					{
						sprintf(str, " %.1f ", (float)((15 * 1000 - ttt) / 1000.0));
						TEXTOUT(PATHcTow(str), SleepX - 5, SleepY - 12, 1, L"幼圆", 50, 80, 255, 255, 255, 255);
					}


					if ((GetAsyncKeyState(Charkey[1]) & 0x8000) || (GetAsyncKeyState(Charkey[2]) & 0x8000))  SHeepSiagme -= 0.4;
					if ((GetAsyncKeyState(Charkey[0]) & 0x8000) || (GetAsyncKeyState(Charkey[3]) & 0x8000))  SHeepSiagme += 0.4;
					if (SHeepSiagme > 360) SHeepSiagme = 0;
					if (SHeepSiagme < 0)   SHeepSiagme = 360;
					ax = v0 * cos(SHeepSiagme * pi / 180) * 0.1;
					ay = v0 * sin(SHeepSiagme * pi / 180) * 0.1;

					SleepX += ax;	SleepY -= ay;
					if (ttt >= 15 * 1000 || SleepY >= -UItop + 50 || UItop - 150 >= SleepY || SleepX <= UIleft - 150 || -UIleft + 150 <= SleepX)
					{
						Bomb = 0; break;
					}

					if (getpixel(SleepX - 1, SleepY - 1) == Block || getpixel(SleepX + 17, SleepY - 1) == Block ||
						getpixel(SleepX - 1, SleepY + 8) == Block || getpixel(SleepX + 17, SleepY + 8) == Block ||
						getpixel(SleepX - 1, SleepY + 17) == Block || getpixel(SleepX + 17, SleepY + 17) == Block ||
						getpixel(SleepX + 8, SleepY - 1) == Block || getpixel(SleepX + 8, SleepY + 17) == Block)
					{
						x0 = SleepX;
						y0 = SleepY + 10;
						Repeat = 1;
						break;
					}
					TEXTOUT(L"羊", SleepX, SleepY, 3, L"幼圆", 255, 50, 50, 255, 255, 255);

				}

				break;
			}
			//Fox3飞弹
			case 7:
			{
				Repeat = 0; Bomb = 1;

				if (SkillRep == 0) {
					x0 = FireTemp[4] = PlayersData[Play][1];
					y0 = FireTemp[5] = PlayersData[Play][2];
					r = 0; Inj = 150;
					SkillRep++;
				}
				else {
					x0 = FireTemp[4] = PlayersData[Play][1];
					y0 = FireTemp[5] = PlayersData[Play][2];

					r = FireTemp[2];
					Dir = FireTemp[7];
					SkillRep++;

					Sigma += RANDOM(-3, 3);
					v0 += RANDOM(-1, 1);
				}

				break;
			}
			//护盾充能
			case 8:
			{
				Bomb = 0;
				sprintf(str, "+300");

				if (数据[Play][10] + 300 <= 4800)
					PlayersData[Play][10] += 300;
				else {
					sprintf(str, "+%.0f", 300 - 4800 + 数据[Play][10]);
					数据[Play][10] = 4800;
				}
				TEXTOUT(PATHcTow(str), PlayersData[Play][1] - 5, PlayersData[Play][2] - 12, 1, L"幼圆", 50, 80, 250);

				Sleep(400);

				break;
			}
			//字符斩
			case 9:
			{
				Repeat = 1; Bomb = 1;
				Inj = 1000; a = 0; r = -10;

				if (SkillRep == 0) {
					SkillRep--;
					goto SkillBack;	//返回循环部分，以指定斩杀位置
				}
				SavXY(X, Y);

				float LashX = PlayersData[Play][1];
				float LashY = PlayersData[Play][2];
				x0 = X - 8; y0 = Y + 5;
				float LashDistance = sqrt((LashX - X) * (LashX - X) + (LashY - Y) * (LashY - Y));

				/*
				sprintf(str, "%.1f", LashDistance);
				TEXTOUT(PATHcTow(str), 0,0);
				system("pause");*/

			LashBack:  //不满足条件回溯
				float LashMode = RANDOM(1, 10);

				//技能是可以整合的但是我懒
				//高数突进斩
				if (LashMode <= 4.5)
					if (LashDistance <= 300) {

						double Lashangle = atan2(Y - PlayersData[Play][2], PlayersData[Play][1] - X); //弧度制
						double angleTemp = Lashangle;
						float ax = 0, ay = 0, axTemp = 0, ayTemp = 0, Xtemp = 0, Ytemp = 0, XLash = PlayersData[Play][1], YLash = PlayersData[Play][2];
						int Lashttt = 0, tttTemp = 0, v0Temp = 30;
						v0 = 30;

						while (1)
						{
							Lashttt++;

							if (fabs(LashX - PlayersData[Play][1]) <= 1 && fabs(LashY - PlayersData[Play][2]) <= 1 && Lashttt > 60);
							else
							{
								ax = v0 * cos(Lashangle) * 0.1;
								ay = v0 * sin(Lashangle) * 0.1;

								PlayersData[Play][1] += ax;
								PlayersData[Play][2] -= ay;
							}


							TEXTOUT(PlayerChar[Play - 1], PlayersData[Play][1], PlayersData[Play][2], 3, L"幼圆", PlayersData[Play][3], PlayersData[Play][4], PlayersData[Play][5]);

							if (Lashttt == 90) Lashangle += 180;
							if (Lashttt == 250) Lashangle -= 180;
							if (Lashttt == 110)
							{
								v0 = 40;
								Lashangle = atan2(PlayersData[Play][2] - Y, X - PlayersData[Play][1]);
								Xtemp = PlayersData[Play][1];
								Ytemp = PlayersData[Play][2];

							}
							if (Lashttt == 280)
							{
								v0 = 20;
								Lashangle = atan2(PlayersData[Play][2] - LashY, LashX - PlayersData[Play][1]);
								Xtemp = PlayersData[Play][1];
								Ytemp = PlayersData[Play][2];
							}


							if (Lashttt >= 60) {
								axTemp = v0Temp * cos(angleTemp) * 0.1;
								ayTemp = v0Temp * sin(angleTemp) * 0.1;

								XLash += axTemp;
								YLash -= ayTemp;


								TEXTOUT(PlayerChar[Play - 1], XLash, YLash, 3, L"幼圆", 255, 255, 255);
							}

							if (Lashttt == 60 + 90) angleTemp += 180;
							if (Lashttt == 60 + 250) angleTemp -= 180;
							if (Lashttt == 60 + 110)
							{
								v0Temp = 40;
								angleTemp = atan2(PlayersData[Play][2] - Y, X - PlayersData[Play][1]);
							}
							if (Lashttt == 60 + 280)
							{
								v0Temp = 20;
								angleTemp = atan2(PlayersData[Play][2] - LashY, LashX - PlayersData[Play][1]);
							}

							if (fabs(LashX - XLash) <= 5 && fabs(LashY - YLash) <= 5 && Lashttt > 280)
							{
								Sleep(150);
								break;
							}



							delay(1);
						}
					}
					else  goto LashBack;

				//超级大长刀
				if (4 < LashMode && LashMode < 9.5)
					if (LashDistance >= 150) {

						double Lashangle = atan2(PlayersData[Play][2] - Y, X - PlayersData[Play][1]) * (180 / pi);
						if (Lashangle <= 0) Lashangle = 360 + Lashangle;

						if (PlayersData[Play][1] <= X) { Lashangle += 45;  Dir = 1; }
						else { Lashangle -= 45; Dir = -1; }		//抬刀45度是你的美

						float YamatoDis = 10,
							YamatoX = LashX + cos(Lashangle * (pi / 180)) * YamatoDis,
							YamatoY = LashY - sin(Lashangle * (pi / 180)) * YamatoDis;//每寸大和剑

						unsigned int Rotnums = 0;

						while (1)
						{

							YamatoX = LashX + cos(Lashangle * (pi / 180)) * YamatoDis;
							YamatoY = LashY - sin(Lashangle * (pi / 180)) * YamatoDis;

							if (UIleft - 20 <= YamatoX && YamatoX <= -UIleft &&
								UItop - 20 <= YamatoY && YamatoY <= -UItop)
							{
								YamatoDis += 5;
							}
							else {
								YamatoDis = 10;

								delay(10);

								//再来一遍清除痕迹，因此改数据最好查找一起改掉
								while (true)
								{
									YamatoX = LashX + cos(Lashangle * (pi / 180)) * YamatoDis;
									YamatoY = LashY - sin(Lashangle * (pi / 180)) * YamatoDis;

									if (UIleft - 20 <= YamatoX && YamatoX <= -UIleft &&
										UItop - 20 <= YamatoY && YamatoY <= -UItop)
									{
										YamatoDis += 5;
									}
									else break;


									TEXTOUT(L"刀", YamatoX, YamatoY, 3, L"幼圆", 255, 255, 255);

								}

								YamatoDis = 10;
								if (Dir == 1) Lashangle -= 5;
								else Lashangle += 5;
								Rotnums++;

							}
							TEXTOUT(L"刀", YamatoX, YamatoY);

							if (Rotnums == 20) break;
							//delay(5);
						}
					}
					else  goto LashBack;

				//吐字清晰
				if (LashMode >= 9.5)
					if (LashDistance >= 100) {
						double Lashangle = atan2(PlayersData[Play][2] - Y, X - PlayersData[Play][1]) * (180 / pi);
						float YamatoDis = LashDistance / 80,
							YamatoX = LashX + cos(Lashangle * (pi / 180)) * YamatoDis,//CopyPasteYYDS
							YamatoY = LashY - sin(Lashangle * (pi / 180)) * YamatoDis;//每寸大和剑

						int LashChars = 0, Lashttt = 0, Where = 0; //'他'到底吐了多少字,吐了多久,吐了什么
						const TCHAR* LashTuArry[] = { L"们", L"叉", L"你", L"是", L"沃", L"吃", L"勾", L"石", L"的", L"找", L"真" };
						BOOL HitHito = FALSE;

						for (int f = 1; f <= Players; f++)	//记录每个精彩的距离
						{
							float dis = sqrt(pow(PlayersData[f][1] + 8 - X, 2) + pow(PlayersData[f][2] + 8 - Y, 2));
							if (f != Play && dis <= 24 + r) HitHito = TRUE;
						}
						while (true)
						{
							YamatoDis += LashDistance / 80;

							Prime_RECT(YamatoX, YamatoY, YamatoX + 16, YamatoY + 16);

							if (LashChars == 15) break;

							YamatoX = LashX + cos(Lashangle * (pi / 180)) * YamatoDis;
							YamatoY = LashY - sin(Lashangle * (pi / 180)) * YamatoDis;

							if ((fabs(YamatoX - X) <= 25 && fabs(YamatoY - Y) <= 25 && HitHito == TRUE) ||
								UIleft - 20 > YamatoX || YamatoX > -UIleft ||
								UItop - 20 > YamatoY || YamatoY > -UItop)  //这可比sqrt节省多了
							{
								LashChars++;
								YamatoDis = 10;

								if (HitHito == TRUE)
								{
									setlinecolor(RGB(255, 50, 0));
									for (int f = 0; f <= 5; f++)
										line(X, Y, X + RANDOM(-3, 3) * 7, Y + RANDOM(-3, 3) * 7);
								}

							}

							Where = (int)RANDOM(0, 10);
							TEXTOUT(LashTuArry[Where], YamatoX, YamatoY);

							delay(1);
						}

					}
					else  goto LashBack;

				//system("pause");

				break;
			}
			//移动到平台
			case 10:
			{
				Repeat = Bomb = 0;

				if (SkillRep == 0) {
					SkillRep = -1;
					goto SkillBack;	//返回循环部分，以指定瞬移位置
				}
				SavXY(X, Y);

				PlayersData[Play][1] = X - 7;
				PlayersData[Play][2] = Y - 7;

				LoadMap();
				if (_tcsicmp(PlayerChar[0], L"凉") == 0)
					TEXTOUT(L"STARRY", X - 35, Y + 12, 8, L"等线", 150, 0, 150, 255, 200, 200);
				else
					TEXTOUT(L"大平台", X - 35, Y + 12, 8, L"等线", 150, 0, 150, 255, 200, 200);
				SaveAnSave();
				Sleep(150);


				break;
			}
			//上勾拳
			case 11:
			{
				Repeat = 1; Bomb = 1;
				Inj = 200; a = 0; r = 5;

				break;
			}
			//裁雨留虹
			case 12:
			{
				Repeat = 0; Bomb = 1;
				Inj = 85; a = 0; r = -12; Sigma = -90; v0 = 35;

				if (SkillRep == 0) {
					SkillRep = -1;
					Shields = 0;
					goto SkillBack;	//返回循环部分，以指定参上位置
				}
				SavXY(X, Y);

				x0 = X + RANDOM(-75, 75);
				y0 = UItop - 20;
				SkillRep++;

				break;
			}
			//抛物线保护
			case 13:
			{
				Repeat = Bomb = 0;
				PlayersData[Play][11] = 1;

				setlinecolor(RGB(50, 60, 255));
				arc(PlayersData[Play][1] - 5, PlayersData[Play][2] - 5, PlayersData[Play][1] + 21, PlayersData[Play][2] + 21, -40 * (pi / 180), 220 * (pi / 180));

				Sleep(100);
				break;
			}
			//南蛮冲撞
			case 14:
			{
				Repeat = Bomb = 1;

				if (SkillRep == 0) {
					FireTemp[6] = 0;	//初始时间清0
					FireTemp[4] = x0, FireTemp[5] = y0;  //float FireTemp[4] = x0, FireTemp[5] = y0;ox?OiY!o_O
					SkillRep++;
				}
				float Distance[11]{ 0 };
				int DirBull = Dir;
				int CAo = 0;

				Inj = 120; r = 2;
				Sleep(150);  //BULLSLEEP150ms

				//地面跑步阶段
				while (1)
				{
					FireTemp[6]++;
					delay(1);//60000》》1min

					if (DirBull <= 0) FireTemp[4] -= 0.1;
					else FireTemp[4] += 0.1;

					if (getpixel(FireTemp[4] + 5, FireTemp[5] + 17) != Block && getpixel(FireTemp[4] + 11, FireTemp[5] + 17) != Block)
						FireTemp[5] += 0.1;
					if (getpixel(FireTemp[4], FireTemp[5] + 17) == Block || getpixel(FireTemp[4] + 17, FireTemp[5] + 17) == Block || getpixel(FireTemp[4] + 8, FireTemp[5] + 17) == Block)
						FireTemp[5] -= 0.2;

					if (FireTemp[4] <= UIleft - 25 || -UIleft + 15 <= FireTemp[4] || -UItop + 5 <= FireTemp[5] || FireTemp[6] >= 5000)
					{
						FireTemp[6] = 99999;
						Repeat = Bomb = 0;

						break;
					}

					//绘画像和判断到人距离			记得添加捡包功能
					for (int i = 1; i <= Players; i++)
						if (PlayersData[i][9] > 0)
						{
							TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);

							if (Play != i)
							{
								//绝对差值求和，"我"抽象为矩形而不是圆形(sqrt)，确保字体大小一样可以补差价
								Distance[i] = fabs(PlayersData[i][1] - FireTemp[4]) + fabs(PlayersData[i][2] - FireTemp[5]);

								if (Distance[i] <= 6) { //点到矩形最小距离
									CAo = TRUE;
									break;
								}
							}
						}

					//撞墙?撞人?
					if (getpixel(FireTemp[4] - 2, FireTemp[5] + 5) == Block || getpixel(FireTemp[4] - 2, FireTemp[5] + 10) == Block ||
						getpixel(FireTemp[4] + 17, FireTemp[5] + 5) == Block || getpixel(FireTemp[4] + 17, FireTemp[5] + 10) == Block ||
						CAo == TRUE)
					{
						x0 = FireTemp[4] + 8;
						y0 = FireTemp[5] + 8;
						FireTemp[6] += 1000;
						break;
					}

					TEXTOUT(L"牛", FireTemp[4], FireTemp[5], 3, L"幼圆", 200, 100, 50, 255, 255, 255);
					sprintf(str, "剩余：%.1f  ", 5 - FireTemp[6] / 1000.0);
					TEXTOUT(PATHcTow(str), UIleft + 20, UItop + 10, 3, L"幼圆", 200, 100, 50, 255, 255, 255);

				}


				break;
			}
			//堡垒机关枪
			case 15:
			{
				Repeat = Bomb = 0;

				if (SkillRep == 0) {
					SkillRep = -1;
					goto SkillBack;	//返回循环部分，以指定放置位置
				}
				SavXY(X, Y);
				Sleep(150);

				PlayersData[10][13] = Play;  //枪主人
				PlayersData[10][14] = 500;   //枪血量
				PlayersData[10][15] = X;   //枪坐标
				PlayersData[10][16] = Y;   //枪坐标
				PlayersData[10][17] = 0;     //枪状态
				PlayersData[10][18] = 0;     //枪降伤



				break;
			}
			//电弧陷阱
			case 16:
			{
				Repeat = 0; Bomb = 1;
				Inj = 250; r = 1;  v0 = 30;
				Sleep(150);

				if (SkillRep == 0) {
					SkillRep--;
					goto SkillBack;	//返回循环部分，以指定射击角度
				}


				break;
			}
			//钻钻钻钻井
			case 17:
			{
				Repeat = 1; Bomb = 1;
				Inj = 380; a = 0; r = 2;

				if (SkillRep == 0) {
					SkillRep--;
					goto SkillBack;	//返回循环部分，以指定参上位置
				}
				SavXY(X, Y);

				Y = UItop - 20;
				int dropttt = 0;
				BOOL looooop = 1;

				while (looooop)
				{
					if (getpixel(X, Y + 10) == Block)
						dropttt++;

					Y += 8;
					TEXTOUT(L"钻", X - 8, Y - 8, 3, L"幼圆", 170, 140, 0);
					Sleep(1);
					TEXTOUT(L"钻", X - 8, Y - 8, 3, L"幼圆", 255, 255, 255);

					for (int i = 1; i <= Players; i++)
						if (fabs(X - PlayersData[i][1] - 8) <= 11 && fabs(Y - PlayersData[i][2] - 8) <= 11 && PlayersData[i][9] > 0)
						{
							looooop = 0;
							break;
						}

					//补给包再判定
					if (fabs(X - PlayersData[10][9]) <= 8 && fabs(Y - PlayersData[10][10]) <= 8 && PlayersData[10][7] > 0)
						break;

					//炮台伤害判定
					if (fabs(X - 数据[10][15]) <= 11 && fabs(Y - 数据[10][16]) <= 11 && 数据[10][14] > 0)
						break;

					if (dropttt >= 15 || Y >= -UItop + 40) break;

					LoadMap();
					Prime_RECT(X - 8, Y - 8, X + 8, Y + 8);
					SaveAnSave();
					DrawMan();

				}

				x0 = X - 8;
				y0 = Y;

				break;
			}
			//半程追踪弹
			case 18:
			{
				Repeat = 0; Bomb = 1;
				Inj = 420; r = 2;
				float xxyy[6]{ 0 };
				xxyy[0] = 数据[Play][1] + 8;
				xxyy[1] = 数据[Play][2] + 8;

				if (SkillRep == 0) {
					SkillRep--;
					goto SkillBack;	//返回循环部分，以指定参上位置
				}
				SavXY(X, Y);

				Sleep(100);

				if (SkillRep == -2)
					while (true)
					{
						for (int i = 0; i < sizeof(Charkey) / sizeof(Charkey[0]); i++)
							if ((GetAsyncKeyState(Charkey[i]) & 0x8000) || (GetAsyncKeyState(VK_SHIFT) & 0x8000) || SkillRep == -2)
							{
								if (GetAsyncKeyState(VK_SHIFT))
								{
									float XTemp = X, YTemp = Y;

									if (GetAsyncKeyState(Charkey[0]))   X -= 1;
									if (GetAsyncKeyState(Charkey[1]))   X += 1;
									if (GetAsyncKeyState(Charkey[2]))   Y -= 1;
									if (GetAsyncKeyState(Charkey[3]))   Y += 1;

									if (XTemp != X || YTemp != Y)
									{
										LoadMap();
										DrawMan();
										Sleep(5);
									}
								}
								else
								{
									if (GetAsyncKeyState(Charkey[2]) && Sigma < 80) Sigma++;
									if (GetAsyncKeyState(Charkey[3]) && Sigma > -60) Sigma--;
									if (GetAsyncKeyState(Charkey[4])) { Dir = -Dir; Sleep(100); }
									if (GetAsyncKeyState(Charkey[5]) && v0 < 25) v0++;
									if (GetAsyncKeyState(Charkey[6]) && v0 > 6) v0--;
									LoadMap();
									DrawMan();

									int DIidid = (Dir == 1) ? 0 : 180;  //这是令人看不懂的，可读性是不好的，不要为了那一点运行效率而使用????:::

									setlinecolor(RGB(v0 * 10, 255 - v0 * 9, v0 * 4));
									for (int f = 10; f < v0 + 10; f++)
										arc(xxyy[0] - f, xxyy[1] - f, xxyy[0] + f, xxyy[1] + f, (DIidid + Dir * Sigma - 10) * (pi / 180), (DIidid + Dir * Sigma + 10) * (pi / 180));
								}

								setlinecolor(RGB(0, 255, 200));
								line(X - 10, Y, X + 10, Y);
								line(X, Y - 10, X, Y + 10);

								if (SkillRep == -2) {
									SkillRep = -3;
									break;
								}
							}

						if (GetAsyncKeyState(VK_RETURN) & 0x8000)
							break;
						Sleep(1);
					}


				break;
			}
			//散装弹
			case 19:
			{
				Repeat = 0; Bomb = 1;
				Inj = 200; r = 1;

				if (SkillRep == 0) {
					x0 = FireTemp[4] = PlayersData[Play][1];
					y0 = FireTemp[5] = PlayersData[Play][2];
					SkillRep++;
				}
				else {
					x0 = FireTemp[4];
					y0 = FireTemp[5];

					Sigma = RANDOM(40, 55);
					v0 = RANDOM(0, 6);
					if (RANDOM(0, 1) >= 0.5)
						Dir = 1;
					else
						Dir = -1;

					r = 1;
					SkillRep++;
				}


				break;
			}
			//战墙
			case 20:
			{
				Repeat = 0; Bomb = 0;

				LoadMap();
				Prime_RECT(数据[Play][1] + 8 + Dir * 35, 数据[Play][2] - 50, 数据[Play][1] + 8 + Dir * 15, 数据[Play][2] + 20, 255, 200, 200);
				SaveAnSave();

				break;
			}
			//充能左轮枪
			case 21:
			{
				Repeat = 0; Bomb = 1;
				key = 0;  Inj = 63;  r = -12; v0 = 18;

				x0 = FireTemp[4] = PlayersData[Play][1] + 8;
				y0 = FireTemp[5] = PlayersData[Play][2] + 8;

				SkillRep++;
				if ((SkillRep - 1) % 3 == 0) {
					Sleep(100);
					goto SkillBack;	//返回循环部分，以指定射击直线
				}

				break;
			}
			//爆炸网球
			case 22:
			{
				x0 = x0 + 1;	y0 = y0 - 40;
				BOOL Sua = 1;
				Repeat = 1;
				Bomb = 1;
				Inj = 400; r = 1.6;
				Sleep(200);
				int Tset = 5;

				//发射前预览
				while(1)
				{
					for (int i = 2; i <= 6; i++)
						if (GetAsyncKeyState(Charkey[i]))
							Sua = 1;

					if (Sua == 1)
					{
						if (GetAsyncKeyState(Charkey[2]) && Sigma < 80)
							Sigma++;
						if (GetAsyncKeyState(Charkey[3]) && Sigma > -60)
							Sigma--;
						if (GetAsyncKeyState(Charkey[4]))
							Dir = -Dir;
						if (GetAsyncKeyState(Charkey[5]) && v0 < 24)
							v0++;
						if (GetAsyncKeyState(Charkey[6]) && v0 > 6)
							v0--;

						Sua = 0;
						tp = 0;
						vx = v0 * Dir * cos(Sigma * pi / 180) * 4;
						vy = v0 * sin(Sigma * pi / 180) * 4;
						setlinecolor(RGB(60, 235, 55));
						LoadMap();
						sprintf(str, "(TAB键)延迟爆破：%d", Tset);
						TEXTOUT(PATHcTow(str), UIleft, UItop, 5, L"幼圆", 50, 200, 255, 255, 255, 255);
						DrawMan();

						while(tp < 1.5)
						{
							tp += 0.001;
							X = x0 + vx * tp;
							Y = y0 - vy * tp + 5 * tp * tp;
							if ( int(tp*10) % 2 == 0)
								circle(X+5, Y+38, 1);
						}

					}

					if ((GetAsyncKeyState(VK_TAB) & 0x8000))
					{
						Tset++;
						if (Tset == 6)
							Tset = 1;
						sprintf(str, "(TAB键)延迟爆破：%d", Tset);
						TEXTOUT(PATHcTow(str), UIleft, UItop, 5, L"幼圆", 50, 200, 255, 255, 255, 255);
						Sleep(150);
					}

					if ((GetAsyncKeyState(VK_RETURN) & 0x8000))
						break;
					Sleep(1);
				}
				LoadMap();
				DrawMan();

				tp = 0;
				xp = X = x0; yp = Y = y0;

				vx = v0 * Dir * cos(Sigma * pi / 180) * 4;
				vy = v0 * sin(Sigma * pi / 180) * 4;
				float tx = 0, ty = 0, Lstp = 0;
				float ShellmidX = x0 + 5, ShellmidY = y0 + 38;
				int p2 = getpixel(X + 0.15, Y - 0.9),
					p4 = getpixel(X + 0.4, Y - 1.15),
					p6 = getpixel(X + 0.15, Y - 1.4),
					p7 = 0,
					p8 = getpixel(X - 0.1, Y - 1.15),
					p9 = 0;

				//发射后循环
				while (Repeat == 1)
				{
					tp += 0.001;
					tx += 0.015;
					ty += 0.015;
					TEXTOUT(L".", X, Y, 20, L"等线", 60, 235, 55);

					if (xp != X && yp != Y && TRUNCATE(Sigma, 0) != 270) {
						Sigma = asin((Y - yp) / DISTANCE(xp, yp, X, Y));
						if (X < xp) {
							Sigma = 180 - Sigma;
							Dir = -1;
						}
						else {
							Dir = 1;
						}
						if (Y <= yp) {
							Sigma = 360 + Sigma;
						}
					}

					xp = X;//改变之前的坐标
					yp = Y;

						X = x0 + vx * tx;
						Y = y0 - vy * ty + 5 * ty * ty;
					

					ShellmidX = X + 5, ShellmidY = Y + 38;
					p2 = getpixel(ShellmidX, ShellmidY + 7);
					p4 = getpixel(ShellmidX - 7, ShellmidY);
					p6 = getpixel(ShellmidX + 7, ShellmidY);
					//p7 = getpixel(ShellmidX - 5, ShellmidY - 7);
					p8 = getpixel(ShellmidX, ShellmidY - 7);
					//p9 = getpixel(ShellmidX + 5, ShellmidY - 7);//p9??
					
					//退出条件
					{
						if (X <= UItop - 300 || Y >= -UItop + 40 ||
							X <= UIleft - 100 || Y >= -UIleft + 100)
							Repeat = -1;

						if (fabs(vx) < 0.001 && fabs(vy) < 0.001)
							Lstp += 0.001;
						else
							Lstp = 0;
						if (Lstp >= 0.5)
							break;

						BOOL Ref = 0;
						for (int i = 1; i <= Players; i++)
							if (tp >= 0.5 && PlayersData[i][9] > 0 && PlayersData[i][11] > 0 && DISTANCE(ShellmidX, ShellmidY, 数据[i][1], 数据[i][2]) <= 12)
								Ref = 1;

						if (p2 == Block ||
							p4 == Block ||
							p6 == Block ||
							p8 == Block ||
							Ref == 1)
						{
							if (Ref == 1) {
								vx = -vx;
								vy = fabs(vy);
								vx = TRUNCATE(RANDOM(-3, 3), 2) + vx;
								vy = TRUNCATE(RANDOM(1, 4), 2) + vy;
							}

							//碰撞反弹
							{
								if (p6 == Block)
									vx = -0.8 * fabs(vx);
								if (p4 == Block)
									vx = 0.8 * fabs(vx);

								/*if ((p8 == Block || p4 == Block) && tx >= 0.1) {
									if ((Sigma >= 45 && Sigma <= 135) || (Sigma >= 225 && Sigma <= 315)) {
										vx = -0.9 * vx;
									}
									else {
										vx = -0.95 * Dir * cos(Sigma) * vx;
									}
								}*/

								if (p2 == Block) {
									vy = 0.95 * fabs(vy);
								}
								if (p8 == Block) {
									vx = vx * 0.6;
									vy = -0.9 * fabs(vy);
								}
							}

							tx = 0.0;
							ty = 0.0;
							x0 = X;
							y0 = Y;
							//system("pause");
						}


						if (tp >= Tset)
							break;
					}
				
					FallDown(0.5);

					delay(1);
					Prime_RECT(ShellmidX - 3, ShellmidY - 2, ShellmidX + 3, ShellmidY + 3);
					//Prime_RECT(ShellmidX - 5, ShellmidY - 5, ShellmidX + 5, ShellmidY + 5, 0,0,0);
					//putpixel(ShellmidX, ShellmidY + 7, RGB(255,0,0));
					//putpixel(ShellmidX - 7, ShellmidY, RGB(255,0,0));
					//putpixel(ShellmidX + 7, ShellmidY, RGB(255,0,0));
					//putpixel(ShellmidX, ShellmidY - 7, RGB(255,0,0));
					//system("pause");
				}

				if (Repeat == -1)
					Bomb = 0;

				x0 = ShellmidX;
				y0 = ShellmidY+10;

				break;
			}

			default:
				break;
			}
			if (Skill > 0)
			{
				FireTemp[0] = Skill;
				FireTemp[1] = Inj;
				FireTemp[2] = r;
			}//存储初始数据

			//炮弹绘画循环 为0时直接没有炮弹  repeat不为0时瞬爆
			if (Bomb == 1)
			{
				//---------------炮弹Head---------
				tp = 0;
				if (数据[Play][12] == 0)
					FireTemp[1] = Inj;
				else
					FireTemp[1] = Inj * 0.8;

				if (VxVy == TRUE) {
					vx = v0 * cos(Sigma * pi / 180) * 4;
					vy = v0 * sin(Sigma * pi / 180) * 4;
				}
				xp = x0;	yp = y0 - 10;
				float ShellmidX = xp + 7, ShellmidY = yp + 7;
				float LastX = x0, LastY = y0;
				int FireMode = 0, JudgeR = 0;
				if (Skill == 16 || Skill == 21)
					FireMode = 1;
				if (Skill == 21)
					JudgeR = -3;

				while (Repeat == 0)
				{
					tp += 0.01;
					//Prime_RECT(xp + 3, yp , xp + textwidth("炮") - 3 , yp + textheight("炮"));

					if (Skill != 21)
						drawshell(1, xp, yp);
					else
					{
						if ((int)FP(tp / 0.5) == 0) {
							LINE(LastX, LastY, xp, yp, { 50, 200, 255 });
							if (tp >= 0.5)
								LINE(x0 + 2 + vx * (tp - 0.2), y0 - 8 - vy * (tp - 0.2), x0 + 2 + vx * (tp - 0.1), y0 - 8 - vy * (tp - 0.1), { 255,255,255 });
							LastX = xp;  LastY = yp;
						}
					}

					if (Skill == 18 && tp >= 5 && FireMode == 0)
					{
						FireMode = 1;
						Sigma = atan2(ShellmidY - Y, X - ShellmidX) * (180 / pi);

						v0 = 20;
						vx = v0 * cos(Sigma * pi / 180) * 4;
						vy = v0 * sin(Sigma * pi / 180) * 4;
						tp = 0; x0 = xp - 2; y0 = yp + 8;
					}

					//飞行模式
					if (FireMode == 0)
					{
						xp = x0 + 2 + vx * tp * Dir + a * pow(tp, 2);
						yp = y0 - 8 - vy * tp + 5 * pow(tp, 2);
					}
					else
					{
						xp = x0 + 2 + vx * tp;
						yp = y0 - 8 - vy * tp;
					}

					if (Skill != 21) { ShellmidX = xp + 7, ShellmidY = yp + 7; }
					else { ShellmidX = xp, ShellmidY = yp; }


					if (yp <= UItop - 300 || yp >= -UItop + 40 ||
						xp <= UIleft - 100 || xp >= -UIleft + 100)
						Repeat = 1;

					//障碍
					if (Skill != 21) {
						if (getpixel(ShellmidX - 8, ShellmidY + 8) == Block ||     // 左上
							getpixel(ShellmidX + 8, ShellmidY + 8) == Block ||     // 右上
							getpixel(ShellmidX - 8, ShellmidY - 8) == Block ||     // 左下
							getpixel(ShellmidX + 8, ShellmidY - 8) == Block)     // 右下
							Repeat = 1;
					}
					else
						if (getpixel(xp, yp) == Block)
							Repeat = 1;

					//人障碍
					if (tp >= 0.7)
					{
						for (int i = 1; i <= Players; i++)
							if (fabs(ShellmidX - PlayersData[i][1] - 8) <= 11 + JudgeR && fabs(ShellmidY - PlayersData[i][2] - 8) <= 11 + JudgeR && PlayersData[i][9] > 0)
							{
								Repeat = 1;
								break;
							}

						//补给包
						if (fabs(ShellmidX - PlayersData[10][9] - 7) <= 9 + JudgeR && fabs(ShellmidY - PlayersData[10][10] - 7) <= 9 + JudgeR && PlayersData[10][7] > 0)
						{
							if (数据[10][7] == 4)
								数据[10][7] = -1;
							else
								数据[10][7] = 0;

							Repeat = 1;
							break;
						}

						//炮台
						if (fabs(ShellmidX - 数据[10][15]) <= 11 + JudgeR && fabs(ShellmidY - 数据[10][16]) <= 11 + JudgeR && 数据[10][14] > 0)
						{
							Repeat = 1;
							break;
						}
					}

					if (Skill != 21)
						drawshell(2, xp, yp);

					/*
					{
						Prime_RECT(200, 50, 250, 200, 255, 255, 254);
						sprintf_s(str, L"%.2f", t);		TEXTOUT(PATHcTow(str), 200, 50);
						sprintf_s(str, L"%.2f", PrDistce[1]);		TEXTOUT(PATHcTow(str), 200, 80);
						sprintf_s(str, L"%.2f", PrDistce[2]);		TEXTOUT(PATHcTow(str), 200, 110);
						sprintf_s(str, L"%.2f", fabs(ShellmidX - PlayersData[3][1] - 8));		TEXTOUT(PATHcTow(str), 200, 140);
						sprintf_s(str, L"%.2f", fabs(ShellmidY - PlayersData[3][2] - 8));		TEXTOUT(PATHcTow(str), 200, 170);
					}*/

					FallDown(0.5);

					//TEXTOUT(L"炮", xp, yp, 2, L"等线", 0, 0, 0, 255, 255, 255);
					delay(1);

				}	//cannonball move loop? 炮弹运动循环
				//---------------炮弹ass---------

				if (Skill != 21) { ShellmidX = xp + 7; ShellmidY = yp + 7; }
				else { ShellmidX = xp; ShellmidY = yp; }

				for (int f = 1; f <= Players; f++)	//记录每个精彩的距离
					if (PlayersData[f][9] > 0) {
						if (Skill != 11)
							PrDistce[f] = sqrt(pow(PlayersData[f][1] + 8 - ShellmidX, 2) + pow(PlayersData[f][2] + 8 - ShellmidY, 2));
						else {
							PrDistce[f] = sqrt(pow(PlayersData[f][1] - PlayersData[Play][1], 2) + pow(PlayersData[f][2] - PlayersData[Play][2], 2));
							if (PrDistce[f] <= 0.1) PrDistce[f] = 999;
						}

						//反弹
						if (数据[f][11] > 0 && PrDistce[f] <= 24 + r)
							if ((int)(r * 10) != -41 && (int)(r * 10) != 61 && (int)v0 != 42 &&
								(Skill <= 3 || (Skill == 4 && Inj != 550) || Skill == 7 || Skill == 8 || Skill == 13 || Skill == 19)) {
								setlinecolor(RGB(50, 60, 255));
								arc(PlayersData[f][1] - 5, PlayersData[f][2] - 5, PlayersData[f][1] + 21, PlayersData[f][2] + 21, -40 * (pi / 180), 220 * (pi / 180));
								Sleep(50);
								setlinecolor(RGB(255, 255, 255));
								arc(PlayersData[f][1] - 5, PlayersData[f][2] - 5, PlayersData[f][1] + 21, PlayersData[f][2] + 21, -40 * (pi / 180), 220 * (pi / 180));

								vx = -vx + RANDOM(-8, 8);  vy = fabs(vy) + RANDOM(3, 8);
								Repeat = 0; VxVy = FALSE; 数据[f][11] = 0;
								Skill = 0; x0 = xp; y0 = yp;
								goto SkillBack;
							}
					}

				Hittt = 0;	//抖动确定
				for (int f = 1; f <= Players; f++)
					if (PrDistce[f] <= 24 + r)
					{
						Hittt++;
						if (Skill == 9 && f == Play) { PrDistce[f] = 999; Hittt--; }
					}

				/* {
					Prime_RECT(200, 50, 250, 200, 255, 255, 254);
					sprintf_s(str, L"%.2f", t);		TEXTOUT(PATHcTow(str), 200, 50);
					sprintf_s(str, L"%.2f", PrDistce[1]);		TEXTOUT(PATHcTow(str), 200, 80);
					sprintf_s(str, L"%.2f", PrDistce[2]);		TEXTOUT(PATHcTow(str), 200, 100);
					sprintf_s(str, L"%.2f", PrDistce[3]);		TEXTOUT(PATHcTow(str), 200, 120);
				}*/

				//Fox3伤害改动
				if (Skill == 7)
					FireTemp[1] = Inj = 0.7 * sqrt(pow(FireTemp[4] - ShellmidX, 2) + pow(FireTemp[5] - ShellmidY, 2));

				//天姬屏幕抖动
				if (FireTemp[1] >= 500 && yp < map_high / 2.0 + 20 && (Hittt >= 1 || Skill == 4 || Skill == 5 || Skill == 6))
				{
					setorigin(0, 0);
					loadimage(&CharWar, PATHcTow(PATH), map_wide, map_high, true);

					for (int i = 0; i <= (int)FireTemp[1] / 62.5; i++) {
						putimage(pow(-1, i) * FireTemp[1] / 50, RANDOM(-FireTemp[1] / 100.0, FireTemp[1] / 100.0), &CharWar);
						delay(10);
					}
					/*
						for (int i = 0; i <= 8; i++) {
							putimage(pow(-1, i)*10, RANDOM(-5, 5), &CharWar);
							Sleep(2);
						}
					*/
				}

				LoadMap();

				//画绚烂圆圈，但是排除 斩！和 升！
				if (Skill != 9 && Skill != 11 && Skill != 21)
				{
					setbkcolor(RGB(255,255,255));
					clearcircle(ShellmidX, ShellmidY, 20 + r);
					setlinecolor(RGB(50, 80, 255));
					circle(ShellmidX, ShellmidY, 19 + r);
				}

				float PlayVx[12]{ 0 };  //水平飞速度
				float PlayVy[12]{ 0 };  //竖直飞速度
				float PFlyXt[12]{ 0 };   //水平飞时间
				float PFlyYt[12]{ 0 };   //竖直飞时间

				//补给包再判定
				if (fabs(xp - PlayersData[10][9]) <= 20 + r && fabs(yp - PlayersData[10][10]) <= 20 + r && PlayersData[10][7] > 0)
				{
					if (数据[10][7] == 4)
						数据[10][7] = -1;
					else
						数据[10][7] = 0;
				}

				//炮台伤害判定
				if (fabs(ShellmidX - 数据[10][15]) <= 24 + r && fabs(ShellmidY - 数据[10][16]) <= 24 + r && 数据[10][14] > 0)
				{
					Inj = FireTemp[1];
					数据[10][14] -= Inj;

					if (数据[10][14] <= 0)
						数据[10][17] = -1;
				}

				//伤害判定
				for (int f = 1; f <= Players; f++)	//查询每个精彩的距离
					if (PrDistce[f] <= 24 + r)
					{
						Inj = FireTemp[1];

						if (Skill == 12) {
							PlayersData[Play][10] += 50;
							Shields += 50;
							sprintf(str, "+50");
							TEXTOUT(PATHcTow(str), PlayersData[Play][1], PlayersData[Play][2] - 20, 1, L"幼圆", 100, 100, 255, 255, 255, 255);
						}
						if (PrDistce[f] >= 18) {
							Inj = Inj * PrDistce[f] / 30;
							if (PrDistce[f] >= 24) Inj = Inj * 0.8;
							if (Skill == 11) Inj = 200;
						}
						if (PrDistce[f] <= 12 + r && (Skill <= 3 || Skill == 8 || Skill == 13 || Skill == 22)) {
							Inj = Inj * 1.5;
							TEXTOUT(L"暴击!", PlayersData[f][1], PlayersData[f][2] - 20, 1, L"华文新魏", 255, 0, 0);
						}

						PlayVx[f] = 0.5 * (PlayersData[f][1] + 8 - ShellmidX);	//能这么直接写算法的也是天才
						PlayVy[f] = -0.9 * sqrt(fabs(PrDistce[f] * PrDistce[f] - PlayVx[f] * PlayVx[f]));

						if (Skill == 16) PlayersData[f][12] = 2; //Move 决定移动
						if (PlayersData[f][10] == 0 || (4 <= Skill && Skill <= 6) || Skill == 9 || Skill == 12 || Skill == 14)
							PlayersData[f][9] = PlayersData[f][9] - Inj;
						//Data[9] 生命值 [10]护盾值  直接扣生命值
						else
						{
							if (Skill == 3) {
								Inj = Inj * 1.3;
								TEXTOUT(L"破甲!", PlayersData[f][1], PlayersData[f][2] - 30, 1, L"华文新魏", 0, 0, 255);
							}

							if (Skill == 21 && Inj < 150) {
								Inj = Inj * 0.8;
								TEXTOUT(L"未穿甲", PlayersData[f][1] - 5, PlayersData[f][2] - 20, 1, L"华文新魏", 200, 80, 160);
							}

							if (PlayersData[f][10] >= Inj)
								PlayersData[f][10] = PlayersData[f][10] - Inj;
							else {
								PlayersData[f][9] += PlayersData[f][10] - Inj;
								PlayersData[f][10] = 0;
							}
						}

						if ((Inj - (int)Inj) * 100 > 0) {
							if ((Inj - (int)Inj) * 10 > 0)
								sprintf(str, "%.1f", Inj);
							else
								sprintf(str, "%.2f", Inj);
						}
						else
							sprintf(str, "%.0f", Inj);

						if (Inj < 0) sprintf(str, "什么血包：%f", Inj);
						if (Inj > 2000) sprintf(str, "什么爆炸伤害：%f", Inj);

						//为什么就天姬伤害显示会漂移
						TEXTOUT(PATHcTow(str), PlayersData[f][1] + 8 - textwidth(PATHcTow(str)) / 2.0, PlayersData[f][2] - 10, 2, L"华文新魏", 150, 50, 0);
					}

				//绚烂线条
				for (int i = 1; i <= Players; i++)	//绘画每个精彩的线条
					if (PrDistce[i] <= 24 + r)
					{
						float PrDistceTemp[12]{ 0 };
						for (int f = 1; f <= Players; f++)
						{
							PrDistceTemp[f] = PrDistce[f] / 10;
							if (PrDistceTemp[f] >= 2.4) PrDistceTemp[f] = 2.4;
						}

						if (Skill == 21)
							Inj += 62;

						for (int f = 0; f <= (int)Inj * 2 / 25; f++)
						{
							setlinecolor(RGB(255 - (int)(PrDistceTemp[i] * 110), 50, (int)(PrDistceTemp[i] * 110)));
							line(PlayersData[i][1] + 8, PlayersData[i][2] + 8,
								2 * PlayersData[i][1] + 24 - xp + RANDOM(-Inj / 125, Inj / 125) * 7, 2 * PlayersData[i][2] + 24 - yp + RANDOM(-Inj / 125, Inj / 125) * 7);
						}
					}

				//让半空中悬停的你即使没被击中也飞起来
				for (int f = 1; f <= Players; f++) {

					if (getpixel(PlayersData[f][1] + 2, PlayersData[f][2] + 16 + 2) != Block &&
						getpixel(PlayersData[f][1] + 16 - 2, PlayersData[f][2] + 16 + 2) != Block &&
						getpixel(PlayersData[f][1] + 8, PlayersData[f][2] + 16 + 2) != Block &&
						PrDistce[f] > 24 + r)
						PlayVy[f] = -0.001;
					if (PrDistce[f] == 999) { PlayVy[f] = -15; PlayVx[f] = 0; }
					if (Skill == 11 && PrDistce[f] <= 24 + r)
					{
						PlayVy[f] = -20; PlayVx[f] = Dir * 3;
					}
					if (Skill == 14 && PrDistce[f] <= 24 + r)
					{
						PlayVy[f] = -20; PlayVx[f] = Dir * 2;
					}
				}

				//弹弹弹烫烫烫跳跳跳
				for (int i = 1; i <= Players; i++)
					if (PrDistce[i] <= 24 + r || Skill == 11)
					{
						int Rep = 0;
						BOOL PlayRep[12]{ 0 };

						while (Rep < Players)
						{
							Rep = 0;
							for (int f = 1; f <= Players; f++)
								if (fabs(PlayVy[f]) <= 0.001 || PlayersData[f][2] >= 300) {
									PlayRep[f] = 1;
									TEXTOUT(PlayerChar[f - 1], PlayersData[f][1], PlayersData[f][2], 3, L"幼圆", PlayersData[f][3], PlayersData[f][4], PlayersData[f][5]);
								}
								else
								{
									PlayRep[f] = 0;
									PFlyXt[f] += 0.01;
									PFlyYt[f] += 0.01;

									TEXTOUT(PlayerChar[f - 1], PlayersData[f][1], PlayersData[f][2], 3, L"幼圆", 255, 255, 255);
									PlayersData[f][1] += PlayVx[f] * PFlyXt[f];
									PlayersData[f][2] += PlayVy[f] * PFlyYt[f] + 50 * PFlyYt[f] * PFlyYt[f];

									if (getpixel(PlayersData[f][1] + 18, PlayersData[f][2] + 8) == Block && PFlyXt[f] >= 0.05) { PlayVx[f] = -fabs(PlayVx[f]);	PFlyXt[f] = 0; }
									if (getpixel(PlayersData[f][1] - 2, PlayersData[f][2] + 8) == Block && PFlyXt[f] >= 0.05) { PlayVx[f] = 0.95 * fabs(PlayVx[f]);		PFlyXt[f] = 0; }
									if (getpixel(PlayersData[f][1] + 8, PlayersData[f][2] - 2) == Block && PFlyYt[f] >= 0.05) { PlayVy[f] = 0.95 * fabs(PlayVy[f]);		PFlyYt[f] = 0; }
									if (getpixel(PlayersData[f][1] + 2, PlayersData[f][2] + 16 + 2) == Block ||
										getpixel(PlayersData[f][1] + 16 - 2, PlayersData[f][2] + 16 + 2) == Block ||
										getpixel(PlayersData[f][1] + 8, PlayersData[f][2] + 18) == Block)
									{
										PlayVx[f] = 0.85 * PlayVx[f];

										if (Skill != 11 || PFlyYt[f] >= 0.05) {
											PlayVy[f] = -0.5 * fabs(PlayVy[f]);
											if (Skill == 11)
												PlayVy[f] = 0;
										}
										PFlyYt[f] = 0;

									}
									TEXTOUT(PlayerChar[f - 1], PlayersData[f][1], PlayersData[f][2], 3, L"幼圆", PlayersData[f][3], PlayersData[f][4], PlayersData[f][5]);

								}
							for (int f = 1; f <= Players; f++)
								Rep += PlayRep[f];

							delay(8);
						}

						break;
					}

				//显示最终加的护盾值
				if (Skill == 12 && Shields > 0 && SkillRep == 20)
				{
					sprintf(str, "+%d", Shields);
					TEXTOUT(PATHcTow(str), PlayersData[Play][1] - textwidth(PATHcTow(str)) / 2, PlayersData[Play][2] - 24, 3, L"幼圆", 100, 100, 255, 255, 255, 255);
					Sleep(100);
				}

				Sleep(100);
				LoadMap();
				if (Skill != 9 && Skill != 11)
					clearcircle(ShellmidX, ShellmidY, 20 + r);
				SaveAnSave();
				if (Auto > 0 && View == 0 && Storge > 0) {
					sprintf(str, "%s\\SaveMap\\%s.png", AllPath, SaveName);
					CopyFile(PATHcTow(PATH), PATHcTow(str), FALSE);
				}
				//地雷飞飞飞
				if (View == 0)
					for (int i = 1; i <= Mines; i++)
						if (MinesArry[3 * i - 2] == 1)
						{
							float MineX = MinesArry[3 * i - 1];
							float MineY = MinesArry[3 * i];

							float distance = sqrt(pow(MineX + 6 - ShellmidX, 2) + pow(MineY + 6 - ShellmidY, 2));

							if (distance <= 22 + r)
							{
								float FlyXt = 0; //水平飞时间
								float FlyYt = 0; //竖直飞时间
								float FlyXv = 0.1 * (MineX + 6 - ShellmidX);
								float FlyYv = -0.2 * sqrt(fabs(distance * distance - FlyXv * FlyXv));//下面这一坨屎原代码，distance用做起飞竖直速度
								//M9(12,4*Z-2):=√(ABS(.9*d1^2-M9(12,4*Z-3)^2));
								//-0.8*(MineY+6-ShellmidY);

								delay(20);
								LoadMap();

								//显示轰炸
								if (v0 == 42) {
									TEXTOUT(L"轰炸中...正在: ", -map_wide / 2 + 20, map_high / 2 + 30, 3, L"幼圆", 255, 50, 120, 255, 255, 255);
									sprintf(str, "%d", times);
									TEXTOUT(PATHcTow(str), -map_wide / 2 + 20 + 100, map_high / 2 + 30, 3, L"幼圆", 255, 50, 120, 255, 255, 255);
								}

								TEXTOUT(L"累伤: ", -map_wide / 2 + 20, map_high / 2 + 10, 2, L"幼圆", 220, 60, 80, 255, 255, 255);

								for (int i = 1; i <= Players; i++)
									if (PlayersData[i][9] > 0)
										TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);

								while (fabs(FlyXv > 0.2) || fabs(FlyYv) > 0.2)
								{
									FlyXt += 0.01;
									FlyYt += 0.01;

									TEXTOUT(L"雷", MineX, MineY, 1, L"幼圆", 255, 255, 255);

									MineX += FlyXv * FlyXt;
									MineY += FlyYv * FlyYt + 6 * FlyYt * FlyYt;

									if (getpixel(MineX + 14, MineY + 6) == Block) { FlyXv = -fabs(0.85 * FlyXv);	FlyXt = 0; }
									if (getpixel(MineX - 2, MineY + 6) == Block) { FlyXv = fabs(0.85 * FlyXv);	FlyXt = 0; }
									if (getpixel(MineX + 6, MineY + 14) == Block) { FlyYv = -fabs(RANDOM(0.85, 0.95) * FlyYv);  FlyXv = 0.88 * FlyXv;	FlyYt = 0; }
									if (getpixel(MineX + 13, MineY + 13) != Block && fabs(FlyXv) <= 1 && fabs(FlyYv) <= 1)
									{
										FlyXv = 3;	FlyXt = 0;
									}
									if (getpixel(MineX - 1, MineY + 13) != Block && fabs(FlyXv) <= 1 && fabs(FlyYv) <= 1)
									{
										FlyXv = -3;	FlyXt = 0;
									}
									if (MineY >= map_high / 2.0 + 15) { MinesArry[3 * i - 2] = 0; break; }

									TEXTOUT(L"雷", MineX, MineY, 1, L"幼圆", 50, 30, 60);
									delay(4);
								}
							}
							while (getpixel(MineX + 1, MineY + 14) != Block && getpixel(MineX + 11, MineY + 14) != Block)
							{
								TEXTOUT(L"雷", MineX, MineY, 1, L"幼圆", 255, 255, 255);
								MineY += 1;
								if (MineY >= map_high / 2.0 + 20) { MinesArry[3 * i - 2] = 0; break; }
								TEXTOUT(L"雷", MineX, MineY, 1, L"幼圆", 50, 30, 60);

							}
							MinesArry[3 * i - 1] = MineX;
							MinesArry[3 * i] = MineY;
						}
			}

			//+升天+ +升天+
			for (int i = 1; i <= Players; i++)
			{
				int Flyyy = 0;
				while ((PlayersData[i][9] <= 0 || PlayersData[i][2] >= map_high / 2.0) && PlayersData[i][2] != map_high / 2.0 + 999)
				{
					Prime_RECT(PlayersData[i][1] - 21, PlayersData[i][2] + 15, PlayersData[i][1] + 40, PlayersData[i][2] + 18);
					TEXTOUT(L"+  +", PlayersData[i][1] - 6, PlayersData[i][2] - 18, 3, L"幼圆", 200, 220, 50, 255, 255, 255);
					TEXTOUT(L"翅     翅", PlayersData[i][1] - 20 + Flyyy % 3, PlayersData[i][2] - 5, 1, L"幼圆", 180, 190, 255, 255, 255, 255);
					TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5], 255, 255, 255);
					PlayersData[i][2] -= 1.0;
					Flyyy++;
					if (PlayersData[i][2] <= -map_high / 2.0 - 20)
					{
						if (PlayersData[i][9] > 0)
						{

							PlayersData[i][1] = RANDOM(-map_wide / 2.0 - 30, map_wide / 2.0 - 30);
							PlayersData[i][2] = -map_high / 2.0 - 20;
						}
						else {
							PlayersData[i][1] = 0;
							PlayersData[i][2] = map_high / 2.0 + 999;
						}

						break;

					}
					delay(5);
				}
			}

			int Alive = 0;
			for (int i = 1; i <= Players; i++)
				if (PlayersData[i][9] > 0) Alive++;
			if (Alive <= 1) { Play = -2; break; }

			//地雷爆爆爆!!
			if (Round >= Players)
			{
				for (int i = 1; i <= Mines; i++)
					if (MinesArry[3 * i - 2] == 1)
					{
						float MineX = MinesArry[3 * i - 1];
						float MineY = MinesArry[3 * i];

						for (int f = 1; f <= Players; f++)
							if (fabs(MineX + 6 - PlayersData[f][1] - 8) <= 16 && fabs(MineY + 6 - PlayersData[f][2] - 8) <= 16 && PlayersData[f][9] > 0)
							{
								TEXTOUT(L"雷", MineX, MineY, 1, L"幼圆", 255, 255, 255);
								Sleep(200);
								Inj = 150; r = -4.1; x0 = MineX - 2; y0 = MineY + 6;
								Bomb = Repeat = 1; key = 30; Skill = 0;
								MinesArry[3 * i - 2] = 0; MinesArry[3 * i - 1] = -999; MinesArry[3 * i] = -999;
								goto SkillBack;
							}
					}
			}

			//爆炸补给包
			if (数据[10][7] == -1) {
				数据[10][7] = 0;

				Inj = 250;  r = 6.1;
				x0 = 数据[10][9];  y0 = 数据[10][10] + 7;
				Skill = 0;  Repeat = Bomb = 1;

				goto SkillBack;
			}

			//爆炸炮台
			if (数据[10][14] <= 0 && 数据[10][17] == -1)
			{
				数据[10][17] = 0;

				Inj = 220;  r = 6.1;
				x0 = 数据[10][15] - 8;  y0 = 数据[10][16] + 8;
				Skill = 0;  Repeat = Bomb = 1;

				for (int f = 0, InBomb = 500; f <= (int)InBomb * 2 / 25; f++)
				{
					setlinecolor(RGB(255, 50, 0));
					line(数据[10][15], 数据[10][16],
						数据[10][15] + RANDOM(-InBomb / 125, InBomb / 125) * 7, 数据[10][16] + RANDOM(-InBomb / 125, InBomb / 125) * 7);
				}

				Sleep(60);
				goto SkillBack;

			}

			//技能回卷
			Skill = (int)FireTemp[0];

			//散装弹记录第一次位置
			if (Skill == 19 && SkillRep == 1) { FireTemp[4] = xp; FireTemp[5] = yp; }

			//技能回溯
			if ((Skill == 4 && SkillRep == 1) ||
				(Skill == 7 && SkillRep <= 8) ||
				(Skill == 12 && SkillRep <= 19) ||
				(Skill == 14 && FireTemp[6] < 5000) ||
				(Skill == 19 && SkillRep <= 6) ||
				(Skill == 21 && SkillRep <= 8))
				goto SkillBack;

			//轰炸
			if ((Round == 70 || Round == 100 || Round == 120) && times <= 30)
			{
				x0 = RANDOM(-UIleft, UIleft);
				y0 = UItop - 20;
				times++;
				Skill = 0;
				a = 0;  Inj = 135;  r = 0;
				Sigma = -90;  v0 = 42;
				Bomb = 1;  Repeat = 0;
				if (Round == 100) { r = 2; Inj = 145; }
				if (Round == 120) { r = 5; Inj = 160; }

				goto SkillBack;
			}

			//换回合必要初始化
			{
				//机枪记录位置
				if (数据[10][14] > 0 && (SkillRep >= 1 || times >= 1 || Skill == 15))	//828祝毕业#DE6DA9 #42CEDDh 编辑于828
					for (int i = 1; i <= Players; i++)
						if (PlayersData[i][9] > 0)
							数据[21][i] = TRUNCATE(fabs(数据[i][1] - 数据[i][2]), 2);  //你动关我枪什么事
						else
							数据[21][i] = -1;

				//必要初始化
				Skill = (int)FireTemp[0];
				if (1 <= Skill)
				{
					PlayersData[Play + 10][Skill]--;
					PlayersData[Play][8] -= Cost;
				}

				if (Skill == -1)
					TEXTOUT(L"+5oil", PlayersData[Play][1] - 12, PlayersData[Play][2] - 12, 2, L"幼圆", 255, 255, 255);

				Repeat = 0; Bomb = 1;  JUmp = FALSE;  数据[10][17] = 0;//炮台发射状态归位
				Inj = 125; r = 0;  Sigma = 45;  v0 = 10;  key = 0;  RunTime = 0;  SkillKey = -99;
				Cost = 0; Skill = 0; times = 0; SkillGroup = 0;  VxVy = TRUE; FeedKey = 0;
				if (Auto < 0)
					Auto = 0;
				PlayersData[10][11] = 0;  //Skill
				PlayersData[10][12] = 0;  //Cost
				for (int i = 0; i < sizeof(FireTemp) / sizeof(FireTemp[0]); i++)
					FireTemp[i] = 0;

				//换回合初始化与存储数据
				if (View == 0)
				{
					//----------------变量初始化--------------
					for (int i = 1; i <= Players; i++) PrDistce[i] = 0;  //伤害距离
					a = round(RANDOM(0, 4) * 100.0) / 100.0;
					Direct = round(RANDOM(1, 2));
					if (Direct >= 1.5) a = -a;
					if (Round == PlayersData[10][8])
					{
						PlayersData[10][7] = (int)RANDOM(1, 9.01);//整9概率究极小，平均概率低于0.2%,这不比O0概率低
						PlayersData[10][8] += (int)RANDOM(4, 9);
						TEXTOUT(L"包", PlayersData[10][9], PlayersData[10][10], 2, L"幼圆", 255, 255, 255);
						PlayersData[10][9] = RANDOM(UIleft + 10, -UIleft - 20);
						PlayersData[10][10] = UItop - 20;
						if (Storge > 0)
						{
							Datafile = fopen(Data, "a");
							if (Datafile == NULL)
								Storge = -1;

							if (Storge > 0)
							{
								fprintf(Datafile, "Pk:");
								for (int i = 7; i <= 10; i++)
									if (floNum(PlayersData[10][i]) == 0)
										fprintf(Datafile, "%.0f,", PlayersData[10][i]);
									else
										fprintf(Datafile, "%f,", PlayersData[10][i]);
								fprintf(Datafile, "Pk;");

								fclose(Datafile);
							}
						}
					}

					//包的位置同步还有问题，查一下原因

					//存储数据至Data数组
					{
						PlayersData[10][1] = Round;
						PlayersData[10][2] = Play;
						PlayersData[10][3] = Players;
						PlayersData[10][4] = OnlOrLoc;
						PlayersData[10][5] = a;
						PlayersData[10][6] = Direct;
					}

					//联机模式下再次同步
					if (OnlOrLoc == 1)
					{
						if (selfround == Play)
						{
							Sleep(80);
							sprintf(str, "%s", "FireEnd");
							ret = send(serverSocket, (char*)str, strlen(str), 0);

							Sleep(50);
							ret = send(serverSocket, (char*)PlayersData, sizeof(PlayersData), 0);
							if (ret <= 0) {
								CloseAndMenu(); Play = -1; break;
							}

							Sleep(20);
							ret = send(serverSocket, (char*)MinesArry, sizeof(MinesArry), 0);
							if (ret <= 0) {
								CloseAndMenu(); Play = -1; break;
							}

						}
						else
						{
							ret = recv(serverSocket, (char*)PlayersData, sizeof(PlayersData), 0);
							ret = recv(serverSocket, (char*)MinesArry, sizeof(MinesArry), 0);

							//转换数组存储
							{
								Round = PlayersData[10][1];
								Play = PlayersData[10][2];
								Players = PlayersData[10][3];
								OnlOrLoc = PlayersData[10][4];
								a = PlayersData[10][5];
								Direct = PlayersData[10][6];
							}

						}
					}

					//回合
					do {
						Play++;
						if (OnlOrLoc == 0)
						{
							selfround++;
							if (selfround == Players + 1) selfround = 1;
						}
						if (Play == Players + 1) Play = 1;
					} while (PlayersData[Play][9] <= 0);

					int Alive = 0;
					for (int i = 1; i <= Players; i++)
						if (PlayersData[i][9] > 0) Alive++;
					if (Alive <= 1) { Play = -2; break; }

					//闪烁只是
					{
						int Bling = 0;
						const TCHAR* POi = L"↓";

						if (数据[Play][2] <= UItop - 20) {
							POi = L"↑";
							X = 数据[Play][1] - 8;
							Y = UItop;
							Bling = 5;
						}

						if (数据[Play][1] <= UIleft - 20)
						{
							POi = L"←";
							Bling = 1;
							X = UIleft;
							Y = 数据[Play][2] + 8;

							if (数据[Play][2] <= UItop - 20) {
								POi = L"↖";
								Bling = 2;
								X = UIleft;
								Y = UItop;
							}
						}

						if (-UIleft + 20 <= 数据[Play][1])
						{
							POi = L"→";
							Bling = 3;
							X = -UIleft - 20;
							Y = 数据[Play][2] + 8;

							if (数据[Play][2] <= UItop - 20) {
								POi = L"↗";
								Bling = 4;
								X = -UIleft - 35;
								Y = UItop;
							}
						}

						//闪烁指示
						for (int i = 1; i <= 2; i++)
						{
							if (Bling == 0) {
								TEXTOUT(L"↓", 数据[Play][1] - 8, 数据[Play][2] - 30, 10);
								TEXTOUT(PlayerChar[Play - 1], PlayersData[Play][1], PlayersData[Play][2], 3, L"幼圆", 255, 255, 255);
								Sleep(150);

								TEXTOUT(L"↓", 数据[Play][1] - 8, 数据[Play][2] - 30, 10, L"幼圆", 255, 255, 255);
								TEXTOUT(PlayerChar[Play - 1], PlayersData[Play][1], PlayersData[Play][2], 3, L"幼圆", PlayersData[Play][3], PlayersData[Play][4], PlayersData[Play][5]);
								Sleep(100);

							}
							else
							{
								TEXTOUT(POi, X, Y, 10);
								Sleep(150);

								TEXTOUT(POi, X, Y, 10, L"幼圆", 255, 255, 255);
								Sleep(100);

							}
						}
					}

					Round++;
					if (Play == Players + 1) Play = 1;
					x0 = PlayersData[Play][1];
					y0 = PlayersData[Play][2];

					//----------------玩家回合初始化--------------

					//PlayersData[Play][6] -> 总油量	PlayersData[Play][6] -> 当前油量
					if (PlayersData[Play][6] + PlayersData[Play][7] > 10 || PlayersData[Play][6] >= 10)
					{
						PlayersData[Play][6] = PlayersData[Play][6] + PlayersData[Play][7] - 15;
						PlayersData[Play][7] = 15;
					}
					else
					{
						PlayersData[Play][6] = PlayersData[Play][6] + PlayersData[Play][7];
						if (PlayersData[Play][6] < 15)	PlayersData[Play][6] = 0;
					}

					PlayersData[Play][8]++;		//->技力点

					//----------------玩家回合初始化end--------------

					//存储数据至Data数组
					{
						PlayersData[10][1] = Round;
						PlayersData[10][2] = Play;
						PlayersData[10][3] = Players;
						PlayersData[10][4] = OnlOrLoc;
						PlayersData[10][5] = a;
						PlayersData[10][6] = Direct;
					}

					LoadMap();
					drawUI();
					DrawMan();

					//画地雷
					for (int i = 1; i <= Mines; i++)
						if (MinesArry[3 * i - 2] == 1)
							TEXTOUT(L"雷", MinesArry[3 * i - 1], MinesArry[3 * i], 1, L"幼圆", 50, 30, 60);

					if (70 - Players <= Round && Round <= 70)
						sprintf(str, "剩余%d回合轰炸", 70 - Round);
					if (100 - Players <= Round && Round <= 100)
						sprintf(str, "剩余%d回合轰炸", 100 - Round);
					if (120 - Players <= Round && Round <= 120)
						sprintf(str, "剩余%d回合轰炸", 120 - Round);
					if (70 - Players <= Round && Round <= 120 && strncmp(str, "剩余", 2) == 0)
						TEXTOUT(PATHcTow(str), UIleft + 200, UItop, 5, L"幼圆", 255, 50, 100);

					if (Storge > 0)
						Storge = SaveData(Storge, ROWS, COLS, Round);
				}
				else
					for (int i = 1; i <= Players; i++)
						PlayersData[i][9] = 4800;

				//受伤归0和检测其他杂项
				for (int i = 1; i <= Players; i++)
					if (PlayersData[i][9] > 0)
					{
						AllHit[i] = 0;
						TEXTOUT(PlayerChar[i - 1], PlayersData[i][1], PlayersData[i][2], 3, L"幼圆", PlayersData[i][3], PlayersData[i][4], PlayersData[i][5]);

						if (数据[i][12] >= 1)
							数据[i][12]--;

						if (数据[i][12] != 0)
						{
							setlinecolor(RGB(255, 60, 255));
							arc(数据[i][1] + 8 - 10, 数据[i][2] + 8 - 10, 数据[i][1] + 8 + 10, 数据[i][2] + 8 + 10, -40 * (pi / 180), 220 * (pi / 180));
							line(数据[i][1], 数据[i][2], 数据[i][1] + 16, 数据[i][2] + 16);
							line(数据[i][1], 数据[i][2] + 16, 数据[i][1] + 16, 数据[i][2]);
						}
					}

				for (int i = 0; i < sizeof(RecvBuffer); i++)	RecvBuffer[i] = '\0';
				for (int i = 0; i <= 3; i++) keystr[i] = '!';
			}

		}	//press enter end

		//技能按键判定
		if ((selfround == Play && (SkillKey == 8 || SkillKey == '*' || SkillKey == '.' || (-22 <= SkillKey && SkillKey <= 1) || ('/' <= SkillKey && SkillKey <= '9'))) || (selfround != Play && RecvBuffer[0] == '_'))
		{
			//分割技能组
			if (OnlOrLoc == 1)
			{
				if (selfround == Play)
				{
					sprintf(RecvBuffer, "_%d", SkillKey);
					send(serverSocket, RecvBuffer, strlen(RecvBuffer), 0);
				}
				else if (RecvBuffer[0] == '_')
				{
					for (int i = 0; i < strlen(RecvBuffer) - 1; ++i)
						RecvBuffer[i] = RecvBuffer[i + 1];
					RecvBuffer[strlen(RecvBuffer) - 1] = '\0';

					SkillKey = atoi(RecvBuffer);
				}

			}

			//退格键取消技能
			if (SkillKey == 8 || SkillKey == '*' || SkillKey == '/')
			{
				int Txtleft = map_wide / 2 - 210 + 115;
				int Txttop = -map_high / 2 + 55;

				if (SkillKey == '*' || SkillKey == '/')  //乘号变换技能组
				{
					if (SkillKey == '*')
						SkillGroup++;
					else
						SkillGroup--;
					if (SkillGroup >= 3) SkillGroup = 0;
					if (SkillGroup <= -1) SkillGroup = 2;
					TEXTOUT(L"(组 )", Txtleft - 40, Txttop, 3, L"幼圆", 100, 100, 100, 255, 255, 255);
					sprintf(str, "%d", SkillGroup);
					TEXTOUT(PATHcTow(str), Txtleft - 15, Txttop, 3);

				}

				Skill = 0;
				Cost = 0;

				button(L"当前技能:", getwidth() - 210, 55, 65, 20, 3, L"等线", 0, 0, 100, 20, 200, 255);
				setorigin(getwidth() / 2, getheight() / 2);
				//TEXTOUT(L"当前技能:", Txtleft-115, Txttop, 4, L"等线", 0, 0, 100, 255, 255, 255);
				Prime_RECT(Txtleft, Txttop, map_wide / 2, Txttop + 16);

				Sleep(150);

			}
			if (('0' <= SkillKey && SkillKey <= '9') || SkillKey == 46 || (-22 <= SkillKey && SkillKey <= 1))
			{	//数字0-9
				int Txtleft = map_wide / 2 - 210 + 115;
				int Txttop = -map_high / 2 + 55;

				TEXTOUT(L"(组 )", Txtleft - 40, Txttop, 3, L"幼圆", 100, 100, 100, 255, 255, 255);
				sprintf(str, "%d", SkillGroup);
				TEXTOUT(PATHcTow(str), Txtleft - 15, Txttop, 3);

				if (SkillKey == 46)
					SkillKey++;//偏移一位

				//技能组
				if (-22 <= SkillKey && SkillKey <= 1)
					SkillKey = -SkillKey;
				else
					SkillKey = (SkillKey - 48) + SkillGroup * 10;

				Prime_RECT(Txtleft, Txttop, map_wide / 2, Txttop + 16);
				switch (SkillKey)
				{
					/*
					灰色  平均即可       不消耗技力
					蓝色  50,  100, 200  护盾(直接间接)相关
					红色  255, 0,   0    血量(直接间接)相关(或者是字符级技能)
					橙色  255, 150, 0    炮弹相关(按下直接发射)
					绿色  60,  235, 55   其他相关(按下非直接发射)
					紫色  255, 100, 200  非伤害技能
					*/
				case -1:
					TEXTOUT(L"血包", Txtleft, Txttop, 3, L"幼圆", 240, 80, 80, 255, 255, 255);
					SkillKey = -1;
					Cost = -2;
					break;

				case 0: //跳回合
					TEXTOUT(L"跳回合", Txtleft, Txttop, 3, L"幼圆", 180, 180, 160, 255, 255, 255);
					Cost = -1;
					break;

				case 1: //伤害提升
					TEXTOUT(L"伤害提升", Txtleft, Txttop, 3, L"幼圆", 255, 130, 200, 255, 255, 255);
					Cost = 3;
					break;

				case 2: //进行瞬移
					TEXTOUT(L"缓慢瞬移", Txtleft, Txttop, 3, L"幼圆", 255, 120, 200, 255, 255, 255);
					Cost = 2;
					break;

				case 3: //皇嘉穿甲弹
					TEXTOUT(L"皇嘉穿甲弹", Txtleft, Txttop, 3, L"幼圆", 255, 100, 200, 255, 255, 255);
					Cost = 4;
					break;

				case 4: //战姬轰炸
					TEXTOUT(L"战姬轰炸", Txtleft, Txttop, 3, L"幼圆", 255, 80, 50, 255, 255, 255);
					Cost = 5;
					break;

				case 5: //天姬,参上
					TEXTOUT(L"天姬,参上", Txtleft, Txttop, 3.5, L"方正姚体", 255, 0, 0, 255, 255, 255);
					Cost = 10;
					break;

				case 6: //爆炸飞羊
					TEXTOUT(L"爆炸飞羊", Txtleft, Txttop, 3.5, L"华文新魏", 255, 0, 50, 255, 255, 255);
					Cost = 8;
					break;

				case 7: //Fox3飞弹
					TEXTOUT(L"Fox3飞弹", Txtleft, Txttop, 3, L"幼圆", 255, 150, 0, 255, 255, 255);
					Cost = 7;
					break;

				case 8: //护盾充能
					TEXTOUT(L"护盾充能", Txtleft, Txttop, 3, L"幼圆", 50, 100, 200, 255, 255, 255);
					Cost = 2;
					break;

				case 9: //字符斩
					TEXTOUT(L"字符斩！！", Txtleft, Txttop, 5, L"华文楷体", 255, 0, 0, 255, 255, 255);
					Cost = 10;
					break;

				case 10: //移动到平台
					TEXTOUT(L"移动到平台", Txtleft, Txttop, 3, L"幼圆", 255, 100, 200, 255, 255, 255);
					Cost = 3;
					break;

				case 11: //上勾拳
					TEXTOUT(L"上勾拳! ", Txtleft, Txttop, 3, L"幼圆", 255, 40, 100, 255, 255, 255);
					Cost = 2;
					break;

				case 12: //裁雨留虹
					TEXTOUT(L"裁雨留虹", Txtleft, Txttop, 3, L"幼圆", 50, 200, 200, 255, 255, 255);
					Cost = 6;
					break;

				case 13: //抛物线保护
					TEXTOUT(L"抛射物保护", Txtleft, Txttop, 3, L"幼圆", 30, 80, 255, 255, 255, 255);
					Cost = 2;
					break;

				case 14: //南蛮冲撞
					TEXTOUT(L"南蛮冲撞", Txtleft, Txttop, 3, L"幼圆", 200, 90, 200, 255, 255, 255);
					Cost = 6;
					break;

				case 15: //堡垒机关炮
					TEXTOUT(L"堡垒机关炮", Txtleft, Txttop, 3, L"幼圆", 255, 0, 255, 255, 255, 255);
					Cost = 6;
					break;

				case 16: //电弧陷阱
					TEXTOUT(L"电弧陷阱", Txtleft, Txttop, 3, L"幼圆", 60, 255, 200, 255, 255, 255);
					Cost = 3;
					break;

				case 17: //钻钻钻钻井
					TEXTOUT(L"钻钻钻钻井", Txtleft, Txttop, 3, L"幼圆", 255, 46, 46, 255, 255, 255);
					Cost = 4;
					break;

				case 18: //半程追踪弹
					TEXTOUT(L"半程追踪弹", Txtleft, Txttop, 3, L"幼圆", 60, 200, 100, 255, 255, 255);
					Cost = 4;
					break;

				case 19: //散装弹
					TEXTOUT(L"散装弹", Txtleft, Txttop, 3, L"幼圆", 255, 150, 50, 255, 255, 255);
					Cost = 6;
					break;

				case 20: //战墙
					TEXTOUT(L"战墙", Txtleft, Txttop, 3, L"幼圆", 255, 100, 200, 255, 255, 255);
					Cost = 2;
					break;

				case 21: //充能左轮枪
					TEXTOUT(L"充能双发枪", Txtleft, Txttop, 3, L"幼圆", 50, 210, 255, 255, 255, 255);
					Cost = 4;
					break;

				case 22: //爆炸网球
					TEXTOUT(L"爆炸网球", Txtleft, Txttop, 3, L"幼圆", 60, 245, 55, 255, 255, 255);
					Cost = 4;
					break;

				default:
					SkillKey = 0;
					Cost = -1;
					break;
				}

				if ((PlayersData[Play][8] < Cost && Cost > 0) || (PlayersData[Play][12] != 0 && Cost <= -1) || PlayersData[Play + 10][SkillKey + 1] == 0)
				{
					if (PlayersData[Play][8] < Cost)
						button(L"点数不足", getwidth() - 210, 55, 65, 20, 3, L"幼圆", 150, 50, 50, 255, 255, 255);
					if (PlayersData[Play][12] != 0)
						button(L"不能使用", getwidth() - 210, 55, 65, 20, 3, L"幼圆", 150, 50, 50, 255, 255, 255);
					if (PlayersData[Play + 10][SkillKey] == 0)
						button(L"次数不足", getwidth() - 210, 55, 65, 20, 3, L"幼圆", 150, 50, 50, 255, 255, 255);
					setorigin(getwidth() / 2, getheight() / 2);

					/*
					if (PlayersData[Play][8] < Cost)
						TEXTOUT(L"点数不足", Txtleft-112, Txttop+1, 3.5, L"幼圆", 150, 50, 50, 255, 255, 255);
					if (PlayersData[Play][12] != 0)
						TEXTOUT(L"不能使用", Txtleft-112, Txttop+1, 3.5, L"幼圆", 150, 50, 50, 255, 255, 255);
					if (PlayersData[Play+10][SkillKey] == 0)
						TEXTOUT(L"次数不足", Txtleft-112, Txttop+1, 3.5, L"幼圆", 150, 50, 50, 255, 255, 255);
						*/
					Cost = 0;
				}
				if (Cost == 0)    Skill = 0;//归0，否则存Skill
				else
				{
					button(L"当前技能:", getwidth() - 210, 55, 65, 20, 3, L"等线", 0, 0, 100, 20, 200, 255);
					setorigin(getwidth() / 2, getheight() / 2);

					//TEXTOUT(L"当前技能:", Txtleft-115, Txttop, 4, L"等线", 0, 0, 100, 255, 255, 255);
					Skill = SkillKey;
					if (Cost == -1) { Skill = -1; Cost = 0; }//跳回合
					if (Cost == -2) { Skill = -2; Cost = 0; }//血包
				}

				if (View == 0 && Storge > 0) {
					Datafile = fopen(Data, "a");
					if (Datafile == NULL)
						Storge = -1;

					if (Storge > 0)
						fprintf(Datafile, "Skill:%d,", Skill);
					fclose(Datafile);
				}

				Sleep(100);
			}

			PlayersData[10][11] = Skill;
			PlayersData[10][12] = Cost;
			SkillKey = -99;
		}

		/*
		//跳跃判定
		if ((keystr[0] == '&' || keystr[1] == '&' || keystr[2] == '&' || keystr[3] == '&') &&
			(getpixel(PlayersData[Play][1]+2, PlayersData[Play][2]+16+2) == Block ||
			 getpixel(PlayersData[Play][1]+16-2, PlayersData[Play][2]+16+2) == Block) ||
			 getpixel(PlayersData[Play][1]+8, PlayersData[Play][2]+16+2) == Block)
		{
			JUmp = 1;

			vx = 0;
			vy = 3;//垂直
			jumpt = 0;
			y0 = PlayersData[Play][2];
			key = 0;  for (int i = 0; i <= 3; i++) keystr[i] = '!';

		}
		//跳跃判定
		if (JUmp == TRUE)
		{
			jumpt += 0.01;

			PlayersData[Play][2] = PlayersData[Play][2]-1-vy*jumpt + 5*pow(jumpt, 2);
			TEXTOUT(PlayerChar[Play-1], PlayersData[Play][1], PlayersData[Play][2], 3, L"幼圆", PlayersData[Play][3], PlayersData[Play][4], PlayersData[Play][5], 255, 255, 255);

			//TEXTOUT(PlayerChar[Play], PlayersData[Play][1], PlayersData[Play][2], 3, L"幼圆");

			if (getpixel(PlayersData[Play][1]+2, PlayersData[Play][2]-2)==Block||
				getpixel(PlayersData[Play][1]+16-2, PlayersData[Play][2]-2)==Block||
				getpixel(PlayersData[Play][1]+2, PlayersData[Play][2]+16+2)==Block||
				getpixel(PlayersData[Play][1]+16-2, PlayersData[Play][2]+16+2)==Block ||
				PlayersData[Play][2] >= map_high/2.0)
				JUmp = FALSE;

			delay(1);

		}
		*/

		//操作
		if (keystr[0] != '!')
		{
			x0 = PlayersData[Play][1];
			y0 = PlayersData[Play][2];

			key = 0;
			for (int i = 0; i <= 3; i++) //0124相加排列组合
			{
				if (keystr[i] == 'W' || keystr[i] == 'S' || keystr[i] == 'Q')   key += 2;
				if (keystr[i] == '+' || keystr[i] == '-')						key += 3;
				if (keystr[i] == 'A' || keystr[i] == 'D')						key += 11;
			}

			/*  2  3  11 (真值表大雾)
				2   5    13  16  7  4  15
				3   5    14  16  7
				11 13  14  16  15
			*/

			//WSQ
			if (key == 2 || key == 5 || key == 13 || key == 16 || key == 7 || key == 4 || key == 15)   //W<>119，S<>115，Q<>113,   0124相加排列组合
			{
				setlinecolor(RGB(255, 255, 255));
				line(x0 + 8, y0, x0 + 8 + Dir * 10, y0 - tan(Sigma * pi / 180) * 10);
				if ((keystr[0] == 'W' || keystr[1] == 'W' || keystr[2] == 'W' || keystr[3] == 'W') && Sigma <= 80) Sigma++;
				if ((keystr[0] == 'S' || keystr[1] == 'S' || keystr[2] == 'S' || keystr[3] == 'S') && Sigma >= -60) Sigma--;
				if (keystr[0] == 'Q' || keystr[1] == 'Q' || keystr[2] == 'Q' || keystr[3] == 'Q') { Dir = -Dir; delay(100); }
				if (Sigma < -60 || Sigma > 80) { Sigma = 45;  Dir = -Dir; }
				setlinecolor(RGB(0, 0, 0));
				line(x0 + 8, y0, x0 + 8 + Dir * 10, y0 - tan(Sigma * pi / 180) * 10);
				if (View == 0) {
					sprintf(str, "%d", Sigma);
					TEXTOUT(PATHcTow(str), UIleft + 45 + 100, UItop + 1, 3, L"等线", 255, 255, 255, 120, 120, 120);
				}
				//delay(60);
			}
			//+-
			if (key == 3 || key == 5 || key == 14 || key == 16 || key == 7)  //-<>45，+<>43，	   0124相加排列组合
			{
				if ((keystr[0] == '+' || keystr[1] == '+' || keystr[2] == '+' || keystr[3] == '+') && v0 < 25)  v0++;
				if ((keystr[0] == '-' || keystr[1] == '-' || keystr[2] == '-' || keystr[3] == '-') && v0 > 6)  v0--;
				if (View == 0) {
					Prime_RECT(UIleft + 37, UItop + 20, UIleft + 33 + 150, UItop + 16 + 15);
					Prime_RECT(UIleft + 37, UItop + 20, UIleft + 33 + (v0 - 5) * 7.5, UItop + 16 + 15, v0 * 10, 255 - v0 * 9, v0 * 4);
				}
				//delay(80);
			}
			//AD
			if ((key == 11 || key == 13 || key == 14 || key == 16 || key == 15) && (PlayersData[Play][7] >= 0.1 || View != 0) && 数据[Play][12] < 1)  //A，D，	,   0124相加排列组合
			{
				if (keystr[0] == 'A' || keystr[1] == 'A' || keystr[2] == 'A' || keystr[3] == 'A')  //A
					if (getpixel(PlayersData[Play][1] - 2, PlayersData[Play][2] + 5) != Block && getpixel(PlayersData[Play][1] - 1, PlayersData[Play][2] + 3) != Block)
					{
						PlayersData[Play][7] -= 0.1;
						PlayersData[Play][1] -= 1;
					}

				if (keystr[0] == 'D' || keystr[1] == 'D' || keystr[2] == 'D' || keystr[3] == 'D')  //D
					if (getpixel(PlayersData[Play][1] + 18, PlayersData[Play][2] + 5) != Block && getpixel(PlayersData[Play][1] + 18, PlayersData[Play][2] + 3) != Block)
					{
						PlayersData[Play][7] -= 0.1;
						PlayersData[Play][1] += 1;
					}

				if (getpixel(PlayersData[Play][1] + 2, PlayersData[Play][2] + 16 + 2) == Block ||
					getpixel(PlayersData[Play][1] + 16 - 2, PlayersData[Play][2] + 16 + 2) == Block ||
					getpixel(PlayersData[Play][1] + 8, PlayersData[Play][2] + 16 + 2) == Block)
					PlayersData[Play][2] -= 2.5;

				if (View == 0) {
					Prime_RECT(UIleft + 7, UItop + 20, UIleft + 3 + 20, UItop + 16 + 65);
					Prime_RECT(UIleft + 7, UItop + 20 + (1 - PlayersData[Play][7] / 15) * 60, UIleft + 3 + 20, UItop + 16 + 65, int(255 - PlayersData[Play][7] * 40 / 3), int(PlayersData[Play][7] * 17), int(PlayersData[Play][7] * 20 / 3));
				}

				Prime_RECT(PlayersData[Play][1], PlayersData[Play][2], PlayersData[Play][1] + 15, PlayersData[Play][2] + 16);
				TEXTOUT(PlayerChar[Play - 1], PlayersData[Play][1], PlayersData[Play][2], 3, L"幼圆", PlayersData[Play][3], PlayersData[Play][4], PlayersData[Play][5]);
				if (getpixel(PlayersData[Play][1] + 2, PlayersData[Play][2] + 17) == Block || getpixel(PlayersData[Play][1] + 14, PlayersData[Play][2] + 17) == Block) PlayersData[Play][2] -= 2;
				//delay(10);
			}

			if (View != 0)
			{
				sprintf_s(str, "角度：%d  ", Sigma);
				TEXTOUT(PATHcTow(str), UIleft + 300, -UItop - 15, 2, L"幼圆", 0, 0, 0, 255, 255, 255);
				sprintf_s(str, "力度：%d  ", v0);
				TEXTOUT(PATHcTow(str), UIleft + 400, -UItop - 15, 2, L"幼圆", 0, 0, 0, 255, 255, 255);
			}

			/*
			Prime_RECT(50, 50, 130, 150, 255, 255, 254);
			sprintf_s(str, L"%d", Sigma);
			TEXTOUT(PATHcTow(str), 50, 50);
			sprintf_s(str, L"%d", v0);
			TEXTOUT(PATHcTow(str), 80, 50);
			sprintf_s(str, L"%d", Play);
			TEXTOUT(PATHcTow(str), 100, 50);
			sprintf_s(str, L"%f", TRUNCATE(x0, 1));
			TEXTOUT(PATHcTow(str), 70, 70);
			sprintf_s(str, L"%f", TRUNCATE(y0, 1));
			TEXTOUT(PATHcTow(str), 70, 90);
			outtextxy(50, 70, keystr[0]);
			outtextxy(50, 90, keystr[1]);
			outtextxy(50, 110, keystr[2]);
			outtextxy(50, 130, keystr[3]);
			*/
			key = 0;  for (int i = 0; i <= 3; i++) keystr[i] = '!';
		} //player input
	} //main loop  play=-1>>quit

	//、
	if (Play == -2)
	{
		Sleep(150);
		Prime_RECT();

		X = 0; Y = UItop - 20;
		int WinNer = 0;

		for (int i = 1; i <= Players + 1; i++) {
			if (PlayersData[i][9] > 0)
			{
				WinNer = i;
				break;
			}
			if (i == Players + 1)
				WinNer = -1;
		}

		if (WinNer <= 0)
		{
			WinNer = 1;
			PlayerChar[0] = { L"??" };
		}

		//"奖杯"掉掉掉
		while (true)
		{
			TEXTOUT(L"d b", X - 9, Y, 6, L"黑体", 240, 230, 0);
			TEXTOUT(L"U", X, Y, 10, L"黑体", 240, 230, 0);
			TEXTOUT(L"⊥", X - 3, Y + 18, 6, L"黑体", 240, 230, 0);

			Y++;
			delay(2);

			if (Y >= -100) break;
		}

		Prime_RECT();

		TEXTOUT(L"d b", X - 9, Y, 6, L"黑体", 240, 230, 0);
		TEXTOUT(L"U", X, Y, 10, L"黑体", 240, 230, 0);
		TEXTOUT(L"⊥", X - 3, Y + 18, 6, L"黑体", 240, 230, 0);
		TEXTOUT(L"手  手", X - 9, Y + 20, 1, L"幼圆", 数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5]);
		setlinecolor(RGB(数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5]));
		line(X - 9 + 5, Y + 20 + 10, X, Y + 48);
		line(X - 9 + 30, Y + 20 + 10, X + 15, Y + 48);
		TEXTOUT(PlayerChar[WinNer - 1], X, Y + 40, 3, L"幼圆", 数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5]);

		TEXTOUT(L"开发版本，不代表最终质量，以正式版为准！", UIleft + 250, -UItop - 20);
		TEXTOUT(L"按下Enter以退出程序", UIleft, -UItop - 20);

		unsigned int EndT = 0;

		//颜色数组
		int colors[15][5] = {
	{255, 0, 0},     // 红色
	{255, 64, 0},
	{255, 127, 0},   // 橙色
	{255, 191, 0},
	{255, 255, 0},   // 黄色
	{191, 255, 0},
	{127, 255, 0},
	{64, 255, 0},
	{0, 255, 0},     // 绿色
	{0, 191, 64},
	{0, 127, 127},
	{0, 64, 191},
	{0, 0, 255},     // 蓝色
	{75, 0, 130},    // 靛蓝
	{148, 0, 211}    // 紫色
		};
		int ColW = -1;
		BOOL SHift = 0;
		float INjjjj = 0;

		float POsi[15][7] = { 0 };  //初始X，Y，角度，速度，RGB
		for (int i = 0; i < 14; i++) {
			POsi[i][0] = RANDOM(-40, 30);
			POsi[i][1] = RANDOM(UItop + 50, UItop + 90);
			POsi[i][2] = RANDOM(0, 361) * (pi / 180);
			POsi[i][3] = RANDOM(5, 15);
			POsi[i][4] = (int)RANDOM(0, 256);
			POsi[i][5] = (int)RANDOM(0, 256);
			POsi[i][6] = (int)RANDOM(0, 256);
		}

		while (true)
		{
			EndT++;

			//闪闪闪
			if (EndT % 8 == 0) {
				if (ColW == 15) SHift = 1;
				if (ColW == -1) SHift = 0;

				if (SHift == 0) ColW++; else ColW--;

				TEXTOUT(L"You Are", -40, UItop + 50, 8, L"Unispace", colors[ColW][0], colors[ColW][1], colors[ColW][2]);
				TEXTOUT(L"Charpion", -55, UItop + 80, 10, L"Unispace", colors[ColW][0], colors[ColW][1], colors[ColW][2]);
			}

			//飞飞飞
			for (int i = 0; i < 14; i++)
			{
				TEXTOUT(L"炮", POsi[i][0], POsi[i][1], 3, L"幼圆", 255, 255, 255);

				POsi[i][0] += POsi[i][3] * cos(POsi[i][2]);
				POsi[i][1] -= POsi[i][3] * sin(POsi[i][2]);

				TEXTOUT(L"炮", POsi[i][0], POsi[i][1], 3, L"幼圆", POsi[i][4], POsi[i][5], POsi[i][6]);

				if (UIleft - 20 >= POsi[i][0] || POsi[i][0] >= -UIleft ||
					UItop - 20 >= POsi[i][1] || POsi[i][1] >= -UItop)
				{
					POsi[i][0] = RANDOM(-40, 30);
					POsi[i][1] = RANDOM(UItop + 50, UItop + 90);
					POsi[i][2] = RANDOM(0, 361) * (pi / 180);
					POsi[i][3] = RANDOM(5, 15);
					POsi[i][4] = (int)RANDOM(0, 256);
					POsi[i][5] = (int)RANDOM(0, 256);
					POsi[i][6] = (int)RANDOM(0, 256);
				}

				if (fabs(POsi[i][0] - X - 8) <= 20 && fabs(POsi[i][1] - Y - 20) <= 40)
				{
					if (fabs(POsi[i][0] - X - 8) <= 11 && fabs(POsi[i][1] - Y - 48) <= 11)
					{
						INjjjj += 125 * 1.5;
						TEXTOUT(L"暴击!", X - 5, Y - 30, 2, L"幼圆", 255, 0, 0);
					}
					else
						INjjjj += 125;

					if (INjjjj >= 65535) INjjjj = 65535;

					TEXTOUT(L"炮", POsi[i][0], POsi[i][1], 3, L"幼圆", 255, 255, 255);

					sprintf(str, "%.0f", INjjjj);
					TEXTOUT(PATHcTow(str), X - textwidth(PATHcTow(str)) / 2.0 + 7, Y - 15, 3, L"Unispace", 150, 50, 0, 255, 255, 255);

					POsi[i][0] = RANDOM(-40, 30);
					POsi[i][1] = RANDOM(UItop + 50, UItop + 90);
					POsi[i][2] = RANDOM(0, 361) * (pi / 180);
					POsi[i][3] = RANDOM(5, 15);
					POsi[i][4] = (int)RANDOM(0, 256);
					POsi[i][5] = (int)RANDOM(0, 256);
					POsi[i][6] = (int)RANDOM(0, 256);

					TEXTOUT(L"d b", X - 9, Y, 6, L"黑体", 240, 230, 0);
					TEXTOUT(L"U", X, Y, 10, L"黑体", 240, 230, 0);
					TEXTOUT(L"⊥", X - 3, Y + 18, 6, L"黑体", 240, 230, 0);
					TEXTOUT(L"手  手", X - 9, Y + 20, 1, L"幼圆", 数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5]);
					setlinecolor(RGB(数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5]));
					line(X - 9 + 5, Y + 20 + 10, X, Y + 48);
					line(X - 9 + 30, Y + 20 + 10, X + 15, Y + 48);
					TEXTOUT(PlayerChar[WinNer - 1], X, Y + 40, 3, L"幼圆", 数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5]);

				}
			}

			if (GetAsyncKeyState(Charkey[0]) || GetAsyncKeyState(Charkey[1]) || GetAsyncKeyState(Charkey[2]) || GetAsyncKeyState(Charkey[3]))
			{
				if (GetAsyncKeyState(Charkey[0]))  X -= 2;
				if (GetAsyncKeyState(Charkey[1]))  X += 2;
				if (GetAsyncKeyState(Charkey[2]))  Y -= 2;
				if (GetAsyncKeyState(Charkey[3]))  Y += 2;

				TEXTOUT(L"d b", X - 9, Y, 6, L"黑体", 240, 230, 0, 255, 255, 255);
				TEXTOUT(L"U", X, Y, 10, L"黑体", 240, 230, 0, 255, 255, 255);
				TEXTOUT(L"⊥", X - 3, Y + 18, 6, L"黑体", 240, 230, 0, 255, 255, 255);
				TEXTOUT(L"手  手", X - 9, Y + 20, 1, L"幼圆", 数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5], 255, 255, 255);
				setlinecolor(RGB(数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5]));
				line(X - 9 + 5, Y + 20 + 10, X, Y + 48);
				line(X - 9 + 30, Y + 20 + 10, X + 15, Y + 48);
				TEXTOUT(PlayerChar[WinNer - 1], X, Y + 40, 3, L"幼圆", 数据[WinNer][3], 数据[WinNer][4], 数据[WinNer][5], 255, 255, 255);
			}

			if (GetAsyncKeyState(VK_RETURN) || INjjjj >= 65535)
			{
				if (INjjjj >= 65535) {
					int msgboxID = MessageBox(NULL, L"数值超出最大值！", L"CharWar(假装程序错误)", MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION);
					if (msgboxID != 5)  //选忽略是不能忽略的
						break;
				}

				if (GetAsyncKeyState(VK_RETURN))
					break;
			}

			Sleep(1);
		}

		for (int i = 0; i < 14; i++)
		{
			TEXTOUT(L"开发版本，不代表最终质量，以正式版为准！", UIleft + RANDOM(0, -UIleft * 2 - 150), UItop + RANDOM(0, -UItop * 2 - 20), 3, L"幼圆", colors[i][0], colors[i][1], colors[i][2]);
			Sleep(100);
		}

	}

	if (OnlOrLoc == 1) {
		WaitForSingleObject(hRecvThread, INFINITE);
		CloseHandle(hRecvThread);
	}
}

