#include <stdio.h>
#include "tool.h"

int to10(int flag, char* a, int len) {
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