#include <winsock2.h>
#include <prime.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 255

#define TxtX    50
int TxtY = 0;
unsigned int TxtRow = 0;


FILE* file;
FILE* tempFile;
char* str;
char strtemp[50];
char Row[100];
const char* filename = "D:\\CharWar\\PlayerData.txt";
const char* tempFilename = "temp_file.txt";
FILE *xttxOut = fopen("D:\\CharWar\\Print.txt", "wb");//读读读读读读没有就创创创
    const char* sever_ip = "127.0.0.1";
    int port = 8080;
    int PlaerCnet[MAX_CLIENTS];
    SOCKET serverSocket;
    SOCKADDR_IN serverAddress, clientAddress;
    int clientAddressSize;
    SOCKET ClientSockets[MAX_CLIENTS];
    int numClients = 0;
char inputstr[BUFFER_SIZE];  //用户输入
const char* PlAyerName = "Name";    //用户名称
int ClientPosition = 0; //用户号位




typedef struct
{
    SOCKET socket;
    SOCKADDR_IN address;
    DWORD id;
} ClientInfo;


//模式。转换。打印。输出。刷新。
//sprintf的进阶，对比无需使用前一个参数，xttxOut需要自行修改或者加入参数中
void FPrint(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);


    //--------匹配字符颜色
    char* ptr = buffer;
    char output[256];
    char* output_ptr = output;//指向输出数组指针

    const int MAX_COLORS = 10;//最多颜色+1
    int red[MAX_COLORS], green[MAX_COLORS], blue[MAX_COLORS];
    int count = 0;

    while ((ptr = strstr(ptr, "/RGB")) != NULL && count < MAX_COLORS) {
        sscanf(ptr, "/RGB(%d,%d,%d)", &red[count], &green[count], &blue[count]);
        count++;
        ptr++; // 避免死循环
    }

    ptr = buffer;//我将之重载
    int ininninRGB = 0;//标记是否内内部部

    while (*ptr) {
        if (*ptr == '/' && *(ptr + 1) == 'R' && *(ptr + 2) == 'G' && *(ptr + 3) == 'B') {
            ininninRGB = 1;
                while (*ptr && *ptr != ')')
                ptr++;
            ininninRGB = 0;
        }else if (!ininninRGB) {
            *output_ptr = *ptr; // 将字符存储到输出数组
            output_ptr++; // 移动输出指针到下一个位置
        }
        ptr++;
    }*output_ptr = '\0';
    strcpy(buffer, output);
    //--------匹配字符颜色end

    TxtRow++;
    printf("%s", buffer);
    //TEXTOUT(buffer, TxtX, TxtY, 4, "幼圆", );
    fputs(buffer, xttxOut);
    fflush(xttxOut);

    va_end(args);
}

DWORD WINAPI ClientThread(LPVOID lpParameter)
{
    ClientInfo* clientInfo = (ClientInfo*)lpParameter;
    SOCKET clientSocket = clientInfo->socket;
    char buffer[BUFFER_SIZE];
    int bytesRead;
    char* ClientName;

    bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0); buffer[bytesRead - 1] = '\0';
    ClientName = buffer;
    FPrint("%s 已上线\n", ClientName);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        bytesRead = send(ClientSockets[i], buffer, BUFFER_SIZE, 0);
    }

    while (1)
    {
        //memset(buffer, 0, sizeof(buffer));
        bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);

        if (bytesRead <= 0)
        {
            FPrint("%s 已下线，等待重连中...\n", ClientName);
            //accept（）
            break;
        }
        //buffer[bytesRead - 1] = '\0';

        FPrint("%s\n", buffer);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            bytesRead = send(ClientSockets[i], buffer, BUFFER_SIZE, 0);
        }
    }

    closesocket(clientSocket);
    free(clientInfo);
    return 0;
}

//客户端接收收收收线程
static void ClientRecvThread(SOCKET* ConnectSocket) {
    char recvBuffer[BUFFER_SIZE];
    int recvSize;
    recvSize = recv(*ConnectSocket, recvBuffer, BUFFER_SIZE, 0);
    //recvBuffer[recvSize] = '\0';
    ClientPosition = atoi(recvBuffer);


    while (1) {
        recvSize = recv(*ConnectSocket, recvBuffer, BUFFER_SIZE, 0);
        fputs(recvBuffer, xttxOut);
        fflush(xttxOut);
        if (recvSize > 0 && atoi(recvBuffer) != ClientPosition) {
            recvBuffer[recvSize - 1] = '\0';
            printf("\n%s\n你说：", recvBuffer);//你说说你说说你
        }
    }
}

//服务务务务务务务端说说说
static void ServerSendThread(SOCKET* ClientSocket) {
    char recvBuffer[BUFFER_SIZE];
    int bytesRead;
    FPrint("\n你说：");

    while (1)
    {
        if (fgets(inputstr, sizeof(inputstr), stdin)) {
                fputs(inputstr, xttxOut);   fflush(xttxOut);

            inputstr[strcspn(inputstr, "\n")] = '\0';
                sprintf(recvBuffer, "%d)%s：%s", -1, PlAyerName, inputstr);//const char* PlAyerName,inputstr[256]

            for (int i = 0; i < MAX_CLIENTS; i++) {
                bytesRead = send(ClientSockets[i], recvBuffer, BUFFER_SIZE, 0);
            }
            FPrint("你说: ");//你说说你服雾埋说说你服器

        }
        else {
            printf("输入错误或结束。\n");
        }
    }
}




int main()
{
    //initgraph(640, 640); Prime_RECT();

    int PlayerPOSI = 1;
    system("COLOR 09 ");

    file = fopen(filename, "r");
    if (file == NULL) {
        FPrint("无法打开文件 %s\n", filename);
        return 1;
    }
    if (xttxOut == NULL) {
        FPrint("无法打开文件 \n");
        return 1;
    }

    fseek(file, 0, SEEK_SET);//文件头
    while (fgets(Row, sizeof(Row), file)) {
        if (strstr(Row, "#PlayerPosi:") != NULL) {
            str = strchr(Row, ':') + 1;
            PlayerPOSI = atoi(str);
                FPrint("号位: %s", str);
        }
        if (strstr(Row, "#InputIP:") != NULL) {
            char buffer[20];
            const char* temp_sever_ip = strchr(Row, ':') + 1;
            size_t length = strcspn(temp_sever_ip, "\r\n");
            strncpy(buffer, temp_sever_ip, length);
            buffer[length] = '\0'; // 添加结束符
            sever_ip = buffer;
                FPrint("服务器地址: %s\n", sever_ip);
        }
        if (strstr(Row, "#InputPortChat:") != NULL) {
            str = strchr(Row, ':') + 1;
            port = atoi(str);
                FPrint("服务器端口: %s", str);
        }
        if (strstr(Row, "#PlayerName:") != NULL) {
            char* player_name = strchr(Row, ':') + 1;
            size_t len = strcspn(player_name, "\r\n");
            player_name[len] = '\0';
            PlAyerName = player_name;
                FPrint("玩家名字: %s", player_name);
            break;
        }
    }fclose(file);

    if (PlayerPOSI == 1)
    {
        WSADATA wsaData;

        // 初始化Winsock库
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            FPrint("%s", "\n\n确定协议版本失败!\n");
            return 1;
        }
        FPrint("%s", "\n\n确定协议版本成功!\n");

        // 创建服务器套接字
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET)
        {
            FPrint("创建socket失败:%d\n", GetLastError());
            WSACleanup();
            return 1;
        }FPrint("创建socket成功!\n", xttxOut);


        // 设置服务器地址
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(sever_ip);
        serverAddress.sin_port = htons(port);

        // 绑定服务器套接字到指定地址和端口
        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
        {
            FPrint("绑定失败:%d\n", GetLastError());

            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }FPrint("绑定成功!\n");
        


        // 开始监听客户端连接请求
        if (listen(serverSocket, MAX_CLIENTS) == SOCKET_ERROR)
        {
            FPrint("监听失败:%d\n", GetLastError());
            

            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }FPrint("监听成功!\n\n等待客户端连接...\n");
        

        // 接受并处理客户端连接
        ClientInfo* clients[MAX_CLIENTS];
        HANDLE threadHandles[MAX_CLIENTS];

        DWORD threadId;
        HANDLE hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerSendThread, (LPVOID)&serverSocket, 0, &threadId);
        if (hRecvThread == NULL) {
            FPrint("失败创建线程\n");
            
            return 1;
        }
        CloseHandle(hRecvThread);


        while (1)
        {
            // 接受客户端连接请求
            clientAddressSize = sizeof(clientAddress);
            ClientSockets[numClients] = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
            if (ClientSockets[numClients] == INVALID_SOCKET)
            {
                FPrint("失败接受客户连接\n");
                
                continue;
            }

            sprintf(inputstr, "%d", numClients);
            send(ClientSockets[numClients], inputstr, BUFFER_SIZE, 0);

            // 创建新线程来处理客户端请求
            ClientInfo* clientInfo = (ClientInfo*)malloc(sizeof(ClientInfo));
            clientInfo->socket = ClientSockets[numClients];
            clientInfo->address = clientAddress;
            clientInfo->id = numClients + 1;

            threadHandles[numClients] = CreateThread(NULL, 0, ClientThread, clientInfo, 0, NULL);

            if (threadHandles[numClients] == NULL)
            {
                printf("\n失败为客户创造线程 #%d\n", clientInfo->id);
                closesocket(ClientSockets[numClients]);
                free(clientInfo);
                FPrint("创建线程失败\n");
                

            }
            else
            {
                FPrint("\n接受客户连接来自 %s:%d (client #%d)\n",
                    inet_ntoa(clientInfo->address.sin_addr),
                    ntohs(clientInfo->address.sin_port),
                    clientInfo->id);

                // 存储客户端信息
                clients[numClients] = clientInfo;
                numClients++;

                // 如果达到最大客户端数目，则退出循环，不再接受新的连接
                if (numClients >= MAX_CLIENTS)
                    break;
            }
        }

        // 等待所有线程退出
        WaitForMultipleObjects(numClients, threadHandles, TRUE, INFINITE);

        // 清理资源
        for (int i = 0; i < numClients; i++)
        {
            closesocket(clients[i]->socket);
            free(clients[i]);
        }

        closesocket(serverSocket);
        WSACleanup();
    }//as Sever
    else
    {
        WSADATA wsaData;
        int ret;
        char RecvBuffer[256];

        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            FPrint("\n\n确定协议版本失败!\n");
            return 1;
        }FPrint("\n\n确定协议版本成功!\n");


        //2 创建socket
        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (SOCKET_ERROR == serverSocket)
        {
            FPrint("创建socket失败:%d\n", GetLastError());
            //9 清理协议版本信息
            WSACleanup();
            return -1;
        }FPrint("创建socket成功!\n");
        

        //3 获取服务器协议地址簇
        SOCKADDR_IN addr = { 0 };
        addr.sin_family = AF_INET;//协议地址族
        addr.sin_addr.S_un.S_addr = inet_addr(sever_ip);//这里填写服务器IP地址
        addr.sin_port = htons(port);//服务器的端口

        //4 连接服务器
        ret = connect(serverSocket, (sockaddr*)&addr, sizeof addr);
        if (ret == -1)
        {
            FPrint("连接服务器失败:%d\n", GetLastError());
            //6 关闭socket
            closesocket(serverSocket);
            //7 清理协议版本信息
            WSACleanup();
            return -1;
        }FPrint("连接服务器成功!\n");

        Sleep(50);
        sprintf(RecvBuffer, "%s", PlAyerName);
        send(serverSocket, RecvBuffer, 255, NULL);

        DWORD threadId;
        HANDLE hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ClientRecvThread, (LPVOID)&serverSocket, 0, &threadId);
        if (hRecvThread == NULL) {
            printf("Failed to create receive thread\n");
            return 1;
        }
        CloseHandle(hRecvThread);


        FPrint("你说: ");

        while (1) 
        {
            if (fgets(inputstr, sizeof(inputstr), stdin)) {
                // 删除末尾的换行符
                inputstr[strcspn(inputstr, "\n")] = '\0';
                sprintf(RecvBuffer, "%d)%s：%s", ClientPosition, PlAyerName, inputstr);
                       fputs(RecvBuffer, xttxOut);
                send(serverSocket, RecvBuffer, 255, NULL);
                FPrint("你说: ");
            }
            else {
                printf("输入错误或结束。\n");
            }
        }



        //6 关闭socket
        closesocket(serverSocket);
        //7 清理协议版本信息
        WSACleanup();
    }
   
    FPrint("进程退出");
    return 0;
}
