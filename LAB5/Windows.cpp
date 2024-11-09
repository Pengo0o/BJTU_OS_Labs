#include <iostream>
#include <windows.h>
#include <time.h>
using namespace std;
#define MAXLOOP 10000
int nAccount1 = 0;
int nAccount2 = 0;

HANDLE hMutex = CreateMutex(NULL, FALSE, "MutexToProtectCriticalResource");
int nLoop = 0;
DWORD WINAPI ThreadExecuteZZW(LPVOID lpParameter)
{
    int *pID = (int *)lpParameter;
    int nTemp1, nTemp2, nRandom;

    do
    {
        WaitForSingleObject(hMutex, INFINITE);
        if (nLoop % 1000 == 0)
        {
            printf("thread %d is called nLoop is %d\n", *pID, nLoop);
        }
        nRandom = rand();
        nTemp1 = nAccount1;
        nTemp2 = nAccount2;
        nAccount1 = nTemp1 + nRandom;
        nAccount2 = nTemp2 - nRandom;
        ++nLoop;
        ReleaseMutex(hMutex);

    } while ((nAccount1 + nAccount2) == 0 && nLoop < MAXLOOP);

    return 0;
}
HANDLE CreateThreadHYP()
{
    HANDLE hThread[2];
    int nPID0 = 0, nPID1 = 1;
    if ((hThread[0] = CreateThread(NULL, 0, ThreadExecuteZZW, &nPID0, 0, NULL)) == NULL)
    {
        printf("线程 ThreadExecuteHYP-0 创建失败\n");
        exit(0);
    };
    if ((hThread[1] = CreateThread(NULL, 0, ThreadExecuteZZW, &nPID1, 0, NULL)) == NULL)
    {
        printf("线程 ThreadExecuteHYP-1 创建失败\n");
        exit(0);
    }

    return hThread[2];
}
int main()
{
    HANDLE hThread[2];
    clock_t start, end;
    start = clock();
    hThread[2] = CreateThreadHYP();

    WaitForMultipleObjects(2, hThread, true, INFINITE);

    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);
    printf("ThreadExecuteHYP-0 结束\n");
    printf("ThreadExecuteHYP-1 结束\n");

    end = clock();
    printf("时间：%lf\n", (double)(end - start) / CLOCKS_PER_SEC);
}