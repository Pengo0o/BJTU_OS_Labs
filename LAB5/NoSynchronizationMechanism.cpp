#include <iostream>
#include <stdio.h>
#include <windows.h>
using namespace std;
int nAccount1 = 0, nAccount2 = 0;
DWORD WINAPI ThreadExecutiveHYP(LPVOID lpParameter)
{
    int *pID = (int *)lpParameter;

    int nLoop = 0;
    int nTemp1 = 0, nTemp2 = 0, nRandom;
    nRandom = rand();
    nAccount1 = nTemp1 + nRandom;
    nAccount2 = nTemp2 - nRandom;
    while (nAccount1 + nAccount2 == 0)
    {
        printf("thread %d is called nLoop is %d\n", *pID, nLoop);
        nRandom = rand();
        nTemp1 = nAccount1;
        nTemp2 = nAccount2;
        nAccount1 = nTemp1 + nRandom;
        nAccount2 = nTemp2 - nRandom;
        ++nLoop;
    }
    return 0;
}
HANDLE CreateThreadHYP()
{
    HANDLE hThread[2];
    int nPID0 = 0, nPID1 = 1;
    if ((hThread[0] = CreateThread(NULL, 0, ThreadExecutiveHYP, &nPID0, 0, NULL)) == NULL)
    {
        printf("线程 ThreadExecutiveHYP-0 创建失败\n");
        exit(0);
    };
    if ((hThread[1] = CreateThread(NULL, 0, ThreadExecutiveHYP, &nPID1, 0, NULL)) == NULL)
    {
        printf("线程 ThreadExecutiveHYP-1 创建失败\n");
        exit(0);
    }
    Sleep(100);
    return hThread[2];
}

int main()
{

    HANDLE hThread[2];
    hThread[2] = CreateThreadHYP();
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);
    printf("ThreadExecutiveHYP-0 结束\n");
    printf("ThreadExecutiveHYP-1 结束\n");
}