#include <float.h>
#include "utils.h"
#include "viterbi.h"
#include "arm_math.h"

int argmax(float* array, int size, float* max_value) {

#ifdef USING_CMSIS_DSP_LIB
    printf("Using vector library to calculate argmax\n");
    // void    arm_max_f32 (float32_t *pSrc, uint32_t blockSize, float32_t *pResult, uint32_t *pIndex)
    int index = 0;
    arm_max_f32(array, size, max_value, &index);
    return index;
#else
    printf("Finding argmax of array with size %d\n", size);
    if (size == 0) {
        return 0;
    }

    int max_index = 0;
    *max_value = array[size - 1];
    for (int i = size - 2; i >= 0; i--) {
        // printf("Considering index %d with value %f and current max is %f\n", i, array[i], *max_value);
        if (array[i] >= *max_value) {
            *max_value = array[i];
            max_index = i;
        }
    }

    // printf("Found max value %f at index %d\n", *max_value, max_index);

    return max_index;
#endif
}

void array_scale_divide(float* array, int size, float scale_const) {

#ifdef USING_CMSIS_DSP_LIB
    // printf("Using vector library to scale down\n");
    //void  arm_scale_f32 (float32_t *pSrc, float32_t scale, float32_t *pDst, uint32_t blockSize)
    arm_scale_f32(array, 1.0 / scale_const, array, size);
#else
    for (int k = 0; k < size; k++) {
        array[k] = array[k] / scale_const;
    }
#endif
}

float get_index(float* array2D, int number_of_cols, int i, int j) {
    return array2D[number_of_cols * i + j];
}


void print_array(float* array1D, int number) {
    printf("Printing array of %d elements at %d --> ", number, array1D);
    for (int i = 0; i < number; ++i) {
        printf("%f --> ", array1D[i]); 
    }
    printf("\n");
}
