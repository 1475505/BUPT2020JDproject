#ifndef _INDEX_C_
#define _INDEX_C_

#define rep(x,y,z) for(int x=y ; x<z ; x++)
#define rrep(x,y,z) for(int x=y; x<=z ; x++)

#endif

void input(FILE* fp, int* code[], 16384) {
	while (!feof(fp) && i < 10 ) {
		fgets(code[i], sizeof(code[i]), fp);
		i++;
	}
}
