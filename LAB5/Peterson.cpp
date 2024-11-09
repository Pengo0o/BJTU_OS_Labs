#include <iostream>
#include <windows.h>
#include <time.h>
using namespace std;
#define MAXLOOP 10000

int nAccount1 = 0;
int nAccount2 = 0;

bool flag[2];
int turn = 0;
int nLoop = 0;

DWORD WINAPI ThreadExecutiveHYP(LPVOID lpParameter)
{
    int *pID = (int *)lpParameter;
    int nTemp1, nTemp2, nRandom;
    do
    {

        flag[*pID] = true;
        turn = 1 - *pID;
        while (flag[1 - *pID] && turn == 1 - *pID)
        {
        }
        if (nLoop % 1000 == 0)
        {
            cout << "thread " << *pID << " is called " << "nLoop is " << nLoop << endl;
        }

        nRandom = rand();
        nTemp1 = nAccount1;
        nTemp2 = nAccount2;
        nAccount1 = nTemp1 + nRandom;
        nAccount2 = nTemp2 - nRandom;
        ++nLoop;

        flag[*pID] = false;
    } while ((nAccount1 + nAccount2) == 0 && nLoop < MAXLOOP);

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
    time_t start, end;

    start = clock();
    hThread[2] = CreateThreadHYP();

    WaitForMultipleObjects(2, hThread, true, INFINITE);
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);
    printf("ThreadExecutiveHYP-0 结束\n");
    printf("ThreadExecutiveHYP-1 结束\n");

    end = clock();
    printf("时间：%lf\n", ((double)end - start) / CLOCKS_PER_SEC);
    return 0;
}
