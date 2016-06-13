	AREA text, CODE, READONLY
		IMPORT ViterbiUpdate_asm
		IMPORT output_array
		EXPORT TEST_BED_ASM
	
TEST_BED_ASM
	LDR R0, =input_array
	LDR R1, =output_array
	LDR R3, =hmm_struct
	MOV R2, #1 ; Observation
	
	PUSH {LR}
	PUSH {R4-R11}
	LDR R4, =ViterbiUpdate_asm
	BLX R4
	
	POP {R4-R11}
	POP {LR}
	BX LR

	ALIGN
hmm_struct DCD 4, 3
transmission DCFS 0.09, 0.9, 0.01, 0, 0.09, 0.01, 0.9, 0, 0.09, 0.45, 0.01, 0.45, 0.5, 0.5, 0.0, 0.0
emission DCFS 1, 0, 0,  0, 1, 0,  0, 0, 1, 0, 1, 0
input_array DCD 0, 0, 1, 0, 0, 0, 0, 0

;expected 0.000000 --> 0.500000 --> 0.000000 --> 0.500000 --> 2.000000 --> 2.000000 --> 2.000000 --> 2.000000
			
		END