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


int main(void) {
	char ir[16], seg[RAMBITS], c[5], a1[4], a2[4], nnum[16]; ; //存放当前处理的指令IR,内存,指令类型，AB寄存器，立即数（不保存，可更新）
	char ax[8][16];//通用寄存器状态
	memset(seg, '0', sizeof(seg));
	int flag = 0, ip = 0; //标志寄存器,指令计数器
	FILE* fp;
	fp = fopen("dict.dic", "r");
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
			cp(nnum, 16);//cp & cp = +-string(2)
			if (strcmp(c, CMDSHUTDOWN) == 0) {
				outio(ip, flag, ir, ax);
				output(seg);
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
			else if (strcmp(c, CMDIO[0]) == 0) io(0, a1, ax, seg);
			else if (strcmp(c, CMDIO[1]) == 0) io(1, a1, ax, seg);
			outio(ip, flag, ir, ax);
		}
	} else printf("Failed to Open File\n");
	fclose(fp);
	system("PAUSE");
	return 0;
}
