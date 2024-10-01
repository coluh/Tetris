#include "errhandle.h"
#include "utils.h"

#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

void segv_handler(int sig) {
	void *info[10];
	int size = backtrace(info, 10);
	Debug("RECEIVE SIGNAL %d:", sig);
	backtrace_symbols_fd(info, size, 2);
	exit(1);
}
