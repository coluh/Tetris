#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
 
int toInt(const char *string);

#define Rect(x, y, w, h) ((SDL_Rect){x, y, w, h})

#define inRect(mx, my, r) (((mx) >= (r).x) && ((mx) < (r).x+(r).w) \
		&& ((my) >= (r).y) && ((my) < (r).y+(r).h))


// err handling
#define Assert(condition, msg) do { \
	if (!(condition)) { \
		Error(msg); \
	} \
} while (0)

#define Debug(fmt, args...) do { \
	printf("\x1B[1;36mDebug: "); \
	printf(fmt, ##args); \
	printf("\x1B[0m\n"); \
	fflush(stdout); \
} while (0)

#define Warning(msg) do { \
	printf("\x1B[1;33mWarning:\x1B[0m %s\n", msg); \
	fflush(stdout); \
} while (0)

#define Error(msg) printError(msg, __FILE__, __LINE__, __FUNCTION__)
void printError(const char *msg, const char *file, int line, const char *function);

#endif // __UTILS_H__
