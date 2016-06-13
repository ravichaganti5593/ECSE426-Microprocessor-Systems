#include "utils.h"
#include "viterbi.h"
#include "math.h"

float viterbi[NOBS][2 * S_DEF];

/** both the viterbiUpdate_C and viterbi_C methods are in this file
*   Viterbi_C part 2 for accelometer is in accelometer.c
*   @param viterbi          defining the matrix size as a global variable
*/


/** @param ViterbiUpdate_C  implementation of viterbi algorithm using just 1 observation 
*   @param InputArray       array vector vitpsi
*   @param OutputArray      array vector vitpsi which needs to be updated with next states after normalization with sums to obtain next states
*   @param hmm              calling struct type hmm_desc from viterbi.h
*   @param observation      contains the observation
*/

int ViterbiUpdate_C(float* InputArray, float* OutputArray, hmm_desc* hmm, int Observation) {        //this method is just for one observation
    int number_of_states = hmm->S;  //defining the number of states from hmm matrix
    u8 error = 0;
    float sum = 0;

    //printf("#############################################\n");

/** @param p_observation_given_k        stores the value from the emission matrix with which maximum probability needs to be normalized
*   @param max                          chooses the maximum probability after multiplying transition matrix's columns with previous state probabilities
*   @param max_index                    stores the max value's state index
*   @param sum                          adds all the probabilities of the current states 
*/

    for (int k = 0; k < number_of_states; k++) {
        float p_observation_given_k = get_index(&((hmm->emission)[0][0]), hmm->V, k, Observation);
        //printf("Considering state %d with p_observation_given_k (%d, %d) is %f\n", k, k, Observation, p_observation_given_k);

        float max = 0;
        int max_index = 0;
        for (int x = 0; x < number_of_states; x++) {
            float value = InputArray[x] * get_index(&((hmm->transition)[0][0]), number_of_states, x, k);
            //printf("###### Evaluating InputArray[%d] * transition[%d][%d] = %f * %f = %f\n", x, x, k, InputArray[x], get_index(&((hmm->transition)[0][0]), number_of_states, x, k), value);
            if (value >= max) {
                max = value;        //choosing the maximum probability which would be mulitplied by p_observation_given_k
                max_index = x;
            }
        }

        OutputArray[number_of_states + k] = max_index;

        if (max < 0) {
            error = 1;
        }
        
        if (max > ZERO_THRESHOLD) {
            OutputArray[k] = max * p_observation_given_k;   //output array storing the normalized value with respect to p_observation_given_k
            //printf("Max value shall be %f multiply by %f gives %f\n", max, p_observation_given_k, OutputArray[k]);
            sum += OutputArray[k];
        } else {
            //printf("Max = 0 \n");
            OutputArray[k] = 0;
            //printf("Max value shall be 0 multiply by %f gives 0\n", max, p_observation_given_k, OutputArray[k]);
        }
    }

    //printf("Sum would be %f\n", sum);
    if (sum > ZERO_THRESHOLD) {
        array_scale_divide(OutputArray, number_of_states, sum);  //storing the next states by calculating the ratios of the normalized values with respect to the sum
    } else {
        //printf("Not gonna divide by 0... Error\n");
        return 1; //Error
    }
    //printf("Conclude for this iteration\n");
    //print_array(OutputArray, number_of_states);

    return error;
}

/** @param Viterbi_C        implementation of Viterbi algorithm using multiple observations (int* observations)
*   @param Nobs             total number of observations 
*   @param Observations     array of all observation types 
*   @param EstimatedStates  updating next states by normalizing with the sum and with respect to all the observations
*/

int Viterbi_C(int* Observations, int Nobs, int* EstimatedStates, hmm_desc* hmm) {

/**@param viterbi       initializing viterbi matrix values to 0
*  @param sum           sum of all the normalized probabilities to calculate next states for all observations
*/

    for (int i = 1; i < Nobs; i++) {
        for (int j = 0; j < 2 * hmm->S; j++) {
            viterbi[i][j] = 0;
        }
    }
        
    float sum = 0;

    //printf("Observation 0 is %d\n", Observations[0]);
    for (int j = 0; j < hmm->S; j++) {
        viterbi[0][j] = hmm->prior[j] * get_index(&((hmm->emission)[0][0]), hmm->V, j, Observations[0]);    //multiplying prior array values with values of columns of emission matrix
        sum += viterbi[0][j];
    }

    if (fabs(sum) > ZERO_THRESHOLD) {
        array_scale_divide(viterbi[0], hmm->S, sum);        //dividing all the normalized maximum probabilities in the viterbi matrix with the sum of all its values
    }

/** repeating the calculations with all the values of observations to get next states
*   @param VITERBI_ASM          to check for the assembly program, calling function ViterbiUpdate_asm 
*   @param else                 to check for C program, calling function ViterbiUpdate_C
*/ 

    for (int i = 1; i < Nobs; i++) {
        //printf("-----> At iteration number %d with Observation %d\n", i, Observations[i]);
        //print_array(viterbi[i - 1], hmm->S);

        //printf("Array output address before is %x\n", viterbi[i]);
#ifdef VITERBI_ASM
        int return_value = ViterbiUpdate_asm(viterbi[i - 1], viterbi[i], Observations[i], hmm);   
#else
        int return_value = ViterbiUpdate_C(viterbi[i - 1], viterbi[i], hmm, Observations[i]);
#endif
        //printf("Array output address after is %x\n", viterbi[i]);
        // //printf("vitpsi is --> ");
        //print_array(viterbi[i], hmm->S);

        if (return_value != 0) {
            return return_value;
        }
    }

    float* max_value;
    EstimatedStates[Nobs - 1] = argmax(viterbi[Nobs - 1], hmm->S, max_value);
    for (int i = Nobs - 1; i > 0; i--) {
        EstimatedStates[i - 1] = viterbi[i][hmm->S + EstimatedStates[i]];       //storing the next state values from the viterbi matrix which has stored previous states
    }

    return 0;
}

/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/
/**********************************************************************************************************************/

/** @param accelometer              to process a stream of accelerometer readings to identify segments in terms of state transitions
*   @param accelometer_evaluate     to compare value with alpha1 and alpha2
*   @param value                    values of accelerometer observations           
*   @param alpha1                   to be compared with value 
*   @param alpha2                   to be compared with value
*   @param return                   returns values (0,1,2) after comparing value with alpha1 and alpha 2
*   There will be 3 states (0,1,2) and state 4 for all other cases which cannot be defined by the function (indexed lower by 1)
*/

int accelometer_evaluate(float value, float alpha1, float alpha2) {
    if (fabs(value) <= alpha1) {
        return 0;
    }
    else if (value > alpha2) {
        return 1;
    }
    else if (value + alpha2 < 0) {
        return 2;
    }
    else {
        return 4;
    }
}

/** @param accelometer_check        to verify the number of states being repeated after converting to values 0,1,2 and 4 from above function
*   @param step                     number of repeated states in a sequence after conversion using the above function
*   @param N1, N2, N3               to be compared with step 
*   @param return                   returns the state values 1, 2 and 3
*/

int accelometer_check(int state, int step, int N1, int N2, int N3) {
    if (state == 0) {
        if (step >= N1) {
            return 1;
        }
    } else if (state == 1) {
        if (step >= N2) {
            return 2;
        }
    } else if (state == 2) {
        if (step >= N3) {
            return 3;
        }
    }
    return 0;
}

/** @param accelometer          combining both the methods above to display the output array
*   @param currentState         evaluating the current state after converting with respect to alpha1 and alpha 2
*   @param condition            will check the states after calculating the number of steps with respect to N1, N2 and N3
*   @param 
*/

int accelometer(int accT, float* accObs, int* output, float alpha1, float alpha2, int N1, int N2, int N3) {
    int outputIndex = 0;
    int step = 0;
    int state = -1;
    for (int i = 0; i < accT; ++i) {
        int currentState = accelometer_evaluate(accObs[i], alpha1, alpha2);
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
            else {
                // printf("Differrent but not enough --> \n");
            }
            step = 1;
        } else {
            // printf("Step is %d\n", step);
            step++;
        }
        state = currentState;
    }

    //Check for last element
    int condition = accelometer_check(state, step, N1, N2, N3);
    if (condition) {
        // printf("Different and udated --> %d\n", outputIndex);
        output[outputIndex] = state;
        outputIndex++;
    } 
    else {
        // printf("Differrent but not enough --> \n");
    }

    return outputIndex;
}
