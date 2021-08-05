/*=====================================
CPU模拟器多核版本
注意：本代码在cpp格式下有可能CE。基于提交的单核版本@Version:2.1-release代码。
@Date:2021/6/5
@Version:1.01
@Verlog：
*好像可以正常卖票了 
*1.0 可运行。bug:锁内存未指定对象,似乎不影响结果.. 
*beta:0.send似乎是进程头文件的一个标识名，相对单核版本更名为cmdsend。
本版本可能不支持一些指令集示例以外的指令
代码预设：
1, 相对单核版本预设：16384对应的内存100.  由于是按位存储，所以需要手动处理二进制序列。
2，寄存器等初始值为0.
=====================================*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<windows.h>
#include<process.h>
//#include "tool.h"
//#include "in.h"
//#include "cmd.h"
//#include "out.h"

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

int to10(int, char *a, int len);//from +-string(2) to num(10) 
void cp(char*, int);//get complement
void input(FILE*, char*) ;//read file
void cmdsend(char*, char*, char (*)[16], char*, char[]);
void cal(int, char*, char*, char (*)[16], char*, char[]);
void logic(int, char*, char*, char (*)[16], char*, char[]);
int cmdcmp(char*, char*, char (*)[16], char*, char[]);
void cmdgoto(int,  char*, char*, int*);
void io(int, int,char *, char (*)[16], char[]);
void outio(int,int, int, char*, char (*)[16]); //cout state after cmd
void output(char[]);
void seglock(int, char*);
DWORD WINAPI run1(LPVOID lpParameter);
DWORD WINAPI run2(LPVOID lpParameter);

HANDLE hMutex;
char seg[RAMBITS];//线程间数据共享,同时防爆栈。

int main(void) {
	memset(seg, '0', sizeof(seg));
	seg[16384 * 8 + 9] = seg[16384 * 8 + 10] = seg[16384 * 8 + 13] = '1'; //100 = 0000000001100100
	HANDLE hThread1, hThread2;//线程的访问句柄
	DWORD ThreadID = 1;  //线程标识号
	FILE* fp1,* fp2;
	fp1 = fopen("dict1.dic", "r");
	if (fp1) {
		input(fp1, seg);
		fp2 = fopen("dict2.dic", "r");
		if (fp2) {
			input(fp2, seg + 256 * 8);
			hThread1 = CreateThread(NULL,  0,  run1, &seg[RAMBITS], 0,  &ThreadID);
			ThreadID = 2;
			hThread2 = CreateThread(NULL,  0,  run2, &seg[RAMBITS], 0,  &ThreadID);
			WaitForSingleObject(hThread1, INFINITE);
			CloseHandle (hThread1);
			WaitForSingleObject(hThread2, INFINITE);
			CloseHandle (hThread2);
		} else printf("Failed to Open File 2.\n");
	} else printf("Failed to Open File 1.\n");
	output(seg);
	fclose(fp1);
	fclose(fp2);
	system("PAUSE");
	return 0;
}

// from tool.c:
int to10(int flag, char *a, int len) {
	int ans = 0;
	int pow2 = 1;
	for (int i = len - 1; i >= 1; i--) {
		if (*(a + i) == '1')	ans += pow2;
		pow2 *= 2;
	}
	if (flag == 1 && *a == '1')	ans = -ans;
	else if (flag == 0 && *a == '1')  ans += pow2;
	return ans;
}


void cp(char* a, int len) {
	if (a[0] == '1') {
		for (int i = 1; i < len; i++) {
			if (a[i] == '1')	{
				a[i]--;
			} else if (a[i] == '0') {
				a[i]++;
			}
		}
		a[len - 1]++;
		for (int i = len - 1; i >= 1; i--) {
			if (a[i] == '2') {
				a[i] = '0';
				a[i - 1]++;
			} else break;
		}
	}
}

// from in.c:
void input(FILE * fp, char* seg ) {
	int i = 0;
	while (!feof(fp)) {
		char tmp = fgetc(fp);
		if (tmp == '\n')	tmp = fgetc(fp);
		seg[i] = tmp;
		i++;
	}
}

//from cmd.c
DWORD WINAPI run1(LPVOID lpParameter) {
	hMutex = CreateMutex (NULL, FALSE, NULL);
	char ir[16], c[5], a1[4], a2[4], nnum[16]; //存放当前处理的指令IR,内存,指令类型，AB寄存器，立即数（不保存，可更新）
	char ax[8][16]; //通用寄存器状态
	int flag = 0, ip = 0; //标志寄存器,指令计数器
	int i = 0;
	while (i < RAMBITS) {
		i = ip * 8;
		for (int k = 0; k < 16; i++, k++) {
			ir[k] = seg[i];
		}
		int j = 4;
		for (int k = 0; k < 4; k++, j++) c[k] = ir[j];
		for (int k = 0; k < 4; k++, j++) a1[k] = ir[j];
		for (int k = 0; k < 4; k++, j++) a2[k] = ir[j];
		for (int k = 0; k < 16; k++, i++) nnum[k] = seg[i];
		ip += 4;
		cp(nnum, 16);//cp & cp = +-string(2)
		if (strcmp(c, CMDSHUTDOWN) == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			outio(1,ip, flag, ir, ax);
			ReleaseMutex(hMutex);
			break;
		} else if (strcmp(c, CMDSEND) == 0) cmdsend(a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[0]) == 0) cal(0, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[1]) == 0) cal(1, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[2]) == 0) cal(2, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[3]) == 0) cal(3, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[0]) == 0) logic(0, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[1]) == 0) logic(1, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[2]) == 0) logic(2, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCMP) == 0) flag = cmdcmp(a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDGOTO) == 0) cmdgoto(flag, a2, nnum, &ip);
		else if (strcmp(c, CMDIO[0]) == 0) io(0, 1,a1, ax, seg);
		else if (strcmp(c, CMDIO[1]) == 0) io(1, 1,a1, ax, seg);
		else if (strcmp(c, CMDLOCK) == 0) seglock(1, nnum);
		else if (strcmp(c, CMDFREE) == 0) seglock(2, nnum);
		else if (strcmp(c, CMDSLEEP) == 0) Sleep(to10(1, nnum, 16));
		WaitForSingleObject(hMutex, INFINITE);
		outio(1,ip, flag, ir, ax);
		ReleaseMutex(hMutex);
	}
	_endthreadex(0);
}

DWORD WINAPI run2(LPVOID lpParameter) {
	hMutex = CreateMutex (NULL, FALSE, NULL);
	char ir[16], c[5], a1[4], a2[4], nnum[16]; //存放当前处理的指令IR,内存,指令类型，AB寄存器，立即数（不保存，可更新）
	char ax[8][16]; //通用寄存器状态
	int flag = 0, ip = 256; //标志寄存器,指令计数器
	int i = 0;
	while (i < RAMBITS) {
		i = ip * 8;
		for (int k = 0; k < 16; i++, k++) {
			ir[k] = seg[i];
		}
		int j = 4;
		for (int k = 0; k < 4; k++, j++) c[k] = ir[j];
		for (int k = 0; k < 4; k++, j++) a1[k] = ir[j];
		for (int k = 0; k < 4; k++, j++) a2[k] = ir[j];
		for (int k = 0; k < 16; k++, i++) nnum[k] = seg[i];
		ip += 4;
		cp(nnum, 16);//cp & cp = +-string(2)
		if (strcmp(c, CMDSHUTDOWN) == 0) {
			WaitForSingleObject(hMutex, INFINITE);
			outio(2,ip, flag, ir, ax);
			ReleaseMutex(hMutex);
			break;
		} else if (strcmp(c, CMDSEND) == 0) cmdsend(a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[0]) == 0) cal(0, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[1]) == 0) cal(1, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[2]) == 0) cal(2, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[3]) == 0) cal(3, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[0]) == 0) logic(0, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[1]) == 0) logic(1, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[2]) == 0) logic(2, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCMP) == 0) flag = cmdcmp(a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDGOTO) == 0) cmdgoto(flag, a2, nnum, &ip);
		else if (strcmp(c, CMDIO[0]) == 0) io(0, 2,a1, ax, seg);
		else if (strcmp(c, CMDIO[1]) == 0) io(1, 2,a1, ax, seg);
		else if (strcmp(c, CMDLOCK) == 0) seglock(1, nnum);
		else if (strcmp(c, CMDFREE) == 0) seglock(2, nnum);
		else if (strcmp(c, CMDSLEEP) == 0) Sleep(to10(1, nnum, 16));
		WaitForSingleObject(hMutex, INFINITE);
		outio(2,ip, flag, ir, ax);
		ReleaseMutex(hMutex);
	}
	_endthreadex(0);
}

void cmdsend(char* a1, char* a2, char (*ax)[16], char* nnum, char seg[]) {
	int dest = to10(0, a1, 4);
	int src = to10(0, a2, 4); //send src to dest
	if (src == 0)	strncpy(ax[dest - 1], nnum, 16);
	else if ( src >= 1 && src <= 4) strncpy(&seg[to10(1, ax[dest - 1], 16) * 8], ax[src - 1], 16);
	else if (src >= 5 && src <= 8) strncpy(ax[dest - 1], &seg[to10(1, ax[src - 1], 16) * 8], 16);
}

void cal(int cmdflag, char* a1, char* a2, char (*ax)[16], char* nnum, char seg[]) {
	int left = to10(0, a1, 4);//who need to change
	int right = to10(0, a2, 4);//what to change
	int i = 15, res = 0;
	char num1[16], num2[16], ans[16]; //in fact no mid-array in cpu , but it's too hard not to use it for pron.
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	memset(ans, '0', sizeof(ans));
	if (left == 0)	strncpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	strncpy(num1, ax[left - 1], 16) ;
	else if (left >= 5 && left <= 8) strncpy(num1, &seg[to10(1, ax[left - 1], 16) * 8], 16);
	if (right == 0)	strncpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	strncpy(num2, ax[right - 1], 16) ;
	else if (right >= 5 && right <= 8) strncpy(num2, &seg[to10(1, ax[right - 1], 16) * 8], 16);
	//get cal-result num(10)
	if (cmdflag == 0 )
		res = to10(1, num1, 16) + to10(1, num2, 16);
	else if (cmdflag == 1)
		res = to10(1, num1, 16) - to10(1, num2, 16);
	else if (cmdflag == 2)
		res = to10(1, num1, 16) * to10(1, num2, 16);
	else if (cmdflag == 3)
		res = to10(1, num1, 16) / to10(1, num2, 16);
	//change num10 to +-string(2)
	if (res < 0) {
		ans[0] = '1';
		res = - res;
	}
	while (res > 0 && i != 0 ) {
		ans[i] = (res % 2) + '0';
		res /= 2;
		i--;
	}
	if (left >= 1 && left <= 4)	strncpy( ax[left - 1], ans, 16) ;
	else if (left >= 5 && left <= 8) strncpy(&seg[to10(1, ax[left - 1], 16) * 8], ans, 16);
}

void logic(int cmdflag, char* a1, char* a2, char (*ax)[16], char* nnum, char seg[]) {
	int left = to10(0, a1, 4);//who need to change
	int right = to10(0, a2, 4);//what to change
	char num1[16], num2[16], ans[16]; //in fact no mid-array in cpu , but it's too hard not to use it for pron.
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	memset(ans, '0', sizeof(ans));
	if (left == 0)	strncpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	strncpy(num1, ax[left - 1], 16) ;
	else if (left >= 5 && left <= 8) strncpy(num1, &seg[to10(1, ax[left - 1], 16) * 8], 16);
	if (right == 0)	strncpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	strncpy(num2, ax[right - 1], 16) ;
	else if (right >= 5 && right <= 8) strncpy(num2, &seg[to10(1, ax[right - 1], 16) * 8], 16);

	if (cmdflag == 0) {
		int flag1 = 0, flag2 = 0;//like switch
		for (int i = 0; i < 16; i++) {
			if ((num1[i] == '1')) flag1++;
			if ((num2[i] == '1')) flag2++;
		}
		if (flag1 && flag2) ans[15] = '1';
	} else if (cmdflag == 1) {
		for (int i = 0; i < 16; i++) {
			if (num1[i] == '1' || num2[i] == '1')	 {
				ans[15] = '1';
				break;
			}
		}
	} else if (cmdflag == 2) {
		ans[15] = '1';
		if (left != 0) {
			for (int i = 0; i < 16; i++) {
				if (num1[i] == '1') {
					ans[15] = '0' ;
					break;
				}
			}
			strncpy(ax[left - 1], ans, 16);
		} else {
			for (int i = 0; i < 16; i++) {
				if (num2[i] == '1') {
					ans[15] = '0' ;
					break;
				}
			}
			strncpy(&seg[to10(1, ax[right - 1], 16) * 8], ans, 16);
		}
		return;
	}
	if (left >= 1 && left <= 4) strncpy(ax[left - 1], ans, 16);
	else if (left >= 5 && left <= 8) strncpy(&seg[to10(1, ax[left - 1], 16) * 8], ans, 16);
}

int cmdcmp(char* a1, char* a2, char (*ax)[16], char* nnum, char seg[]) {
	int left = to10(0, a1, 4);
	int right = to10(0, a2, 4);
	char num1[16], num2[16]; //in fact no mid-array in cpu , but it's too hard not to use it as pron.
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	int res = 0;
	if (left == 0)	strncpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	strncpy(num1, ax[left - 1], 16) ;
	else if (left >= 5 && left <= 8) strncpy(num1, &seg[to10(1, ax[left - 1], 16) * 8], 16);
	if (right == 0)	strncpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	strncpy(num2, ax[right - 1], 16) ;
	else if (right >= 5 && right <= 8) strncpy(num2, &seg[to10(1, ax[right - 1], 16) * 8], 16);

	for (int i = 0; i <= 15; i++) {
		if (num1[i] != num2[i])	res++;
	}
	if (res == 0) return 0;// if strcmp , bug. maybe '\0'
	if (num1[0] == '1' && num2[0] == '0')	res = -1;// - < +
	else if (num1[0] == '0' && num2[0] == '1')	res = 1;//+ > -
	else {
		if (strcmp(num1, num2) > 0)	res = 1;
		else if (strcmp(num1, num2) < 0)	res = -1;
		if (num1[0] == '1' && num2[0] == '1')	res = -res;
	}
	return res;
}

void cmdgoto(int flag, char* a2, char *nnum, int* ip) {
	int cmd = to10(0, a2, 4); //whether change
	int d = to10(1, nnum, 16) - 4; //"-4" to balance main.c
	if  (cmd == 0)	*ip += d;
	else if (cmd == 1 && flag == 0 ) *ip += d;
	else if (cmd == 2 && flag == 1 ) *ip += d;
	else if (cmd == 3 && flag == -1 ) *ip += d;
}

void io(int cmdflag, int threadid,char *a1, char (*ax)[16], char seg[]) {
	int tmp;
	char ans[16] ;
	memset(ans, '0', sizeof(ans));
	int left = to10(0, a1, 4);
	if (cmdflag == 0) {
		printf("id = %d    \nin:\n",threadid);
		scanf("%d", &tmp);
		int i = 15;
		if (tmp < 0) {
			ans[0] = '1';
			tmp = - tmp;
		}
		while (tmp > 0 && i != 0) {
			ans[i] = (tmp % 2) + '0';
			tmp /= 2;
			i--;
		}
		if (left <= 4) strncpy(ax[left - 1], ans, 16);
		else if (left > 4  ) strncpy(&seg[to10(1, ax[left - 1], 16) * 8], ans, 16);
	} else if (cmdflag == 1) {
		if (left <= 4)
			printf("id = %d    out: %d\n", threadid,to10(1, ax[left - 1], 16));
		else {
			printf("id = %d    out: %d\n", threadid, to10(1, &seg[to10(1, ax[left - 1], 16) * 8], 16));
		}
	}
}

void seglock(int cmdflag, char* nnum) {
	if (cmdflag == 1)
		WaitForSingleObject(hMutex, INFINITE); //好像课上说直接这样就行
	else if (cmdflag == 2) {
		ReleaseMutex(hMutex);
	}
}

//from out.c
void outio(int id,int ip, int flag, char* ir, char (*ax)[16]) {
	printf("id = %d\n",id);
	printf("ip = %d\n", ip);
	printf("flag = %d\n", flag);
	printf("ir = %d\n", to10(1, ir, 16));
	for (int i = 0; i < 3; i++) {
		printf("ax%d = %d ", i + 1, to10(1, ax[i], 16));
	}
	printf("ax4 = %d\n", to10(1, ax[3], 16));
	for (int i = 4; i < 7; i++) {
		printf("ax%d = %d ", i + 1, to10(1, ax[i], 16));
	}
	printf("ax8 = %d\n", to10(1, ax[7], 16));
}
void output(char seg[]) {
	int nowp = 0;
	printf("\ncodeSegment :\n");
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 7; j++) {
			printf("%d ", to10(1, seg + nowp, 32));
			nowp += 32;
		}
		printf("%d\n", to10(1, seg + nowp, 32));
		nowp += 32;
	}
	nowp = 16384 * 8;
	printf("\ndataSegment :\n");
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 15; j++) {
			printf("%d ", to10(1, seg + nowp, 16));
			nowp += 16;
		}
		printf("%d\n", to10(1, seg + nowp, 16));
		nowp += 16;
	}
}
