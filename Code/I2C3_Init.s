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
			EXPORT 		I2C3_Init
I2C3_Init   
			LDR R1, =SYSCTL_RCGCGPIO; Activate clock for port D 
			LDR R2, [R1]
			ORR R2, R2, #0x8
			STR R2, [R1]

			NOP
			NOP
			NOP
;wait_for_gpio
			;LDR R2, =SYSCTL_PRGPIO ; Wait for GPIO clock to be ready 
			;LDR R3, [R2]
			;CMP R3, #8
			;BNE wait_for_gpio

			
			LDR R1, =SYSCTL_RCGCI2C ; Activate I2C3's clock 
			LDR R2, [R1]
			ORR R2, R2, #0x8
			STR R2, [R1]

			
wait_for_I2C
			LDR R2, =SYSCTL_PRI2C ; Wait for I2C3 clock to be ready 
			LDR R3, [R2]
			CMP R3, #8
			BNE wait_for_I2C

			
			LDR R1, =GPIOD_DEN ; Digital enable for PD0 and PD1
			LDR R2, [R1]
			ORR R2, R2, #0x03
			STR R2, [R1]

			; Activate AFSEL for PORTD 
			
			LDR R1, =GPIOD_AFSEL
			LDR R2, [R1]
			ORR R2, R2, #0x03
			STR R2, [R1]

			; Configure PCTL register for I2C3 
			
			LDR R1, =GPIOD_PCTL
			LDR R2, [R1]
			ORR R2, R2, #0x33
			STR R2, [R1]

			; SDA (PD1) pin as open drain 
			
			LDR R1, =GPIOD_ODR
			LDR R2, [R1]
			ORR R2, R2, #0x02
			STR R2, [R1]

			; Enable I2C 3 master function 
			
			LDR R1, =I2C3_MCR
			MOV R2, #0x0010
			STR R2, [R1]

			; Set I2C3 MTPR 
			LDR R1, =I2C3_MTPR
			MOV R2, #0x07
			STR R2, [R1]

			BX 	LR  ; Return
			
			

;***************************************************************
; End of the program  section
;***************************************************************
;LABEL      DIRECTIVE       VALUE                           COMMENT
            ALIGN
            END
