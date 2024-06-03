#include<stdio.h>
#include<string.h>

int arr[10] = {0};
int id = 0;

int toint(char f) {
	if (f >= '0' && f <= '9') return f - '0';
	if (f >= 'a' && f <= 'f') return f - 'a' + 10;
	if (f >= 'A' && f <= 'F') return f - 'A' + 10;
	return 0xff;
}

void split_str(char *s) {
	int len = strlen(s);
	int t = 0;
	int fl = 0;
	int i;
	int ret;

	for(i = 0; i <= len; i++) {
		char f = s[i];
		if (f == ' ' || f == 0) {
			if (fl == 0) {
				continue;
			} else {
				arr[id] = t;
				id++;
				t = 0;
				fl = 0;
				continue;
			}
		}
		
		fl = 1;
		
		ret = toint(f);
		if(ret == 0xff) {
		    arr[id++] = t;
		    return;
		} else {
		    t <<= 4;
		    t += ret;
		}
	}
}

int main(int argc, char **argv) {
	char *s = " f 5a5a 34ff 99d\t0  e\t";
	split_str(s);
	int i;
	for (i = 0; i < id; i++) {
		printf("arr[%d]=0x%x\n", i, arr[i]);
	}
	
	return 0;
}
