#include <prime.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: program2.exe <int_value>\n");
        return 1;
    }

    int Value = atoi(argv[1]); // 获取传入
    char str[1024]{ 0 };

    // 打印接收到的int值
    printf("今天的起床代码: %d\n", Value);

    switch (Value) {
    case 1: {
    }break;

    default:

        break;
    }

    initgraph(250, 150);
    Prime_RECT();
    TEXTOUT(L"程序已挂起", 50, 50);
    //Sleep(1000);

    system("pause");
    return 1;
}
