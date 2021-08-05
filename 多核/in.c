#include <stdio.h>
#include "in.h"
// from in.c:
void input(FILE* fp, char* seg) {
	int i = 0;
	while (!feof(fp)) {
		char tmp = fgetc(fp);
		if (tmp == '\n')	tmp = fgetc(fp);
		seg[i] = tmp;
		i++;
	}
}