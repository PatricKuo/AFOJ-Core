void write_log(int level, const char *fmt, ...) {
	va_list ap;
	char buffer[4096];
	time_t  timep = time(NULL);
	int l;
	struct tm *p;
    p = localtime(&timep);
    p->tm_year = p->tm_year + 1900;
    p->tm_mon = p->tm_mon + 1;
	sprintf(buffer,"log/Judge-%04d-%02d-%02d.log",p->tm_year, p->tm_mon, p->tm_mday);

	FILE *fp = fopen(buffer, "a+");
	if (fp == NULL) {
		fprintf(stderr, "open logfile error!\n");
		return;
	}
	
	fprintf(fp, "%s:%04d-%02d-%02d %02d:%02d:%02d ",LEVEL_NAME[level],p->tm_year, p->tm_mon, p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	if (DEBUG)
		printf("%s:%04d-%02d-%02d %02d:%02d:%02d ",LEVEL_NAME[level],p->tm_year, p->tm_mon, p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);

	va_start(ap, fmt);
	l = vsprintf(buffer, fmt, ap);
	fprintf(fp, "%s\n", buffer);
	if (DEBUG)
		printf("%s\n", buffer);
	va_end(ap);
	fclose(fp);
}