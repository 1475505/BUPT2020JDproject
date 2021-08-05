#include<stdio.h>
#include <string.h>
#include<stdlib.h>
#include<windows.h>
#include<process.h>
#include "tool.h"
#include "in.h"
#include "out.h"
#include "cmd.h"
#define RAMBITS 32768*8+1

const char CMDSHUTDOWN[5] = "0000";
const char CMDSEND[5] = "0001";
const char CMDCAL[4][5] = {"0010", "0011", "0100", "0101"};   // + - * /
const char CMDLOGIC[3][5] = {"0110", "0111", "1000" };   //  &&  ||  not
const char CMDCMP[5] = "1001";
const char CMDGOTO[5] = "1010";
const char CMDIO[2][5] = {"1011", "1100" };
const char CMDLOCK[5] = "1101";
const char CMDFREE[5] = "1110";
const char CMDSLEEP[5] = "1111";

char seg[RAMBITS];//线程间数据共享,同时防爆栈。

int main(void) {
	memset(seg, '0', sizeof(seg));
	seg[16384 * 8 + 9] = seg[16384 * 8 + 10] = seg[16384 * 8 + 13] = '1'; //100 = 0000000001100100
	HANDLE hThread1, hThread2;//线程的访问句柄
	DWORD ThreadID = 1;  //线程标识号
	FILE* fp1, * fp2;
	fp1 = fopen("dict1.dic", "r");
	if (fp1) {
		input(fp1, seg);
		fp2 = fopen("dict2.dic", "r");
		if (fp2) {
			input(fp2, seg + 256 * 8);
			hThread1 = CreateThread(NULL, 0, run1, &seg[RAMBITS], 0, &ThreadID);
			ThreadID = 2;
			hThread2 = CreateThread(NULL, 0, run2, &seg[RAMBITS], 0, &ThreadID);
			WaitForSingleObject(hThread1, INFINITE);
			CloseHandle(hThread1);
			WaitForSingleObject(hThread2, INFINITE);
			CloseHandle(hThread2);
		}
		else printf("Failed to Open File 2.\n");
	}
	else printf("Failed to Open File 1.\n");
	output(seg);
	fclose(fp1);
	fclose(fp2);
	system("PAUSE");
	return 0;
}