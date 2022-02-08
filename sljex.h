#ifndef SLJEX_H
#define SLJEX_H

#include <setjmp.h>
#include <stdbool.h>

///Basic exception code defined by default.
///All other exception codes must be greater than EXGENERIC
#define EXGENERIC 1

///Call before using sljex features to initialize library.
///Also adds sljex_deinit to atexit to deinit library on exit.
bool sljex_init(void);

///Fetches the code of the current exception.
///Panics if there is no current exception (outside catch/catchany).
int sljex_excode(void);
///Fetches the code of the current exception.
///Panics if there is no current exception (outside catch/catchany).
char const * sljex_exstr(void);

///Sets up an exception state to handle exceptions inside the following block.
///Must be followed by a finally block.
#define try\
	{{{{if(setjmp(sljex_trybuf_()) == 0)
///Executes the following block/statement if an exception matching EX is caught.
///Must follow a try block if used.
#define catch(EX)\
	else if(sljex_catch_(EX))
///executes the following block/statement if an exception is caught.
///Must follow a try block if used.
#define catchany\
	else if(sljex_catchany_())
///cleans up exception state and enforces exception checking.
///Must be precluded by a try block.
#define finally\
	}}}}sljex_finally_();
///Throws an exception code, using the stringized code as the message.
#define throw(EX)\
	longjmp(sljex_throwbuf_(EX, #EX), 1)
///Throws an exception code with an explicit message.
#define throwWithMsg(EX, Message)\
	longjmp(sljex_throwbuf_(EX, Message), 1)
///Rethrows the current exception.
///Used to explicitly propagate an exception through a try-finally.
///Panics if there is no current exception (outside catch/catchany).
#define rethrow\
	longjmp(sljex_rethrowbuf_(), 1)

//non-user functions wrapped with macros
void * sljex_trybuf_(void);
bool sljex_catch_(int excode);
bool sljex_catchany_(void);
void sljex_finally_(void);
void * sljex_throwbuf_(int excode, char const * exstr);
void * sljex_rethrowbuf_(void);

#endif