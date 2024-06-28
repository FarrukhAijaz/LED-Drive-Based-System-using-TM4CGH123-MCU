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
			EXPORT 		I2C3_Write
			EXTERN		I2C3_Error_Check


I2C3_Write
			MOV		R9, LR
			; Check if bytes_count <= 0
			
			LDR 	R0, =0x39         	; Load slave_address into R0
			LDR 	R2, =0x80        	; Load slave_memory_address into R2
			LDR 	R3, =1            	; Load bytes_count into R3
			LDR 	R7, =PowerOn   		; Load value of data array into R4
			
			CMP 	R3, #0
			BLE 	NO_WRITE            ; Branch if less than or equal to 0
			

; Send slave address and starting address

			LSL 	R1, R0, #1          	; Shift slave_address left by 1
			LDR		R6,	=I2C3_MSA
			STR 	R1, [R6]      			; Set I2C3_MSA with slave_address << 1
			LDR		R6,	=I2C3_MDR
			STR 	R2, [R6]      			; Set I2C3_MDR with slave_memory_address
			
			MOV 	R4, #3              	; Set R4 with the value 3 (S-(saddr+w)-ACK-maddr-ACK)
			LDR		R6,	=I2C3_MCS	
			STR 	R4, [R6]      			; Set I2C3_MCS with 3
			
			BL 		I2C3_Error_Check  	; Wait for the I2C operation to complete
			CBNZ 	R1, ERROR_EXIT     		; Check for error and exit if an error occurred
			SUBS	R3, #1
			BEQ		DONE
			ADD		R3, #1

			; Send data one byte at a time

COUNT_GT_1
			LDR		R6,	=I2C3_MDR
			LDRB	R8, [R7],#1
			STRB 	R8, [R6]  			; Write the byte to I2C3_MDR
			
			MOV 	R4, #1           	; Set R4 with the value 1 (-data-ACK-)
			LDR		R6,	=I2C3_MCS
			STR 	R4, [R6]   			; Set I2C3_MCS with 1
			BL 		I2C3_Error_Check 	; Wait until write is complete
			CBNZ 	R1, ERROR_EXIT   	; Check for error and exit if an error occurred
			SUBS 	R3, R3, #1       	; Decrement bytes_count by 1
			BNE 	COUNT_GT_1 			; Branch back if bytes_count is not zero

			; Send last byte and a STOP
DONE		LDR		R6,	=I2C3_MDR			; Load the last byte from data pointer
			AND 	R8, R7, #0xFF
			STRB 	R8, [R6]  				; Write the last byte to I2C3_MDR
			MOV 	R4, #5                	; Set R4 with the value 5 (-data-ACK-P)
			LDR		R6,	=I2C3_MCS
			STR 	R4, [R6]        		; Set I2C3_MCS with 5
			BL 		I2C3_Error_Check     	; Wait until write is complete
			
WHILE_BUS_IS_BUSY
			
			LDR		R6,	=I2C3_MCS
			LDR 	R4, [R6]    			; Load the value of I2C3_MCS into R4
			TST 	R4, #0x40          		; Test the busy bit (bit 6)
			BNE 	WHILE_BUS_IS_BUSY  		; Branch back if the bus is still busy
			CBNZ 	R1, ERROR_EXIT       	; Check for error and exit if an error occurred

			MOV 	R0, #0					; Return 0 (no error)
			MOV		LR, R9
			BX		LR
			
NO_WRITE    MOV		LR, R9
			MOV 	R0, #-1 				; Return -1 (no write was performed)
			BX 		LR
			
ERROR_EXIT  MOV		LR, R9
			BX 		LR 						; Return the error code

			
			

;***************************************************************
; End of the program  section
;***************************************************************
;LABEL      DIRECTIVE       VALUE                           COMMENT
            ALIGN
            END
