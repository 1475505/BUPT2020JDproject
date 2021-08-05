#include <stdio.h>
#include "tool.c"
#include "out.h"
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
void outio(int id, int ip, int flag, char* ir, char(*ax)[16]) {
	printf("id = %d\n", id);
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
