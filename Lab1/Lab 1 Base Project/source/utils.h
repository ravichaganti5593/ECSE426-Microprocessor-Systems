#ifndef UTILS_H
#define UTILS_H

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;

int argmax(float* array, int size, float* max_value);
float get_index(float* array2D, int number_of_cols, int i, int j);
void array_scale_divide(float* array, int size, float scale_const);

void print_array(float* array1D, int number);

#endif
