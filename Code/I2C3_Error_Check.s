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
;***************************************************************
; Directives - This Data Section is part of the code
; It is in the read only section  so values cannot be changed.
;***************************************************************
;LABEL      DIRECTIVE   VALUE       COMMENT
            AREA        sdata, DATA, READONLY
            THUMB
CTR1        DCB         0x10
MSG         DCB         "Copying table..."
            DCB         0x0D
            DCB         0x04
;***************************************************************
; Program section                         
;***************************************************************
;LABEL      DIRECTIVE   VALUE       COMMENT
            AREA        main, READONLY, CODE
            THUMB
			EXPORT 		I2C3_Error_Check

I2C3_Error_Check                    ; Wait until the I2C master is not busy
		
WAIT_NOT_BUSY   
				LDR 	R1, =I2C3_MCS
				LDRB 	R0, [R1]    ; Load the value of I2C3_MCS into R0
				TST 	R0, #1            ; Test the busy bit (bit 0)
				BNE 	WAIT_NOT_BUSY     ; Branch back if the I2C master is still busy

			
				LDR 	R0, =I2C3_MCS ; Return the I2C error code (0 if no error)
				LDR		R1, [R0]
				AND 	R1, R1, #0xE
				
				BX 		LR

			
			

;***************************************************************
; End of the program  section
;***************************************************************
;LABEL      DIRECTIVE       VALUE                           COMMENT
            ALIGN
            END
