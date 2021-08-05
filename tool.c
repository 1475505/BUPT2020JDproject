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
