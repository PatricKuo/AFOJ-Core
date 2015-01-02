int isInFile(const char fname[]) {
	int l = strlen(fname);
	if (l <= 3 || strcmp(fname + l - 3, ".in") != 0)
		return 0;
	else
		return l - 3;
}

void find_next_nonspace(int & c1, int & c2, FILE *& f1, FILE *& f2, int & ret) {
	// Find the next non-space character or \n.
	while ((isspace(c1)) || (isspace(c2))) {
		if (c1 != c2) {
			if (c2 == EOF) {
				do {
					c1 = fgetc(f1);
				} while (isspace(c1));
				continue;
			} else if (c1 == EOF) {
				do {
					c2 = fgetc(f2);
				} while (isspace(c2));
				continue;
			} else if ((c1 == '\r' && c2 == '\n')) {
				c1 = fgetc(f1);
			} else {
				if (DEBUG)
					printf("%d=%c\t%d=%c", c1, c1, c2, c2);
				;
				ret = V_PE;
			}
		}
		if (isspace(c1)) {
			c1 = fgetc(f1);
		}
		if (isspace(c2)) {
			c2 = fgetc(f2);
		}
	}
}
/*
 * translated from ZOJ judger r367
 * http://code.google.com/p/zoj/source/browse/trunk/judge_client/client/text_checker.cc#25
 *
 */


int compare_zoj(const char *file1, const char *file2) {
	int ret = V_AC;
	FILE * f1, *f2;
	f1 = fopen(file1, "r");
	f2 = fopen(file2, "r");
	if (!f1 || !f2) {
		ret = V_RE;
	} else
		for (;;) {
			// Find the first non-space character at the beginning of line.
			// Blank lines are skipped.
			int c1 = fgetc(f1);
			int c2 = fgetc(f2);
			find_next_nonspace(c1, c2, f1, f2, ret);
			// Compare the current line.
			for (;;) {
				// Read until 2 files return a space or 0 together.
				while ((!isspace(c1) && c1) || (!isspace(c2) && c2)) {
					if (c1 == EOF && c2 == EOF) {
						goto end;
					}
					if (c1 == EOF || c2 == EOF) {
						break;
					}
					if (c1 != c2) {
						// Consecutive non-space characters should be all exactly the same
						ret = V_WA;
						goto end;
					}
					c1 = fgetc(f1);
					c2 = fgetc(f2);
				}
				find_next_nonspace(c1, c2, f1, f2, ret);
				if (c1 == EOF && c2 == EOF) {
					goto end;
				}
				if (c1 == EOF || c2 == EOF) {
					ret = V_WA;
					goto end;
				}

				if ((c1 == '\n' || !c1) && (c2 == '\n' || !c2)) {
					break;
				}
			}
		}
end: if (f1)
		 fclose(f1);
	 if (f2)
		 fclose(f2);
	 return ret;
}


void delnextline(char s[]) {
	int L;
	L = strlen(s);
	while (L > 0 && (s[L - 1] == '\n' || s[L - 1] == '\r'))
		s[--L] = 0;
}

int compare(const char *file1, const char *file2) {
	//compare ported and improved from zoj don't limit file size
	return compare_zoj(file1, file2);
}


//是否存在异常
BOOL RUN_existException(DWORD dw){
	switch(dw){
	case EXCEPTION_ACCESS_VIOLATION:
		return TRUE;
	case EXCEPTION_DATATYPE_MISALIGNMENT:   
		return TRUE;
	case EXCEPTION_BREAKPOINT:  
		return TRUE;
	case EXCEPTION_SINGLE_STEP:  
		return TRUE;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:     
		return TRUE;
	case EXCEPTION_FLT_DENORMAL_OPERAND:  
		return TRUE;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:  
		return TRUE;
	case EXCEPTION_FLT_INEXACT_RESULT: 
		return TRUE;
	case EXCEPTION_FLT_INVALID_OPERATION: 
		return TRUE;
	case EXCEPTION_FLT_OVERFLOW: 
		return TRUE;
	case EXCEPTION_FLT_STACK_CHECK:  
		return TRUE;
	case EXCEPTION_FLT_UNDERFLOW:   
		return TRUE;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:  
		return TRUE;
	case EXCEPTION_INT_OVERFLOW:   
		return TRUE;
	case EXCEPTION_PRIV_INSTRUCTION:    
		return TRUE;
	case EXCEPTION_IN_PAGE_ERROR:  
		return TRUE;
	case EXCEPTION_ILLEGAL_INSTRUCTION: 
		return TRUE;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: 
		return TRUE;
	case EXCEPTION_STACK_OVERFLOW:  
		return TRUE;
	case EXCEPTION_INVALID_DISPOSITION:
		return TRUE;
	case EXCEPTION_GUARD_PAGE:  
		return TRUE;
	case EXCEPTION_INVALID_HANDLE: 
		return TRUE;
	default:
		return FALSE;
	}          
}

