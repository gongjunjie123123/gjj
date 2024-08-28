

typedef struct {
	int lvl;
	char inf[16];
} vfio_log_t;

vfio_log_t g_vfio_loglvl[2] = {
	{VFIO_INF, "vfio-inf"},
	{VFIO_ERR, "vfio-err"},
};

enum{
	VFIO_INF = 0,
	VFIO_ERR,
};

#define	VFIO_PR(lvl, fmt, arg...)	vfio_pr(VFIO_##lvl, "[%s:%d] "fmt"\n", __FUNCTION__, __LINE__, ##arg)

char g_vfio_log_path[128];
void vfio_pr(int lvl, char *fmt, ...) {
	FILE *fp;
	va_list args;
	char log[512] = {0};
	char buf[1024];
	char time_buf[512];
	struct stat st;
	char log_na1[128];
	static int once = 0;
	char fname[128];
	
	if (once == 0) {
		once = 1;
		if (getcwd(g_vfio_log_path, 128)) {
			printf("ok");
		} else {
			printf("err");
			once = 0;
			return;
		}
	}
	va_start(args, fmt);
	vsprintf(log, fmt, args);
	va_end(args);
	
	sprintf(buf, "%s:%s", g_vfio_loglvl[lvl].lvl_str, log);
	sprintf(fname, "%s/%s", g_vfio_log_path, "vfio.log");
	fp = fopen(fname, "a+");
	if (fp == 0) {
		return;
	}
	stat(fname, &st);
	if(st.st_size > 2*1024) {
		fclose(fp);
		sprintf(log_na1, "%s%d", fname, 1);
		rename(fname, log_na1);
		fp = fopen(fname, "a+");
		if (fp == 0) {
			return;
		}
	}
	
	fwrite(buf, 1, strlen(buf), fp);
	fclose(fp);
}


int toint(char c) {
	if (c >= 'a' && c <= 'f') {
		return 10 + c - 'a';
	}
	if (c >= 'A' && c <= 'F') {
		return 10 + c - 'A';
	}
	if(c >= '0' && c <= '9') {
		return c - '0';
	}
	
	return -1;
}



uint64_t to64(char *hex) {
	uint64_t vv = 0;
	int digi;
	char ch;
	while((ch = *hex++)) {
		digi = toint(ch);
		if (-1 != digi) {
			vv = vv*16 + digi;
		} else {
			break;
		}
	}
	
	return vv;
}


uint64_t to64_prefix(char *exp) {
	char fst = *exp;
	char sec = *(exp + 1);
	if (fst == '0' && (sec == 'x') || (sec == 'X')) {
		return to64(exp + 2);
	}
	
	return to64(exp);
}


void docmd(char*cmd, int tp, int vdr, int dev, int *fl, int bar, uint64_t *pa) {
	FILE *fp = popen(cmd, "r");
	if (fp == 0) {
		return;
	}
	char buf[1024];
	char buf_t[2048];
	int line = 0;
	while(fgets(buf, sizeof(buf), fp)) {
		buf[strlen(buf) - 1] = 0;
		if (tp == 0) {
			if (fl[0] && fl[1]) {
				pclose(fp);
				return;
			}
			sprintf(buf_t, "cat /sys/bus/pci/devices/%s/vendor", buf);
			docmd(buf_t, 1, vdr, dev, fl, bar, pa);
			if (fl[0]) {
				sprintf(buf_t, "cat /sys/bus/pci/devices/%s/device", buf);
				docmd(buf_t, 2, vdr, dev, fl, bar, pa);
				if (fl[1] == 0) {
					fl[0] == 0;
				} else {
					sprintf(buf_t, "cat /sys/bus/pci/devices/%s/resource", buf);
					docmd(buf_t, 3, vdr, dev, fl, bar, pa);
				}
			}
		}
		else {
			if (tp == 1 && to64_prefix(buf) == vdr) {
				fl[0] = 1;
			}
			if (tp == 2 && to64_prefix(buf) == dev) {
				fl[1] = 1;
			}
			if (tp == 3 && line == bar) {
				*pa = to64_prefix(buf);
			}
		}
		
		line++;
	}
	
	pclose(fp);
}


void pci_trav(int bar, uint64_t *pa, int vendor, int device) {
	
	int fl[2] = {0};
	docmd("ls /sys/bus/pci/devices/", 0, vendor, device, fl, bar, pa);
}




