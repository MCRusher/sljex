# SLJEX (SetJump LongJump EXceptions) Library

A (believed to be) C99 standards compliant, thread-safe C exceptions library

# Building

`make`
`make examples`

# Installation

`make install`

# Basic usage

* call sljex_init() exactly once, before using any of the library's features.
* sljex is deinitialized on normal termination using atexit.
* try, catch, catchany, can all take either a block or a single statement.
* finally is a mandatory ending keyword that automatically cleans up the exception state and checks for unhandled exceptions
* exceptions are thread-local, so you cannot catch exceptions from other threads.
* the exstr value is used as-is, no allocated copy is performed.
* using throwWithMsg, exstr is equal to the message passed, using throw, exstr is equal to the excode argument stringized
  * (throw(EXGENERIC) = {.excode = EXGENERIC, .exstr = "EXGENERIC"})
* sljex_excode and sljex_exstr can be used (only) inside catch/catchany blocks to get the excode and accompanying exstr of the caught exception.
  * Calling them outside these blocks will exit the program with an error.
* using rethrow outside of catch/catchany will exit the program with an error.

# Important considerations

* sljex is subject to the limitations of setjmp & longjmp, therefore (since try calls setjmp,) modifying any non-volatile local variables in the try block renders them inaccessible if an exception is caught (, which calls longjmp).
EX:
```C
    int i = 4;
	try{
		i = 6;/*makes i invalid outside of try block if catchany executes*/
		throw(EXGENERIC);
	}catchany{
		i = 4;/*undefined behavior*/
	}finally;
	i = 5;/*undefined behavior if an exception was caught above*/
```

* new exception type values should always be greater than EXGENERIC, and can never be 0.
* `finally` exists to manage exception states and panic when exceptions go unhandled, and will not execute the following block when a catch is returned from, unlike in C#.
  * (finally{} == finally;{})
* exceptions automatically propagate through functions only when outside of a try block.
  * If any exception is uncaught in a try block, the program will exit and report an unhandled exception. Exceptions must be explicitly rethrown in this case.
EX:
```C
	#define EXOTHER (EXGENERIC + 1)
	void func1(void){
		throw(EXGENERIC);
	}
	//will exit the program since EXGENERIC goes unhandled
	void func2(void) {
		try{
			func1();
		}catch(EXOTHER){
			return;
		}finally;
	}
	//rethrows any unexpected exceptions
	//to propagate to another handler
	void func2_fixed(void) {
		try{
			func1();
		}catch(EXOTHER){
			return;
		}catchany{
			rethrow;
		}finally;
	}
```

* the library is deinitialized using atexit, and thus should not be loaded using dlopen (unix), LoadLibrary (win32), or similar, which may unload the library before calling atexit (will likely SIGSEGV)

# Implementation Notes

Since exception memory is cleaned up in only 3 conditions:

1. start of finally block
2. when throw is used
3. when program exits normally (deinits entire library)
X4. start of try block (probably could, doesn't)

Catching an exception in a function and returning from the function inside the catch block results in an allocated exception state being marked as used, but not yet released until one of the 3 conditions occur.

Therefore returning from a catch results in a temporary fixed-size memory leak, but it is cleaned up when one of the 3 conditions occur.
