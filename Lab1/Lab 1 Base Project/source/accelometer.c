#include <stdio.h>
#include "math.h"

int accelometer_check(int state, int step, int N1, int N2, int N3) {
    if (state == 0) {
        if (step > N1) {
            return 1;
        }
    } else if (state == 1) {
        if (step > N2) {
            return 2;
        }
    } else if (state == 2) {
        if (step > N3) {
            return 3;
        }
    }
    return 0;
}

int accelometer(int accT, float* accObs, int* output, float alpha1, float alpha2, int N1, int N2, int N3) {
    for (int i = 0; i < accT; i++) {
        float temp = accObs[i];
        if (fabs(temp) < alpha1 || fabs(temp) == alpha1) {
            output[i] = 0;
        }
        else if (temp > alpha2) {
            output[i] = 1;
        }
        else if (temp + alpha2 < 0) {
            output[i] = 2;
        }
        else {
            output[i] = 4;
        }
        // printf("%d --> %f\n", i, output[i]);
    }

    int outputIndex = 0;
    int step = 0;
    int state = -1;
    for (int i = 0; i < accT; ++i) {
        int currentState = output[i];
        // printf("state = %d, picked up %d, Step = %d so --> ", state, currentState, step);
        if (currentState != state && i != 0) {
            int condition = accelometer_check(state, step, N1, N2, N3);
            // printf("Condition is %d --> \n", condition);
            if (condition) {
                // printf("Different and udated --> %d\n", outputIndex);
                output[outputIndex] = state;
                // printf("array[%d] = %d\n", outputIndex, state);
                outputIndex++;
            }
            // else {
            //     printf("Differrent but not enough --> \n");
            // }
            step = 1;
        } else {
            // printf("Step is %d\n", step);
            step++;
        }
        state = currentState;
    }

    //Check for last element
    int condition = check(state, step, N1, N2, N3);
    if (condition) {
        // printf("Different and udated --> %d\n", outputIndex);
        output[outputIndex] = state;
        outputIndex++;
    } 
    // else {
    //     printf("Differrent but not enough --> \n");
    // }

    return outputIndex;
}

