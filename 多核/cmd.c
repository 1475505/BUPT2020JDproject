#include <string.h>
#include <stdio.h>
#include "tool.c"
#include "cmd.h"

DWORD WINAPI run1(LPVOID lpParameter) {
hMutex = CreateMutex(NULL, FALSE, NULL);
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
		outio(1, ip, flag, ir, ax);
		ReleaseMutex(hMutex);
		break;
	}
	else if (strcmp(c, CMDSEND) == 0) cmdsend(a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDCAL[0]) == 0) cal(0, a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDCAL[1]) == 0) cal(1, a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDCAL[2]) == 0) cal(2, a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDCAL[3]) == 0) cal(3, a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDLOGIC[0]) == 0) logic(0, a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDLOGIC[1]) == 0) logic(1, a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDLOGIC[2]) == 0) logic(2, a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDCMP) == 0) flag = cmdcmp(a1, a2, ax, nnum, seg);
	else if (strcmp(c, CMDGOTO) == 0) cmdgoto(flag, a2, nnum, &ip);
	else if (strcmp(c, CMDIO[0]) == 0) io(0, 1, a1, ax, seg);
	else if (strcmp(c, CMDIO[1]) == 0) io(1, 1, a1, ax, seg);
	else if (strcmp(c, CMDLOCK) == 0) seglock(1, nnum);
	else if (strcmp(c, CMDFREE) == 0) seglock(2, nnum);
	else if (strcmp(c, CMDSLEEP) == 0) Sleep(to10(1, nnum, 16));
	WaitForSingleObject(hMutex, INFINITE);
	outio(1, ip, flag, ir, ax);
	ReleaseMutex(hMutex);
}
_endthreadex(0);
}

DWORD WINAPI run2(LPVOID lpParameter) {
	hMutex = CreateMutex(NULL, FALSE, NULL);
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
			outio(2, ip, flag, ir, ax);
			ReleaseMutex(hMutex);
			break;
		}
		else if (strcmp(c, CMDSEND) == 0) cmdsend(a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[0]) == 0) cal(0, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[1]) == 0) cal(1, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[2]) == 0) cal(2, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCAL[3]) == 0) cal(3, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[0]) == 0) logic(0, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[1]) == 0) logic(1, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDLOGIC[2]) == 0) logic(2, a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDCMP) == 0) flag = cmdcmp(a1, a2, ax, nnum, seg);
		else if (strcmp(c, CMDGOTO) == 0) cmdgoto(flag, a2, nnum, &ip);
		else if (strcmp(c, CMDIO[0]) == 0) io(0, 2, a1, ax, seg);
		else if (strcmp(c, CMDIO[1]) == 0) io(1, 2, a1, ax, seg);
		else if (strcmp(c, CMDLOCK) == 0) seglock(1, nnum);
		else if (strcmp(c, CMDFREE) == 0) seglock(2, nnum);
		else if (strcmp(c, CMDSLEEP) == 0) Sleep(to10(1, nnum, 16));
		WaitForSingleObject(hMutex, INFINITE);
		outio(2, ip, flag, ir, ax);
		ReleaseMutex(hMutex);
	}
	_endthreadex(0);
}

void cmdsend(char* a1, char* a2, char(*ax)[16], char* nnum, char seg[]) {
	int dest = to10(0, a1, 4);
	int src = to10(0, a2, 4); //send src to dest
	if (src == 0)	strncpy(ax[dest - 1], nnum, 16);
	else if (src >= 1 && src <= 4) strncpy(&seg[to10(1, ax[dest - 1], 16) * 8], ax[src - 1], 16);
	else if (src >= 5 && src <= 8) strncpy(ax[dest - 1], &seg[to10(1, ax[src - 1], 16) * 8], 16);
}

void cal(int cmdflag, char* a1, char* a2, char(*ax)[16], char* nnum, char seg[]) {
	int left = to10(0, a1, 4);//who need to change
	int right = to10(0, a2, 4);//what to change
	int i = 15, res = 0;
	char num1[16], num2[16], ans[16]; //in fact no mid-array in cpu , but it's too hard not to use it for pron.
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	memset(ans, '0', sizeof(ans));
	if (left == 0)	strncpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	strncpy(num1, ax[left - 1], 16);
	else if (left >= 5 && left <= 8) strncpy(num1, &seg[to10(1, ax[left - 1], 16) * 8], 16);
	if (right == 0)	strncpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	strncpy(num2, ax[right - 1], 16);
	else if (right >= 5 && right <= 8) strncpy(num2, &seg[to10(1, ax[right - 1], 16) * 8], 16);
	//get cal-result num(10)
	if (cmdflag == 0)
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
		res = -res;
	}
	while (res > 0 && i != 0) {
		ans[i] = (res % 2) + '0';
		res /= 2;
		i--;
	}
	if (left >= 1 && left <= 4)	strncpy(ax[left - 1], ans, 16);
	else if (left >= 5 && left <= 8) strncpy(&seg[to10(1, ax[left - 1], 16) * 8], ans, 16);
}

void logic(int cmdflag, char* a1, char* a2, char(*ax)[16], char* nnum, char seg[]) {
	int left = to10(0, a1, 4);//who need to change
	int right = to10(0, a2, 4);//what to change
	char num1[16], num2[16], ans[16]; //in fact no mid-array in cpu , but it's too hard not to use it for pron.
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	memset(ans, '0', sizeof(ans));
	if (left == 0)	strncpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	strncpy(num1, ax[left - 1], 16);
	else if (left >= 5 && left <= 8) strncpy(num1, &seg[to10(1, ax[left - 1], 16) * 8], 16);
	if (right == 0)	strncpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	strncpy(num2, ax[right - 1], 16);
	else if (right >= 5 && right <= 8) strncpy(num2, &seg[to10(1, ax[right - 1], 16) * 8], 16);

	if (cmdflag == 0) {
		int flag1 = 0, flag2 = 0;//like switch
		for (int i = 0; i < 16; i++) {
			if ((num1[i] == '1')) flag1++;
			if ((num2[i] == '1')) flag2++;
		}
		if (flag1 && flag2) ans[15] = '1';
	}
	else if (cmdflag == 1) {
		for (int i = 0; i < 16; i++) {
			if (num1[i] == '1' || num2[i] == '1') {
				ans[15] = '1';
				break;
			}
		}
	}
	else if (cmdflag == 2) {
		ans[15] = '1';
		if (left != 0) {
			for (int i = 0; i < 16; i++) {
				if (num1[i] == '1') {
					ans[15] = '0';
					break;
				}
			}
			strncpy(ax[left - 1], ans, 16);
		}
		else {
			for (int i = 0; i < 16; i++) {
				if (num2[i] == '1') {
					ans[15] = '0';
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

int cmdcmp(char* a1, char* a2, char(*ax)[16], char* nnum, char seg[]) {
	int left = to10(0, a1, 4);
	int right = to10(0, a2, 4);
	char num1[16], num2[16]; //in fact no mid-array in cpu , but it's too hard not to use it as pron.
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	int res = 0;
	if (left == 0)	strncpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	strncpy(num1, ax[left - 1], 16);
	else if (left >= 5 && left <= 8) strncpy(num1, &seg[to10(1, ax[left - 1], 16) * 8], 16);
	if (right == 0)	strncpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	strncpy(num2, ax[right - 1], 16);
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

void cmdgoto(int flag, char* a2, char* nnum, int* ip) {
	int cmd = to10(0, a2, 4); //whether change
	int d = to10(1, nnum, 16) - 4; //"-4" to balance main.c
	if (cmd == 0)	*ip += d;
	else if (cmd == 1 && flag == 0) *ip += d;
	else if (cmd == 2 && flag == 1) *ip += d;
	else if (cmd == 3 && flag == -1) *ip += d;
}

void io(int cmdflag, int threadid, char* a1, char(*ax)[16], char seg[]) {
	int tmp;
	char ans[16];
	memset(ans, '0', sizeof(ans));
	int left = to10(0, a1, 4);
	if (cmdflag == 0) {
		printf("id = %d    \nin:\n", threadid);
		scanf("%d", &tmp);
		int i = 15;
		if (tmp < 0) {
			ans[0] = '1';
			tmp = -tmp;
		}
		while (tmp > 0 && i != 0) {
			ans[i] = (tmp % 2) + '0';
			tmp /= 2;
			i--;
		}
		if (left <= 4) strncpy(ax[left - 1], ans, 16);
		else if (left > 4) strncpy(&seg[to10(1, ax[left - 1], 16) * 8], ans, 16);
	}
	else if (cmdflag == 1) {
		if (left <= 4)
			printf("id = %d    out: %d\n", threadid, to10(1, ax[left - 1], 16));
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

