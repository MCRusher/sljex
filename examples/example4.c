//A copy of example1 that uses sljex_initNoCleanup instead
//Note that if catchany were not present and EXGENERIC were thrown,
// the program would exit before sljex_deinit
// could be called and leak memory

#include "../sljex.h"

#include <limits.h>
#include <stdio.h>

#define EXOVERFLOW (EXGENERIC + 1)

int add(int a, int b);//throws EXOVERFLOW

int main(void) {
    if(!sljex_initNoCleanup()){
        return 1;
    }
    
    int a = 1;
    try{
        //assignment to a only happens if add succeeds,
        // so a should still be valid if exception is thrown
        a = add(a, INT_MAX);
    }catch(EXOVERFLOW){
        printf(
            "caught EXOVERFLOW: \"%s\"\n",
            sljex_exstr()
        );
    }catchany{
        printf(
            "caught unexpected exception(%d): \"%s\"\n",
            sljex_excode(), sljex_exstr()
        );
    }finally;//cleans up exception memory
    
    printf("result is: %d\n", a);
    
    sljex_deinit();
}

int add(int a, int b) {
    if(((b > 0) && (INT_MAX - b < a))
    || ((b < 0) && (INT_MIN - b > a))){
        //throw(EXOVERFLOW);//implicit exstr is "EXOVERFLOW"
        throwWithMsg(EXOVERFLOW, "integer addition overflow");
    }
    return a + b;
}
