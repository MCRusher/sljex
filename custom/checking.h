#ifndef CUSTOM_DEBUG_H
#define CUSTOM_DEBUG_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

//stickyAssert doesn't check the expr result
// when NDEBUG is defined, but expr is still evaluated
//EX: No NDEBUG: stickyAssert(0) => assert(0)
//	     NDEBUG: stickyAssert(0) => 0
#ifndef NDEBUG
	#define stickyAssert(expr)\
		assert(expr)
#else
	#define stickyAssert(expr)\
		expr
#endif

//a block of code that only exists in debug
#ifndef NDEBUG
	#define DEBUG_BLOCK(...) { __VA_ARGS__ }
#else
	#define DEBUG_BLOCK(...)
#endif

///takes a fmt string and variadics, prints to stderr and calls exit(EXIT_FAILURE)
#define panic(...) do{fprintf(stderr, __VA_ARGS__);exit(EXIT_FAILURE);}while(0)
///takes an exit code, a fmt string and variadics, prints to stderr and calls exit(code)
#define panicWithCode(code, ...) do{fprintf(stderr, __VA_ARGS__);exit(code);}while(0)

#define panicIf(expr, ...) do{ if((expr)){fprintf(stderr, __VA_ARGS__);exit(EXIT_FAILURE);} }while(0)

//provides debug traces with no release impact
#ifndef NDEBUG
	struct DEBUG_trace_TYPE {
		uintmax_t line;
		char const * file;
		char const * func;
	};
	//takes a name and a comma if needed and defines a variable/argument with that name
	//ex: void func(DTRACE_VAR(trace,) int v1, double v2);
	#define DTRACE_VAR(...) struct DEBUG_trace_TYPE __VA_ARGS__
	//get a trace instance representing the current trace context
	#define DTRACE_GET(...)\
		((struct DEBUG_trace_TYPE){.line = __LINE__, .file = __FILE__, .func = __func__}) __VA_ARGS__
	//panic using debug trace info
	#define DTRACE_panic(dtrace, ...)\
		do{\
			fprintf(stderr,\
				"DTRACE[line:%" PRIuMAX ",file:\"%s\",func:\"%s\"]:\n\t",\
				(dtrace).line, (dtrace).file, (dtrace).func\
			);\
			fprintf(stderr, __VA_ARGS__);exit(EXIT_FAILURE);\
		}while(0)
	#define DTRACE_panicWithCode(dtrace, code, ...)\
		do{\
			fprintf(stderr,\
				"DTRACE[line:%" PRIuMAX ",file:\"%s\",func:\"%s\"]:\n\t",\
				(dtrace).line, (dtrace).file, (dtrace).func\
			);\
			fprintf(stderr, __VA_ARGS__);exit(code);\
		}while(0)
#else
	#define DTRACE_VAR(...)
	#define DTRACE_GET(...)
	//still calls panic, but without the extra debug trace information
	#define DTRACE_panic(dtrace, ...) panic(__VA_ARGS__)
	#define DTRACE_panicWithCode(dtrace, code, ...) panicWithCode(code, __VA_ARGS__)
#endif

#endif