#define SLJEX_NOWARN
#include "../sljex.h"

#include <limits.h>
#include <stdio.h>

#define EXOVERFLOW (EXGENERIC + 1)
#define EXNEGATIVE (EXOVERFLOW + 1)

int add(int a, int b) {
	if((b > 0) && (INT_MAX - b < a)
	|| (b < 0) && (INT_MIN - b > a)){
		throw(EXOVERFLOW);//implicit exstr is "EXOVERFLOW"
	}
	return a + b;
}

int add2(int a, int b) {
	if(a < 0){
		throw(EXNEGATIVE);
	}
	//add's EXOVERFLOW exception propagates through add2
	try{
		return add(a, add(b, b));
	}catch(EXGENERIC){
		return 0;
	}catchany{
		//allows EXOVERFLOW to propagate
		// through a try-finally handler
		rethrow;
	}finally;
}

int main(void) {
	if(!sljex_init()){
		return 1;
	}
	
	int a = 1;
	int b = INT_MAX;
	
	try{
		printf("%d + (%d * 2) = %d\n", a, b, add2(a, b));
	}catch(EXOVERFLOW){
		puts("the operation overflowed.");
	}catch(EXNEGATIVE){
		puts("a was negative.");
	}catchany{//without this, the program would exit from any unhandled exception
		printf(
			"caught unexpected exception \"%s\"(%d)\n",
			sljex_exstr(), sljex_excode()
		);
	}finally;
}
