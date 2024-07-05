#pragma once
#if defined(_DEBUG)
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
#endif // _WIN64
#else // _DEBUG
#define ASSERT(expr,msg) 
#endif // _DEBUG