/*=================

@Date:
@Version:
@Verlog:

=================*/
#include<stdio.h>
#include <string.h>
#define rep(x,y,z) for(int x=y ; x<z ; x++)
#define rrep(x,y,z) for(int x=y; x<=z ; x++)
int main() {
	int flag;
	scanf("%d", &flag);
	char a[16];
	scanf("%s",a);
	int ans = 0;
	int pow2 = 1;
	for (int i = strlen(a) - 1; i >= 1; i--) {
		if (a[i] == '1') {
			ans += pow2;
		}
		pow2 *= 2;
	}
	if (flag == 1 && a[0] == '1')	ans = -ans;
	else if (flag == 0 && a[0] == '1')  ans += pow2;
	printf("%d",ans);
}

