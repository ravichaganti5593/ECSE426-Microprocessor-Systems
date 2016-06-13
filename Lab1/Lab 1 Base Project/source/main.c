#include <stdio.h>
#include "arm_math.h"

#include "viterbi.h"
#include "test_1.h"
#include "utils.h"


int main()
{
    //printf("The quick brown fox jumps over the lazy dog.\n");

#ifdef VITERBI_ASM
    printf("Using ViterbiUpdate_asm\n");
#else
    printf("Using ViterbiUpdate_C\n");
#endif

int output[NOBS];
int error = Viterbi_C(vitTestArray, NOBS, output, &hmm1);
printf("Error is %d\n", error);
for (int i = 0; i < NOBS; i++) {
    printf(" --> %d", output[i]);
} printf("\n");

// #ifdef TEST_1
//     test1();
// #endif

// #ifdef TEST_2
//     test2();
// #endif

// #ifdef TEST_3
//     test3();
// #endif

// #ifdef TEST_4
//     test4();
// #endif

// #ifdef TEST_5
//     test5();
// #endif


// #ifdef TEST_6
//     test6();
// #endif

// #ifdef TEST_7
//     test7();
// #endif

// #ifdef TEST_UPDATE_1
//     test_update_1();
// #endif

// #ifdef TEST_UPDATE_2
//     test_update_2();
// #endif

// #ifdef TEST_UPDATE_3
//     test_update_3();
// #endif
        
    return 0;
}
