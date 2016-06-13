	AREA text, CODE, READONLY
		EXPORT ViterbiUpdate_asm

;int ViterbiUpdate_C(float* InputArray, float* OutputArray, hmm_desc* hmm, int Observation) {
;    int number_of_states = hmm->S;
;    u8 error = 0;
;    float sum = 0;
;
;    printf("#############################################\n");
;    LOOP1
;    for (int k = 0; k < number_of_states; k++) {
;        float p_observation_given_k = get_index(hmm->emission, hmm->V, k, Observation);
;        printf("Considering state %d with p_observation_given_k (%d, %d) is %f\n", k, k, Observation, p_observation_given_k);
;
;        /*void  arm_mult_f32 (float32_t *pSrcA, float32_t *pSrcB, float32_t *pDst, uint32_t blockSize)*/
;        LOOP2
;        float temporary[number_of_states];
;        for (int x = 0; x < number_of_states; x++) {
;            float value = InputArray[x] * get_index(hmm->transition, number_of_states, k, x);
;            printf("###### Evaluating %f * %f = %f\n", InputArray[x], get_index(hmm->transition, number_of_states, k, x), value);
;            temporary[x] = value;
;        }
;
;        float* max;
;        LOOP2_5
;        int max_index = argmax(temporary, hmm->S, max);
;        OutputArray[number_of_states + k] = max_index;
;
;        if (*max < 0) {
;            error = 1;
;        }
;        
;        OutputArray[k] = *max * p_observation_given_k;
;        printf("Max value shall be %f multiply by %f gives %f\n", *max, p_observation_given_k, OutputArray[k]);
;        sum += OutputArray[k];
;    }
;
;    printf("Sum would be %f\n", sum);
;    LOOP3
;    /*void  arm_scale_f32 (float32_t *pSrc, float32_t scale, float32_t *pDst, uint32_t blockSize)*/
;    for (int k = 0; k < number_of_states; k++) {
;        OutputArray[k] = OutputArray[k] / sum;
;    }
;    printf("Conclude for this iteration\n");
;    print_array(OutputArray, number_of_states);
;
;    return error;
;}

ViterbiUpdate_asm FUNCTION
	;We have 4, 5, 6, 7, 8, 10 and 11
	;Always use 10 & 11 as temp/ calculation var
	;R6 = error and S1 = sum and R10 = k and R11 = number_of_states
	PUSH {R4, R5, R6, R9, R10, R11}
	VSUB.F32 S1, S1
	VSUB.F32 S2, S2
	VSUB.F32 S5, S5
	VSUB.F32 S6, S6
	VSUB.F32 S11, S11
	VSUB.F32 S12, S12
	
	MOV R6, #0
	
	;R10 = k and R11 = number_of_states
	MOV R10, #0 ; R10 = k
	LDR R11, [R3] ; R11 = S = number_of_states
	B LOOP1
BACK1

	;S1 = sum
	VCMP.F32 S1, #0.0 ;if sum <= 0
	VMRS APSR_nzcv, FPSCR
	IT LE ; then
	MOVLE R6, #1 ; error = 1

	;R10 = k and R11 = number_of_states
	MOV R10, #0
	B LOOP3
BACK3
	;

	;MOV R4, R1
	MOV R0, R6
	
	POP {R4, R5, R6, R9, R10, R11}
	BX LR;


LOOP1 ;R6 = error and R10 = k and R11 = number_of_states
	CMP R10, R11
	BEQ BACK1
	;S12 = emission[k][Observation]
	; Now we have to calculate S * S to get pass emission array
	LDR R5, [R3]
	MUL R5, R5, R5 ; R5 = S * S
	LSL R5, #2 ; R5 = 4 S^2
	ADD R5, #8 ; Offset for S and V
	ADD R4, R3, R5 ;get emission matrix address
	LDR R5, [R3, #4] ; R5 = V
	MUL R5, R10, R5 ; R5 = k * V
	ADD R5, R5, R2 ; R5 = k * V + Observation
	ADD R5, R4, R5, LSL #2
	VLDR.F32 S12, [R5]; p_observation_given_k = get_index(hmm->emission, hmm->V, k, Observation);

	;#########################Matrix term by term multiplication####################
	MOV R9, #0
	ADD R4, R3, #8; R4 = hmm->transition
	VSUB.F32 S11, S11 ; *max = 0

	B LOOP2
BACK2
    ;#########################Check for error#######################################
	;S11 = *max and S12 = p_observation_given_k
	VCMP.F32 S11, #0.0 ;if *max < 0
	VMRS APSR_nzcv, FPSCR
	IT LT ; then
	MOVLT R6, #1 ; error = 1

	;#########################Assign value and accumulate sum ######################
	VMUL.F32 S2, S11, S12 ; temp = max * p_observation_given_k
	ADD R5, R1, R10, LSL #2

	VADD.F32 S1, S1, S2 ; sum = sum + temp
	VSTR.F32 S2, [R5] ; OutputArray[k] = temp
	

	ADD R10, #1 ; k++
	B LOOP1

	;#########################Matrix term by term multiplication####################
LOOP2 ; R9 = x and R10 = k and R11 = number_of_states and S5 = InputArray[x] and R4 = hmm->transition
	CMP R9, R11
	BEQ BACK2

	;S6 = value

	;S6 = get_index(hmm->transition, number_of_states, x, k);
	MUL R5, R9, R11; R5 = x * number_of_states
	ADD R5, R5, R10 ; R5 = x * number_of_states + k
	ADD R5, R4, R5, LSL #2 ; R5 = address of get_index(hmm->transition, number_of_states, x, k)
	VLDR.F32 S6, [R5] ; S6 = get_index(hmm->transition, number_of_states, x, k);

	;S5 = InputArray[x]
	LSL R5, R9, #2 ; R5 = 4 * x
	ADD R5, R0
	VLDR.F32 S5, [R5]

	VMUL.F32 S6, S5, S6 ; S6 = InputArray[x] * get_index(hmm->transition, number_of_states, x, k);

	VMOV.F32 S2, S6 ; next element in temp array
	VCMP.F32 S11, S2 ; max compare to current??
	VMRS APSR_nzcv, FPSCR
	BGE PASS; if max >= current, pass. Else
	VMOV.F32 S11, S2 ; max = current

	VMOV S6, R9; max_index = x
	VCVT.F32.S32 S6, S6 ; convert from int --> float

	ADD R5, R10, R11 ; R5 = k + number_of_states
	LSL R5, #2 ; R5 = R5 * 4
	ADD R5, R5, R1
	VSTR.F32 S6, [R5] ; OutputArray[number_of_states + k] = max_index

	
PASS
	ADD R9, #1
	B LOOP2


	;#########################Divide by sum ########################################
LOOP3 ;R10 = k and R11 = number_of_states and S1 = sum
	CMP R10, R11
	BEQ BACK3
	
	MOV R4, R10
	LSL R4, #2
	ADD R4, R1
	VLDR.F32 S2, [R4] ; temp =  OutputArray[k]
	VDIV.F32 S2, S2, S1 ; temp = temp / sum
	VSTR.F32 S2, [R4] ; OutputArray[k] = temp

	ADD R10, #1

	B LOOP3

	ENDFUNC
	END
