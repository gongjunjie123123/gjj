

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



