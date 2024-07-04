#pragma once

#ifdef _WIN64
#define ASSERT(expr,msg) \
if (!(expr)) \
{\
	DebugBreak();\
}
#else
#define ASSERT(expr,msg) \
if (!(expr)) \
{\
	__asm {\
		int 3\
	}\
}
#endif