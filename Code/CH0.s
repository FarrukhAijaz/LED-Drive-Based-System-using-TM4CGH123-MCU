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
;*************************************************************** 
; EQU Directives
; These directives do not allocate memory
;***************************************************************
;LABEL      DIRECTIVE   VALUE       COMMENT
OFFSET      EQU         0x10
FIRST       EQU         0x20000400  
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
            EXTERN      OutStr      ; Reference external subroutine 
			EXTERN		I2C_WAIT_TILL_DONE
			EXTERN		I2C3_Init
			EXTERN		I2C3_Read
			EXTERN		I2C3_WRITE_MULTIPLE
            EXPORT      CH0      ; Make available

CH0			; Initialise conditions for Read Multiple for low Data Ch0
			
			;PUSH	{LR}
			MOV		R11, LR
			
			LDR 	R0, =0x39         	; Load slave_address into R0
			LDR 	R2, =0x8C         	; Load slave_memory_address into R2
			LDR 	R3, =1            	; Load bytes_count into R3
			LDR 	R7, =lowDataCH0   	; Load address of data array into R4
			
			MOV		R9, LR
			BL		I2C3_Read
			
			MOV		R9, #0
			ADD		R9, R5
			
			
			; Initialise conditions for Read Multiple for high Data Ch0
			
			LDR 	R0, =0x39         	; Load slave_address into R0
			LDR 	R2, =0x8D         	; Load slave_memory_address into R2
			LDR 	R3, =1            	; Load bytes_count into R3
			LDR 	R7, =highDataCH0	; Load address of data array into R4
			
			BL		I2C3_Read
			
			MOV		R10, #0
			ADD		R10, R5
			
			MOV		R0,#256
			MUL		R0, R10
			ADD		R0, R9
			
			;POP		{LR}
			MOV		LR,	R11
			BX 		LR
;***************************************************************
; End of the program  section
;***************************************************************
;LABEL      DIRECTIVE       VALUE                           COMMENT
            ALIGN
            END
