/*注意：此源文件未进行完全的开发

Attention:This source file has not been fully developed
*/////////////////////////

#pragma warning(disable:6031)
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include <time.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include <vector>

SOCKADDR_IN cAddr = { 0 };
int len = sizeof cAddr;
#define IMAGE_PATH "D:\\charwar\\charwar.png"
const char* sever_ip = "127.0.0.1";
int port = 1234;
char SendBuffer[256];



void RandomArray(int max, int* array) {
	// 初始化数组，数组索引从 1 开始
	for (int i = 1; i <= max; i++) {
		array[i] = i;
	}

	// 对数组进行随机置换
	for (int i = 1; i <= max; i++) {
		int j = rand() % max + 1;
		// 交换 array[i] 和 array[j]
		int temp = array[i];
		array[i] = array[j];
		array[j] = temp;
	}
}

int main() {
	//1 确定协议版本
	system("COLOR 0D ");

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		printf("确定协议版本失败!\n");
		return -1;
	}
	printf("确定协议版本成功!\n");

	//2 创建socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == serverSocket)
	{
		printf("创建socket失败:%d\n", GetLastError());
		//9 清理协议版本信息
		WSACleanup();
		return -1;
	}
	printf("创建socket成功!\n");

	printf("以下不会写入Data中\n");

	int host = 2;
	printf("是否为主机(1为是，其他为否)：");
	scanf("%1d", &host);
	printf("%d", host);

	printf("\n输入本机IP地址：");
	char ip_address[16] = { 0 };
	scanf("%15s", ip_address);
		if (strlen(ip_address) <= 6 || strlen(ip_address) >= 16)
			strncpy_s(ip_address, sever_ip, 16);
		printf("\nIP地址为：%s\n输入端口(4位或5位，理论上随便输)：", ip_address);
	scanf("%5d", &port);
		if( port<1000 || port > 100000)
			port = 9123;
		printf("IP地址为：%d\n", port);


	//3 创建服务器协议地址簇
	SOCKADDR_IN addr = { 0 };
	addr.sin_family = AF_INET;//协议地址族
	addr.sin_addr.S_un.S_addr = inet_addr(ip_address);//这里改成服务器IP地址
	addr.sin_port = htons(port);//10000左右    小端转大端

	if (host == 1) {
		//4 绑定
		int r = bind(serverSocket, (struct sockaddr*)&addr, sizeof addr);
		if (r == -1)
		{
			printf("\n绑定失败:%d\n", GetLastError());
			//8 关闭socket
			closesocket(serverSocket);
			//9 清理协议版本信息
			WSACleanup();
			return -1;
		}printf("\n绑定成功!\n");

		//5 监听
		r = listen(serverSocket, 10);
		if (r == -1)
		{
			printf("监听失败:%d\n", GetLastError());
			//8 关闭socket
			closesocket(serverSocket);
			//9 清理协议版本信息
			WSACleanup();
			return -1;
		}
		printf("监听成功!\n");

		//6 等待客户端连接
		int Play = 1, Players = 2, Allconnet = 0;
		std::vector<SOCKET> clientSocket(Players + 1);
		std::vector<int> RandPlaer(Players + 1);
		std::vector<BOOL> Playersconnet(2);

		srand(time(0));

		while (Play != 0)
		{
			//system("CLS");
			printf("等待客户端连接...\n");

			system("COLOR 09 ");

			clientSocket[0] = accept(serverSocket, (sockaddr*)&cAddr, &len);
			r = recv(clientSocket[0], SendBuffer, 255, NULL);//获取第一个人传来的玩家人数信息
			SendBuffer[r] = '\0';
			printf("最大玩家数：%d\n", atoi(SendBuffer));
			Players = atoi(SendBuffer);
			if (Players <= 1) Players = 2;
			int* randomArray = (int*)malloc((Players + 1) * sizeof(int));

			clientSocket.resize(Players + 1);//动态分配
			RandPlaer.resize(Players + 1);//记得free
			Playersconnet.resize(Players);


			for (int i = 0; i < Players; i++) {
				RandPlaer[i] = 0;
				Playersconnet[i] = 0;
				//setsockopt(clientSocket[i], SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
			}

			RandomArray(Players, randomArray);
			for (int i = 1; i <= Players; i++) {
				RandPlaer[i - 1] = randomArray[i];
				printf("%d ", RandPlaer[i - 1]);
			}free(randomArray);
			printf("\n");

			Sleep(20);
			sprintf(SendBuffer, "%d!Players", Players);  send(clientSocket[0], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送屎
			sprintf(SendBuffer, "%d!Allconnet", Allconnet);  send(clientSocket[0], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送屎

			Playersconnet[0] = 1;
			while (Allconnet != Players)
			{
				for (int i = 0; i <= Players - 1; i++)
				{
					if (Playersconnet[i] != 1) 	clientSocket[i] = accept(serverSocket, (sockaddr*)&cAddr, &len);
					Playersconnet[i] = 1; Allconnet++;
					Sleep(10);

					for (int j = 0; j <= Players - 1; j++)
					{
						sprintf(SendBuffer, "%d!Players", Players);  send(clientSocket[j], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送屎
						sprintf(SendBuffer, "%d!Allconnet", Allconnet);  send(clientSocket[j], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送屎
						r = recv(clientSocket[j], SendBuffer, 255, NULL);  //接收总人数，顺便看看在不在线
						if (r <= 0) Playersconnet[j] = 0;
						//printf("%d  ", Playersconnet[j]);
					}

					//printf("\n");
					Allconnet = 0;
					for (int j = 0; j <= Players - 1; j++)
						Allconnet += Playersconnet[j];
					for (int j = 0; j <= Players - 1; j++)
					{
						Sleep(30);
						sprintf(SendBuffer, "%d=Players", Players);  send(clientSocket[j], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送总玩家数
						sprintf(SendBuffer, "%d=Allconnet", Allconnet);  send(clientSocket[j], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送目前玩家数
					}
					printf("总人数：%d   现在人数：%d\n", Players, Allconnet);

				}
			}

			Sleep(100);
			for (int i = 0; i <= Players - 1; i++)
			{	//重新发送防止有人没进来
				Sleep(200);
				sprintf(SendBuffer, "%d=Players", Players);  send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送总玩家数
				Sleep(50);
				sprintf(SendBuffer, "%d=Allconnet", Allconnet);  send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送目前玩家数
			}

			int FirstPlaer;

			Sleep(200);
			for (int i = 0; i <= Players - 1; i++)
			{
				Sleep(50);
				if (RandPlaer[i] == 1) FirstPlaer = i;
				sprintf(SendBuffer, "%d=RandPosi", RandPlaer[i]);  send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送总玩家数
			}

			printf("换地图咯\n");
			while (1) {  //算了，这里暂时没有断线重连
				r = recv(clientSocket[FirstPlaer], SendBuffer, 255, NULL); SendBuffer[r] = '\0';
				if (r <= 0) clientSocket[FirstPlaer] = accept(serverSocket, (sockaddr*)&cAddr, &len);

				if (strcmp(SendBuffer, "GoodBye") == 0) {
					for (int i = 1; i <= Players - 1; i++) {
						sprintf(SendBuffer, "%s", "GoodBye");  send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);
					}
					printf("退出游戏咯\n");
					Sleep(250);
					return 1;
				}

				if (strcmp(SendBuffer, "Char!Ranmap") == 0) 
				{
					for (int i = 0; i <= Players - 1; i++)
						if (i != FirstPlaer)
						{
							r = send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);
							if (r <= 0) Playersconnet[i] = 0;
						}

					r = recv(clientSocket[FirstPlaer], SendBuffer, 255, NULL); SendBuffer[r] = '\0';

					long int fileSize = atol(SendBuffer);
					printf("地图文件大小：%ld\n", fileSize);
					char* buffer = (char*)malloc(fileSize);
					if (buffer == NULL) {
						printf("分配缓冲区！\n");
						return 1;
					}

					sprintf(SendBuffer, "%ld", fileSize);
					for (int i = 0; i <= Players - 1; i++)
						if (i != FirstPlaer)
						{
							r = send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);
							if (r <= 0) Playersconnet[i] = 0;
						}

					size_t bytesRect = 0;
					int result = 0;
					while(bytesRect < fileSize)
					{
						result = recv(clientSocket[FirstPlaer], buffer+bytesRect, fileSize-bytesRect, 0);
						if(result <= 0)
						{
							printf("接收失败: %d\n", WSAGetLastError());
							free(buffer);
							return 1;//因为没有断线重连
						}
						for (int i = 0; i <= Players - 1; i++)
							if (i != FirstPlaer)
							{
								r = send(clientSocket[i], buffer+bytesRect, fileSize-bytesRect, NULL);
								if (r <= 0) Playersconnet[i] = 0;
							}

						bytesRect += result;
					}
					printf("接收完毕，转发完毕\n");
				}

				if (strcmp(SendBuffer, "Char!map") == 0) {

					for (int i = 0; i <= Players - 1; i++)
						if (i != FirstPlaer)
						{
							r = send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送Char!地图
							if (r <= 0) Playersconnet[i] = 0;
						}

					r = recv(clientSocket[FirstPlaer], SendBuffer, 255, NULL); SendBuffer[r] = '\0';
					for (int i = 0; i <= Players - 1; i++)
						if (i != FirstPlaer)
							send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送图片地址

					printf("%s\n", SendBuffer);


					/*
					while(1)
					{
						r = recv(clientSocket[FirstPlaer], SendBuffer, 255, NULL); SendBuffer[r] = '\0';

						if (strcmp(SendBuffer, "mapCheck") == 0) break;

						for (int i = 0; i <= Players - 1; i++)
							if (i != FirstPlaer) {
								send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送地图像素
							}

					}

					sprintf(SendBuffer, "%s", "mapCheck");
					for (int i = 0; i <= Players - 1; i++)
						if (i != FirstPlaer) {
							send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送地图完毕
						}
					*/
				}

				if (strcmp(SendBuffer, "Start") == 0) {
					for (int i = 0; i <= Players - 1; i++)
						if (i != FirstPlaer)
							send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);//给客户端i发送Char!要开始了唷

					break;
				}

			}

			//atof
			printf("发送X坐标\n");
			r = recv(clientSocket[FirstPlaer], SendBuffer, 255, NULL); SendBuffer[r] = '\0';

			for (int i = 0; i < Players; i++)
				if (i != FirstPlaer)
				send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);

			printf("发送颜色\n");
			for (int j = 0; j < Players; j++) {
				r = recv(clientSocket[FirstPlaer], SendBuffer, 255, NULL); SendBuffer[r] = '\0';
				for (int i = 0; i < Players; i++)
					if (i != FirstPlaer)
						send(clientSocket[i], SendBuffer, strlen(SendBuffer), NULL);
			}

			printf("进入对战阶段\n");
			char str[100];

			//注意主客同步
			const int ROWS = 21+1;
			const int COLS = 23+1;
			float PlayersData[ROWS][COLS] = { 0 };
			const int Mines = 1;
			float MinesArry[Mines*3+1] = { 0 };


			while (1)
			{
				//  |->按键  :->说话
				for (int i = 0; i <= Players - 1; i++) {
					if (RandPlaer[i] == Play) {
						r = recv(clientSocket[i], SendBuffer, 255, NULL);
						SendBuffer[r] = '\0';
						if (r <= 0)
						{
							sprintf(SendBuffer, "%d号位掉线\n", Play);
							printf("%d号位掉线", Play);
							for (int j = 0; j <= Players - 1; j++)
								send(clientSocket[j], SendBuffer, strlen(SendBuffer), NULL);
							clientSocket[i] = accept(serverSocket, (sockaddr*)&cAddr, &len);
							Playersconnet[i] = 1;

							//发送补全其他数据
						}

						char FirstChar = SendBuffer[0];

						printf("%d正在监听%d：%s\n", r, Play, SendBuffer);

						for (int f = 0; f <= Players - 1; f++)
							if(f != i)
								send(clientSocket[f], SendBuffer, strlen(SendBuffer), NULL);

						//接收到发射
						if (FirstChar == '|' && SendBuffer[1] == '3' && SendBuffer[2] == '5') 
						{
							printf("%d：Fire\n", Play);

							///转发图片仍然有问题
							//接收转发图片

							/*
							while (1) {

								//接收文件大小
								r = recv(clientSocket[i], SendBuffer, 255, NULL); SendBuffer[r] = '\0';

								long int fileSize = atol(SendBuffer);
								printf("地图文件大小：%ld\n", fileSize);
								char* buffer = (char*)malloc(fileSize);
								if (buffer == NULL) {
									printf("分配缓冲区！\n");
									break;
								}

								sprintf(SendBuffer, "%ld", fileSize);
								for (int j = 0; j <= Players-1; j++)
									if (j != FirstPlaer)
									{
										r = send(clientSocket[j], SendBuffer, strlen(SendBuffer), NULL);
										if (r <= 0) Playersconnet[j] = 0;
									}

								size_t bytesRect = 0;
								int result = 0;
								while (bytesRect < fileSize)
								{
									result = recv(clientSocket[FirstPlaer], buffer+bytesRect, fileSize-bytesRect, 0);
									if (result <= 0)
									{
										printf("接收失败: %d\n", WSAGetLastError());
										free(buffer);
										break;//因为没有断线重连
									}

									for (int j = 0; j <= Players-1; j++)
										if (j != FirstPlaer)
										{
											r = send(clientSocket[j], buffer+bytesRect, fileSize-bytesRect, NULL);
											if (r <= 0) Playersconnet[j] = 0;
										}

									bytesRect += result;
								}

								free(buffer);


								printf("地图接收完毕，转发完毕\n");
								break;
							}
							*/
							//接收转发数组
							{
								r = recv(clientSocket[i], (char*)PlayersData, sizeof(PlayersData), 0);
								if (r > 0) {
									for (int f = 0; f <= Players-1; f++)
										if (f != i) {
											send(clientSocket[f], (char*)PlayersData, sizeof(PlayersData), 0);
										}

									printf("数组字节：%d\n", r);
									/*
									printf("内容包(已忽略0行0列)：\n");
									for (int a = 0; a < ROWS; ++a) {
										for (int b = 1; b < COLS; ++b) {
											if (a != 0) printf("%.1f\t", PlayersData[a][b]);
											else printf("%d列\t", b);
										}
										printf("\n%d行：", a+1);
									}*/
								}

							}

							//发送地雷数组
							{
								r = recv(clientSocket[i], (char*)MinesArry, sizeof(MinesArry), 0);
								if (r > 0) {

									for (int f = 0; f <= Players-1; f++)
										if (f != i) {
											send(clientSocket[f], (char*)MinesArry, sizeof(MinesArry), 0);
										}


									printf("地雷数组字节：%d\n", r);
									/*
									printf("内容包(已忽略0行0列)：\n");
									for (int a = 0; a < ROWS; ++a) {
										for (int b = 1; b < COLS; ++b) {
											if (a != 0) printf("%.1f\t", PlayersData[a][b]);
											else printf("%d列\t", b);
										}
										printf("\n%d行：", a+1);
									}*/
								}

							}


						}
						
						if (strcmp(SendBuffer, "FireEnd") == 0)
						{
							//再次转发数组
							r = recv(clientSocket[i], (char*)PlayersData, sizeof(PlayersData), 0);
							if (r > 0) {

								for (int f = 0; f <= Players-1; f++)
									if (f != i) {
										send(clientSocket[f], (char*)PlayersData, sizeof(PlayersData), 0);
									}


								printf("数组字节：%d\n", r);

								/*
								printf("内容包(已忽略0行0列)：\n");
								for (int a = 0; a < ROWS; ++a) {
									for (int b = 1; b < COLS; ++b) {
										if (a != 0) printf("%.1f\t", PlayersData[a][b]);
										else printf("%d列\t", b);
									}
									printf("\n%d行：", a+1);
								}*/
							}

							//发送地雷数组
							{

								r = recv(clientSocket[i], (char*)MinesArry, sizeof(MinesArry), 0);
								if (r > 0) {

									for (int f = 0; f <= Players-1; f++)
										if (f != i) {
											send(clientSocket[f], (char*)MinesArry, sizeof(MinesArry), 0);
										}


									printf("地雷数组字节：%d\n", r);
									/*
									printf("内容包(已忽略0行0列)：\n");
									for (int a = 0; a < ROWS; ++a) {
										for (int b = 1; b < COLS; ++b) {
											if (a != 0) printf("%.1f\t", PlayersData[a][b]);
											else printf("%d列\t", b);
										}
										printf("\n%d行：", a+1);
									}*/
								}

							}

							Play++;
							if (Play == Players+1) Play = 1;
							printf("正在监听%d\n", Play);
							SendBuffer[0] = 0;

						}


						break;
					}
				}
			}
		}
	}//if host
	else  //Client
	{
		//4 连接服务器
		int ret = connect(serverSocket, (sockaddr*)&addr, sizeof addr);
		if (ret == -1)
		{
			printf("连接服务器失败:%d\n", GetLastError());
			//6 关闭socket
			closesocket(serverSocket);
			//7 清理协议版本信息
			WSACleanup();
			system("pause");
			return -1;
		}
		else {
			printf("连接服务器成功!\n");
			printf("该功能未开发完成");
		}

	}


	//8 关闭socket
	closesocket(serverSocket);
	//9 清理协议版本信息
	WSACleanup();
	return 0;
}

