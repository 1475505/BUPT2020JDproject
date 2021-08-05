/*=====================================
CPU模拟器单核版本
注意：本代码在cpp格式下CE
@Date:2021/5/23
@Version:
@Verlog:
但是下面只是解决了re问题，结果全都WA0了...负数和传值出了新问题
*2.0 cp（取补码）取代antic函数
*1.11 单纯的strcmp处理负数是不对的 。版本修复了此bug
*1.1 解决了指令集部分未涉及的指令。比如输入对象A为内存单元 。
*1.02 debug:to10;
*1.01 启用cal PLAN B，后期debug A ，A代码在版本备份中
*1.0 全程使用了带符号二进制序列 应该会出现一系列负数的bug，
0,文件后面有中文会出错
2，cal中策略原码直接做加法 - 错误计算负数
1,按二进制序列处理乘除部分的思路被放弃。
本版本可能不支持一些指令集示例以外的指令，比如a1为地址寄存器
开发时将输出结果存入ans.dic，便于调试.
=====================================*/

#include<stdio.h>
#include <string.h>
#include<stdlib.h>
#define RAMBITS 32768*8+1
const char CMDSHUTDOWN[5] = "0000";
const char CMDSEND[5] = "0001";
const char CMDCAL[4][5] = {"0010", "0011", "0100", "0101"};   // + - * /
const char CMDLOGIC[3][5] = {"0110", "0111", "1000" };   //  &&  ||  not
const char CMDCMP[5] = "1001";
const char CMDGOTO[5] = "1010";
const char CMDIO[2][5] = {"1011", "1100" };

int to10(int, char a[], int len);
void cp(char*, int);
void input(FILE*, char*) ;
void send(char*, char*, char (*)[], char*, char*);
void cal(int, char*, char*, char (*)[], char*, char*);
void logic(int, char*, char*, char (*)[], char*, char*);
int cmdcmp(char*, char*, char (*)[], char*, char*);
void cmdgoto(int,  char*, char*, int*);
void io(int, char *, char (*)[], char*);
void outio(int, int, char*, char (*)[]);
void output(char* seg);
void mystrcpy(char *a, char *b, int);

int main(void) {
	char ir[16], seg[RAMBITS]; //存放当前处理的指令IR,内存
	char ax[8][16];//通用寄存器状态
	memset(seg, '0', sizeof(seg));
	int flag = 0, ip = 0; //标志寄存器,指令计数器
	char c[5], a1[4], a2[4], nnum[16];  //指令类型，AB寄存器，立即数（不保存，可更新）
	FILE* fp;
	fp = fopen("dict2.dic", "r");
	if (fp) {
		input(fp, seg);
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
			cp(nnum, 16);//nnum:code with +/-
			if (strcmp(c, CMDSHUTDOWN) == 0) {
				outio(ip, flag, ir, ax);
				output(seg);
				break;
			} else if (strcmp(c, CMDSEND) == 0) send(a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDCAL[0]) == 0) cal(0, a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDCAL[1]) == 0) cal(1, a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDCAL[2]) == 0) cal(2, a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDCAL[3]) == 0) cal(3, a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDLOGIC[0]) == 0) logic(0, a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDLOGIC[1]) == 0) logic(1, a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDLOGIC[2]) == 0) logic(2, a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDCMP) == 0) flag = cmdcmp(a1, a2, ax, nnum, seg);
			else if (strcmp(c, CMDGOTO) == 0) cmdgoto(flag, a2, nnum, &ip);
			else if (strcmp(c, CMDIO[0]) == 0) io(0, a1, ax, seg);
			else if (strcmp(c, CMDIO[1]) == 0) io(1, a1, ax, seg);
			outio(ip, flag, ir, ax);
			//output(seg);
		}
	} else printf("Failed to Open File\n");
	fclose(fp);
	return 0;
}

int to10(int flag, char a[], int len) {
	int ans = 0;
	int pow2 = 1;
	for (int i = len - 1; i >= 1; i--) {
		if (a[i] == '1') {
			ans += pow2;
		}
		pow2 *= 2;
	}
	if (flag == 1 && a[0] == '1')	ans = -ans;
	else if (flag == 0 && a[0] == '1')  ans += pow2;
	return ans;
}

void input(FILE * fp, char* seg ) {
	int i = 0;
	while (!feof(fp)) {
		char tmp = fgetc(fp);
		if (tmp == '\n')	tmp = fgetc(fp);
		seg[i] = tmp;
		i++;
	}
}

void send(char* a1, char* a2, char (*ax)[16], char* nnum, char* seg) {
	int dest = to10(0, a1, 4);
	int src = to10(0, a2, 4); //send src to dest
	if (src == 0)	mystrcpy(ax[dest - 1], nnum, 16);
	else if ( src >= 1 && src <= 4) mystrcpy(&seg[to10(1, ax[dest - 1], 16) * 8], ax[src - 1], 16);
	else if (src >= 5 && src <= 8) mystrcpy(ax[dest - 1], &seg[to10(1, ax[src - 1], 16) * 8], 16);
}

void cal(int flag, char* a1, char* a2, char (*ax)[16], char* nnum, char* seg) {
	int left = to10(0, a1, 4);//who need to change
	int right = to10(0, a2, 4);//what to change
	char num1[16], num2[16], ans[16]; //in fact no mid-array in cpu , but it's too hard not to use it for pron.
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	memset(ans, '0', sizeof(ans));
	if (left == 0)	mystrcpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	mystrcpy(num1, ax[left - 1], 16) ;
	else if (left >= 5 && left <= 8) mystrcpy(num1, seg + to10(1, ax[left - 1], 16) * 8, 16);
	if (right == 0)	mystrcpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	mystrcpy(num2, ax[right - 1], 16) ;
	else if (right >= 5 && right <= 8) mystrcpy(num2, seg + to10(1, ax[right - 1], 16) * 8, 16);
	//plan B
	if (flag == 0 ) {
		int i = 15;
		int res = to10(1, num1, 16) + to10(1, num2, 16);
		if (res < 0) {
			ans[0] = '1';
			res = - res;
		}
		while (res > 0 ) {
			ans[i] = (res % 2) + '0';
			res /= 2;
			i--;
		}
	} else if (flag == 1) {
		int i = 15;
		int res = to10(1, num1, 16) - to10(1, num2, 16);
		if (res < 0) {
			ans[0] = '1';
			res = - res;
		}
		while (res > 0) {
			ans[i] = (res % 2) + '0';
			res /= 2;
			i--;
		}
	} else if (flag == 2) { //乘和除直接处理二进制序列太难了，在此2->10->2
		int i = 15;
		int res = to10(1, num1, 16) * to10(1, num2, 16);
		if (res < 0) {
			ans[0] = '1';
			res = - res;
		}
		while (res > 0 ) {
			ans[i] = (res % 2) + '0';
			res /= 2;
			i--;
		}
	} else if (flag == 3) {
		int i = 15;
		int res = to10(1, num1, 16) / to10(1, num2, 16);
		if (res < 0) {
			ans[0] = '1';
			res = - res;
		}
		while (res > 0 ) {
			ans[i] = (res % 2) + '0';
			res /= 2;
			i--;
		}
	}
	if (left >= 1 && left <= 4)	mystrcpy( ax[left - 1], ans, 16) ;
	else if (left >= 5 && left <= 8) mystrcpy(seg + to10(1, ax[left - 1], 16) * 8, ans, 16);
}

void logic(int flag, char* a1, char* a2, char (*ax)[16], char* nnum, char* seg) {
	int left = to10(0, a1, 4);//who need to change
	int right = to10(0, a2, 4);//what to change
	char num1[16], num2[16], ans[16]; //in fact no mid-array in cpu , but it's too hard not to use it
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	memset(ans, '0', sizeof(ans));
	if (left == 0)	mystrcpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	mystrcpy(num1, ax[left - 1], 16) ;
	else if (left >= 5 && left <= 8) mystrcpy(num1, seg + to10(1, ax[left - 1], 16) * 8, 16);
	if (right == 0)	mystrcpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	mystrcpy(num2, ax[right - 1], 16) ;
	else if (right >= 5 && right <= 8) mystrcpy(num2, seg + to10(1, ax[right - 1], 16) * 8, 16);
	if (flag == 0) {
		int flag1 = 0, flag2 = 0;//like the switch
		for (int i = 0; i < 16; i++) {
			if ((num1[i] == '1')) flag1++;
			if ((num2[i] == '1')) flag2++;
		}

		if (flag1 && flag2) ans[15] = '1';
	} else if (flag == 1) {
		for (int i = 0; i < 16; i++) {
			if ((num1[i] == '1') || (num2[i] == '1'))	 {
				ans[15] = '1';
				break;
			}
		}
	} else if (flag == 2) {
		ans[15] = '1';
		if (left != 0) {
			for (int i = 0; i < 16; i++) {
				if (num1[i] == '1') {
					ans[15] = '0' ;
					break;
				}
			}
			mystrcpy(ax[left - 1], ans, 16);
		} else {
			for (int i = 0; i < 16; i++) {
				if (num2[i] == '1') {
					ans[15] = '0' ;
					break;
				}
			}
			mystrcpy(seg + to10(1, ax[right - 1], 16) * 8, ans, 16);
		}
		return;
	}
	if (left >= 1 && left <= 4) mystrcpy(ax[left - 1], ans, 16);
	else if (left >= 5 && left <= 8) mystrcpy(seg + to10(1, ax[right - 1], 16) * 8, ans, 16);
}

int cmdcmp(char* a1, char* a2, char (*ax)[16], char* nnum, char* seg) {
	int left = to10(0, a1, 4);
	int right = to10(0, a2, 4);
	char num1[16], num2[16]; //in fact no mid-array in cpu , but it's too hard not to use it
	memset(num1, '0', sizeof(num1));
	memset(num2, '0', sizeof(num2));
	int res = 0;
	if (left == 0)	mystrcpy(num1, nnum, 16);
	else if (left >= 1 && left <= 4)	mystrcpy(num1, ax[left - 1], 16) ;
	else if (left >= 5 && left <= 8) mystrcpy(num1, seg + to10(1, ax[left - 1], 16) * 8, 16);
	if (right == 0)	mystrcpy(num2, nnum, 16);
	else if (right >= 1 && right <= 4)	mystrcpy(num2, ax[right - 1], 16) ;
	else if (right >= 5 && right <= 8) mystrcpy(num2, seg + to10(1, ax[right - 1], 16) * 8, 16);
	for (int i = 0; i <= 15; i++) {
		if (num1[i] != num2[i])	res++;
	}
	if (res == 0) return 0;
	if (num1[0] == '1' && num2[0] == '0')	res = -1;
	else if (num1[0] == '0' && num2[0] == '1')	res = 1;
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

void io(int flag, char *a1, char (*ax)[16], char* seg) {
	int tmp;
	char ans[16] ;
	memset(ans, '0', sizeof(ans));
	int left = to10(0, a1, 4);
	if (flag == 0) {
		printf("in:\n");
		scanf("%d", &tmp);
		int i = 15;
		if (tmp < 0) {
			ans[0] = '1';
			tmp = - tmp;
		}
		while (tmp > 0) {
			ans[i] = (tmp % 2) + '0';
			tmp /= 2;
			i--;
		}
		if (left < 4) mystrcpy(ax[left - 1], ans, 16);
		else if (left >= 4  ) mystrcpy(seg + to10(1, ax[left - 1], 16) * 8, ans, 16);
	} else if (flag == 1) {
		if (left <= 4)
			printf("out: %d\n", to10(1, ax[left - 1], 16));
		else {
			printf("out: %d\n", to10(1, seg + to10(1, ax[left - 1], 16) * 8, 16));
		}
	}
}

void outio(int ip, int flag, char* ir, char (*ax)[16]) {
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

void output(char* seg) {
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

void mystrcpy(char *dest, char *src, int maxlen) {
	for (int i = 0; i < maxlen; i++) {
		*(dest + i) = *(src + i);
	}
}


void cp(char* a, int len) {//plan B
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
/*void antic(char* a, int len) {// plan B : c & c = yuanma
	if (a[0] == '1') {
		for (int i = 0; i < len; i++) {
			if (a[len - 1 - i] == '1')	{
				a[len - 1 - i]--;
				for (int j = 0; j < i; j++) {
					a[len - i + j] = '1';
				}
				break;
			}
		}
		for (int i = 1; i < len; i++) {
			if (a[i] == '1')	{
				a[i]--;
			} else {
				a[i]++;
			}
		}
	}
}*/
