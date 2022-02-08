#define SLJEX_NOWARN
#include "../sljex.h"

#include <stdio.h>

int main(void) {
	if(!sljex_init()){
		return 1;
	}
	
	int num = 4;
	int const num2 = 4;//const is optional, just don't change it in the try block
	volatile int num3 = 4;
	static int num4 = 4;//not a local variable
	try{
		//num cannot be referenced outside the try block
		// after being modified within it if an exception is caught
		num = 6;
		//num2 is fine as long as it is not modified
		int num2_2 = num2 + 2;
		//no restrictions on num3
		num3 = 6;
		
		num4 = 6;
		
		throw(EXGENERIC);
	}catch(EXGENERIC){
		//undefined behavior, num should be volatile
		printf("num: %d\n", num);
		//allowed, num2 was not changed in the try block
		printf("num2: %d\n", num2);
		//allowed, num3 can be used after changing in a try block,
		printf("num3: %d\n", num3);
		
		printf("num4: %d\n", num4);
	}finally{
		//is undefined behavior if an exception was caught, num should be volatile
		printf("num: %d\n", num);
		//allowed, num3 can be used regardless if an exception was caught or not
		printf("num3: %d\n", num3);
	}
	//is undefined behavior if an exception was caught, num should be volatile
	printf("num: %d\n", num);
}
