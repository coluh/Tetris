#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>

#define null NULL

#define CSI_BLACK	"\x1B[1;30m"
#define CSI_RED		"\x1B[1;31m"
#define CSI_GREEN	"\x1B[1;32m"
#define CSI_YELLOW	"\x1B[1;33m"
#define CSI_BLUE	"\x1B[1;34m"
#define CSI_PURPLE	"\x1B[1;35m"
#define CSI_CYAN	"\x1B[1;36m"
#define CSI_WHITE	"\x1B[1;37m"
#define CSI_END		"\x1B[0m"
 
void shuffle(int *array, int n);
int toInt(const char *string);

typedef struct ArrayInt {
	int *data;
	int length;
} ArrayInt;

#define newArrayInt(p, len) (ArrayInt){ .data=p, .length=len };

#define Rect(x, y, w, h) ((SDL_Rect){x, y, w, h})

#define inRect(mx, my, r) (((mx) >= (r).x) && ((mx) < (r).x+(r).w) \
		&& ((my) >= (r).y) && ((my) < (r).y+(r).h))

char *copyString(const char *other);

/**
 * Error Handling
 * */
#define Assert(condition, msg) do { \
	if (!(condition)) { \
		Error(msg); \
	} \
} while (0)

#define Debug(fmt, args...) do { \
	printf(CSI_CYAN "Debug: %s: ", __FUNCTION__); \
	printf(CSI_END); \
	printf(fmt, ##args); \
	printf(CSI_END "\n"); \
	fflush(stdout); \
} while (0)

#define Warning(fmt, args...) do { \
	printf(CSI_YELLOW "Debug: %s: ", __FUNCTION__); \
	printf(CSI_END); \
	printf(fmt, ##args); \
	printf(CSI_END "\n"); \
	fflush(stdout); \
} while (0)

#define Error(msg) printError(msg, __FILE__, __LINE__, __FUNCTION__)
void printError(const char *msg, const char *file, int line, const char *function);

#endif // __UTILS_H__
