#ifndef VITERBI_H
#define VITERBI_H

#define S_DEF 4
#define V_DEF 3
#define NOBS  20

// #define VITERBI_ASM
#define USING_CMSIS_DSP_LIB

// #define TEST_1
// #define TEST_UPDATE_1

// #ifdef TEST_1
// #define S_DEF  2
// #define V_DEF  3
// #define NOBS   3
// #endif

// #ifdef TEST_2
// #define S_DEF  3
// #define V_DEF  4
// #define NOBS   0 //Don't care

// #endif
// #ifdef TEST_3 //hmm1
// #define S_DEF  4
// #define V_DEF  3
// #define NOBS   20
// #endif

// #ifdef TEST_4
// #define S_DEF  1
// #define V_DEF  1
// #define NOBS   1
// #endif

// #ifdef TEST_5
// #define S_DEF  4
// #define V_DEF  3
// #define NOBS   9
// #endif

// #ifdef TEST_6 //hmm2
// #define S_DEF  4
// #define V_DEF  3
// #define NOBS   20
// #endif

// #ifdef TEST_7 //hmm3
// #define S_DEF  4
// #define V_DEF  3
// #define NOBS   20
// #endif

// #ifdef TEST_UPDATE_1
// #define S_DEF  4
// #define V_DEF  3
// #define NOBS   1
// #endif

// #ifdef TEST_UPDATE_2
// #define S_DEF  4
// #define V_DEF  3
// #define NOBS   1
// #endif

// #ifdef TEST_UPDATE_3
// #define S_DEF  4
// #define V_DEF  3
// #define NOBS   1
// #endif

/**
* 
*
*
*
*/
typedef struct{
    int S; //number of states
    int V; //number of observation types
    float transition[S_DEF][S_DEF]; //S x S transition probability matrix
    float emission[S_DEF][V_DEF]; //S x V emission probability matrixd
    float prior[S_DEF]; //S x 1 prior probability matrix
} hmm_desc;

/**
* 
*
*
*
*/
#define ZERO_THRESHOLD 0.0000000001

/**
* 
*
*
*
*/
extern int ViterbiUpdate_asm(float* InputArray, float* OutputArray, int Observation, hmm_desc* hmm);
int ViterbiUpdate_C(float* InputArray, float* OutputArray, hmm_desc* hmm, int Observation);
int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, hmm_desc* hmm);
int accelometer(int accT, float* accObs, int* output, float alpha1, float alpha2, int N1, int N2, int N3);



#endif
