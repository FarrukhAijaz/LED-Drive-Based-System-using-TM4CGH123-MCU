;***************************************************************   
;***************************************************************    
;*************************************************************** 
; EQU Directives
; These directives do not allocate memory
;***************************************************************
;LABEL      	DIRECTIVE   VALUE       COMMENT
SYSCTL_RCGCGPIO EQU 		0x400FE608
SYSCTL_PRGPIO   EQU 		0x400FEA08
SYSCTL_RCGCI2C  EQU 		0x400FE620
SYSCTL_PRI2C    EQU 		0x400FEA20
GPIOD_DEN       EQU 		0x4000751C
GPIOD_AFSEL     EQU 		0x40007420
GPIOD_PCTL      EQU 		0x4000752C
GPIOD_ODR       EQU 		0x4000750C
I2C3_MCR        EQU 		0x40023020
I2C3_MTPR       EQU 		0x4002300C	

I2C3_BASE       EQU 		0x40023000 
I2C3_MSA        EQU 		0x40023000
I2C3_MDR        EQU 		0x40023008
I2C3_MCS        EQU 		0x40023004
	
PowerOn			EQU			0x03
lowDataCH0		EQU			0x20002000
highDataCH0		EQU			0x20003000
lowDataCH1		EQU			0x20004000
highDataCH1		EQU			0x20005000
;***************************************************************
; Directives - This Data Section is part of the code
; It is in the read only section  so values cannot be changed.
;***************************************************************
;LABEL      DIRECTIVE   VALUE       COMMENT
            AREA        sdata, DATA, READONLY
            THUMB

;***************************************************************
; Program section                         
;***************************************************************
;LABEL      DIRECTIVE   VALUE       COMMENT
            AREA        main, READONLY, CODE
            THUMB
			EXPORT 		I2C3_Read
			EXTERN		I2C3_Error_Check



I2C3_Read
			;PUSH	{LR}
			MOV		R9, LR
			; Check if bytes_count <= 0
			CMP 	R3, #0
			BLE 	NO_READ             ; Branch if less than or equal to 0
			

; Send slave address and starting address

			LSL 	R1, R0, #1          ; Shift slave_address left by 1
			LDR 	R6, = I2C3_MSA
			STR 	R1, [R6]      	; Set I2C3_MSA with slave_address << 1
			
			LDR 	R6, = I2C3_MDR
			STR 	R2, [R6]      ; Set I2C3_MDR with slave_memory_address
			
			MOV 	R4, #3              ; Set R4 with the value 3 (S-(saddr+w)-ACK-maddr-ACK)
			LDR 	R6, = I2C3_MCS
			STR 	R4, [R6]      	; Set I2C3_MCS with 3
			;PUSH	{R0}
			MOV	R10, R0
			BL 		I2C3_Error_Check  	; Wait for the I2C operation to complete
			CBNZ 	R1, ERROR_EXIT     	; Check for error and exit if an error occurred
			;POP		{R0}
			MOV	R0, R10
			; Change bus from write to read (restart)
			
			LSL 	R1, R0, #1          ; Shift slave_address left by 1
			ADD 	R1, R1, #1          ; Set R1 to (slave_address << 1) + 1
			LDR 	R6, =I2C3_MSA
			STR 	R1, [R6]			; Set I2C3_MSA with restart configuration     

			; Check for different conditions of bytes count
			LDR 	R6, =I2C3_MCS
			CMP 	R3, #1
			ITE 	EQ
			MOVEQ 	R4, #7
			MOVNE 	R4, #0xB 
			STR 	R4, [R6]

			BL 		I2C3_Error_Check  ; Wait for the I2C operation to complete
			CBNZ 	R1, ERROR_EXIT     	; Check for error and exit if an error occurred

			; Read the first byte and store it
			LDR 	R6, =I2C3_MDR
			LDRB 	R5, [R6]     	; Load the received data byte into R5
			STRB 	R5, [R7],#1           	; Store the received data byte

			; Decrement bytes_count and check if it's zero
			SUBS 	R3, R3, #1         	; Decrement bytes_count by 1
			BEQ 	WAIT_BUS                ; Branch if bytes_count is zero

READ_LOOP
			; Read the rest of the bytes with ack
			MOV 	R4, #9              ; Set R4 with the value 9 (-data-ACK-)
			LDR 	R6, =I2C3_MCS
			STR 	R4, [R6]      ; Set I2C3_MCS with 9
			BL 		I2C3_Error_Check  ; Wait for the I2C operation to complete
			CBNZ 	R1, ERROR_EXIT      ; Check for error and exit if an error occurred

			; Read the byte and store it
			LDR 	R6, =I2C3_MDR
			LDRB 	R5, [R6]      		; Load the received data byte into R5
			STRB 	R5, [R7],#1         ; Store the received data byte

			; Decrement bytes_count and check if it's zero
			SUBS 	R3, R3, #1          ; Decrement bytes_count by 1
			BNE 	READ_LOOP           ; Branch back to READ_LOOP if bytes_count is not zero

			; Send a NACK and stop
			MOV 	R4, #5              ; Set R4 with the value 5 (-data-NACK-P)
			LDR 	R6, =I2C3_MCS
			STR 	R4, [R6]      		; Set I2C3_MCS with 5
			BL 		I2C3_Error_Check  ; Wait for the I2C operation to complete

			; Read the last byte and store it
			LDR 	R6, =I2C3_MDR
			LDRB 	R5, [R6]      		; Load the received data byte into R5
			STRB 	R5, [R7],#1            ; Store the received data byte

			
WAIT_BUS								; Wait until the bus is not busy
			LDR 	R6, =I2C3_MCS
			LDR 	R4, [R6]    		; Load the value of I2C3_MCS into R4
			TST 	R4, #0x40          	; Test the busy bit (bit 6)
			BNE 	WAIT_BUS		 	; Branch back if the bus is still busy

			MOV 	R0, #0				; Return 0 (no error)
			;POP		{LR}
			MOV		LR, R9
			BX 		LR


NO_READ		MOV 	R0, #-1 				; Return -1 (no read performed)
			;POP		{LR}
			MOV		LR, R9
			BX 		LR

ERROR_EXIT  ;POP		{LR}
			MOV		LR, R9
			BX 		LR 						; Return the error code
			
			

;***************************************************************
; End of the program  section
;***************************************************************
;LABEL      DIRECTIVE       VALUE                           COMMENT
            ALIGN
            END
