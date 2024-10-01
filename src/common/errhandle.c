#include "errhandle.h"
#include "utils.h"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

static void print_trace(const char *ss) {
	char *s = malloc(strlen(ss) + 1);
	strcpy(s, ss);		  // filename
	char *l = strchr(s, '('); // func
	char *a = strchr(s, '+'); // offset
	char *r = strchr(s, ')');
	char *m = strchr(s, '[');
	char *e = strchr(s, ']');
	if (!l || !a || !r || !m || !e)
		Error("Fix this");
	*l = *a = *r = *m = *e = 0;
	l++; s++; a++;
	if (*l == 0)
		l = "<lib-func>";
	printf("\x1B[0;31m" "\tat function " CSI_RED "%s(", l);
	printf("\x1B[4;36m" "%s:%s" CSI_RED ")", s, a);
	printf("\n" CSI_END);
}

void segv_handler(int sig) {
	void *info[16];
	int stack_num = backtrace(info, 16);
	printf(CSI_RED "Receive Signal 11, " "\x1B[1;37;41m" "Segmentation Fault" CSI_END ":\n");
	char **stack_trace = backtrace_symbols(info, stack_num);
	for (int i = 0; i < stack_num; i++) {
		/*printf("%s\n", stack_trace[i]);*/
		// format: exepath([func]+offset) [addr]
		print_trace(stack_trace[i]);
	}
	free(stack_trace);
	exit(1);
}
