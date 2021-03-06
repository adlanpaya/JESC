;**** **** **** **** ****
;
; BLHeli program for controlling brushless motors in helicopters and multirotors
;
; Copyright 2011, 2012 Steffen Skaug
; This program is distributed under the terms of the GNU General Public License
;
; This file is part of BLHeli.
;
; BLHeli is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; BLHeli is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with BLHeli.  If not, see <http://www.gnu.org/licenses/>.
;
;**** **** **** **** ****
;
; Hardware definition file "N"
; X  X  RC X  MC MB MA CC    X  X  Cp Cc Bp Bc Ap Ac
;
;**** **** **** **** ****



;*********************
; Device SiLabs EFM8BB1x/2x
;*********************
IF MCU_48MHZ == 0
$include (SI_EFM8BB1_Defs.inc)
ELSE
$include (SI_EFM8BB2_Defs.inc)
ENDIF



TEMP_LIMIT			EQU 	49	; Temperature measurement ADC value for which main motor power is limited at 80degC (low byte, assuming high byte is 1)
TEMP_LIMIT_STEP		EQU	9	; Temperature measurement ADC value increment for another 10degC

;**** **** **** **** ****
; Bootloader definitions
;**** **** **** **** ****
RTX_PORT		  		EQU 	P0		; Receive/Transmit port
RTX_MDOUT				EQU	P0MDOUT	; Set to 1 for PUSHPULL				
RTX_MDIN				EQU	P0MDIN	; Set to 1 for DIGITAL				
RTX_SKIP				EQU	P0SKIP	; Set to 1 for SKIP				
RTX_PIN				EQU 	5		; RTX pin

SIGNATURE_001			EQU	0E8h		; Device signature
IF MCU_48MHZ == 0
SIGNATURE_002			EQU	0B1h
ELSE
SIGNATURE_002			EQU	0B2h
ENDIF


;*********************
; PORT 0 definitions *
;*********************  
;			EQU	7	;i
;			EQU	6	;i
Rcp_In		EQU	5	;i
;			EQU	4	;i 
Mux_C		EQU	3	;i
Mux_B		EQU	2	;i
Mux_A		EQU	1	;i
Comp_Com		EQU	0	;i

P0_DIGITAL	EQU	NOT((1 SHL Mux_A)+(1 SHL Mux_B)+(1 SHL Mux_C)+(1 SHL Comp_Com))
P0_INIT		EQU	0FFh
P0_PUSHPULL	EQU	0
P0_SKIP		EQU	0FFh

Set_Pwm_Polarity MACRO
IF FETON_DELAY == 0
	mov	PCA0POL, #00h				; Pwm noninverted
ELSE
	mov	PCA0POL, #01h				; Damping inverted, pwm noninverted
ENDIF
ENDM
Enable_Power_Pwm_Module MACRO
IF FETON_DELAY == 0
	mov	PCA0CPM0, #4Ah				; Enable comparator of module, enable match, set pwm mode
ELSE
	mov	PCA0CPM1, #42h				; Enable comparator of module, set pwm mode
ENDIF
ENDM
Enable_Damp_Pwm_Module MACRO
IF FETON_DELAY == 0
	mov	PCA0CPM1, #00h				; Disable
ELSE
	mov	PCA0CPM0, #42h				; Enable comparator of module, set pwm mode
ENDIF
ENDM
Clear_COVF_Interrupt MACRO
	anl	PCA0PWM, #0DFh
ENDM
Clear_CCF_Interrupt MACRO	; CCF interrupt is only used for FETON_DELAY == 0
	anl	PCA0CN0, #0FEh
ENDM
Enable_COVF_Interrupt MACRO
	orl	PCA0PWM, #40h
ENDM
Enable_CCF_Interrupt MACRO
	orl	PCA0CPM0,#01h
ENDM
Disable_COVF_Interrupt MACRO
	anl	PCA0PWM, #0BFh
ENDM
Disable_CCF_Interrupt MACRO
	anl	PCA0CPM0,#0FEh
ENDM


;*********************
; PORT 1 definitions *
;*********************
;			EQU	7	;i
;			EQU	6	;i
CpwmFET		EQU	5	;o
CcomFET		EQU	4	;o
BpwmFET		EQU	3	;o
BcomFET		EQU	2	;o
ApwmFET		EQU	1	;o
AcomFET		EQU	0	;o

P1_DIGITAL	EQU	(1 SHL ApwmFET)+(1 SHL BpwmFET)+(1 SHL CpwmFET)+(1 SHL AcomFET)+(1 SHL BcomFET)+(1 SHL CcomFET)
P1_INIT		EQU 	00h		
P1_PUSHPULL	EQU	(1 SHL ApwmFET)+(1 SHL BpwmFET)+(1 SHL CpwmFET)+(1 SHL AcomFET)+(1 SHL BcomFET)+(1 SHL CcomFET)
P1_SKIP		EQU	3Fh

ApwmFET_on MACRO
	setb	P1.ApwmFET
IF FETON_DELAY == 0
	setb	P1.AcomFET
ENDIF
ENDM
ApwmFET_off MACRO
IF FETON_DELAY != 0
	clr	P1.ApwmFET
ELSE
	clr	P1.AcomFET
ENDIF
ENDM
BpwmFET_on MACRO
	setb	P1.BpwmFET
IF FETON_DELAY == 0
	setb	P1.BcomFET
ENDIF
ENDM
BpwmFET_off MACRO
IF FETON_DELAY != 0
	clr	P1.BpwmFET
ELSE
	clr	P1.BcomFET
ENDIF
ENDM
CpwmFET_on MACRO
	setb	P1.CpwmFET
IF FETON_DELAY == 0
	setb	P1.CcomFET
ENDIF
ENDM
CpwmFET_off MACRO
IF FETON_DELAY != 0
	clr	P1.CpwmFET
ELSE
	clr	P1.CcomFET
ENDIF
ENDM
All_pwmFETs_Off MACRO
IF FETON_DELAY != 0
	clr	P1.ApwmFET
	clr	P1.BpwmFET
	clr	P1.CpwmFET
ELSE
	clr	P1.AcomFET
	clr	P1.BcomFET
	clr	P1.CcomFET
ENDIF
ENDM

AcomFET_on MACRO
IF FETON_DELAY == 0
	clr	P1.ApwmFET
ENDIF
	setb	P1.AcomFET
ENDM
AcomFET_off MACRO
	clr	P1.AcomFET
ENDM
BcomFET_on MACRO
IF FETON_DELAY == 0
	clr	P1.BpwmFET
ENDIF
	setb	P1.BcomFET
ENDM
BcomFET_off MACRO
	clr	P1.BcomFET
ENDM
CcomFET_on MACRO
IF FETON_DELAY == 0
	clr	P1.CpwmFET
ENDIF
	setb	P1.CcomFET
ENDM
CcomFET_off MACRO
	clr	P1.CcomFET
ENDM
All_comFETs_Off MACRO
	clr	P1.AcomFET
	clr	P1.BcomFET
	clr	P1.CcomFET
ENDM

Set_Pwm_A MACRO
IF FETON_DELAY == 0
	setb	P1.AcomFET
	mov	P1SKIP, #3Dh
ELSE
	mov	P1SKIP, #3Ch
ENDIF
ENDM
Set_Pwm_B MACRO
IF FETON_DELAY == 0
	setb	P1.BcomFET
	mov	P1SKIP, #37h
ELSE
	mov	P1SKIP, #33h
ENDIF
ENDM
Set_Pwm_C MACRO
IF FETON_DELAY == 0
	setb	P1.CcomFET
	mov	P1SKIP, #1Fh
ELSE
	mov	P1SKIP, #0Fh
ENDIF
ENDM
Set_Pwms_Off MACRO
	mov	P1SKIP, #3Fh
ENDM

Set_Comp_Phase_A MACRO
	mov	CMP0MX, #10h	; Set comparator multiplexer to phase A
ENDM
Set_Comp_Phase_B MACRO
	mov	CMP0MX, #20h	; Set comparator multiplexer to phase B
ENDM
Set_Comp_Phase_C MACRO
	mov	CMP0MX, #30h	; Set comparator multiplexer to phase C
ENDM
Read_Comp_Out MACRO
	mov	A, CMP0CN0	; Read comparator output
ENDM


;*********************
; PORT 2 definitions *
;*********************
DebugPin		EQU	0	;o

P2_PUSHPULL	EQU	(1 SHL DebugPin)


;**********************
; MCU specific macros *
;**********************

Initialize_Xbar MACRO
	mov	XBR2, #40h	; Xbar enabled			
	mov	XBR1, #02h	; CEX0 and CEX1 routed to pins		
ENDM

Initialize_Comparator MACRO
	mov	CMP0CN0, #80h	; Comparator enabled, no hysteresis
	mov	CMP0MD, #00h	; Comparator response time 100ns
ENDM
Initialize_Adc MACRO
	mov	REF0CN, #0Ch	; Set vdd (3.3V) as reference. Enable temp sensor and bias
IF MCU_48MHZ == 0
	mov	ADC0CF, #59h	; ADC clock 2MHz, PGA gain 1
ELSE
	mov	ADC0CF, #0B9h	; ADC clock 2MHz, PGA gain 1
ENDIF
	mov	ADC0MX, #10h	; Select temp sensor input
	mov	ADC0CN0, #80h	; ADC enabled 
	mov	ADC0CN1, #01h	; Common mode buffer enabled
ENDM
Start_Adc MACRO
	mov	ADC0CN0, #90h	; ADC start
ENDM
Read_Adc_Result MACRO
	mov	Temp1, ADC0L
	mov	Temp2, ADC0H
ENDM
Stop_Adc MACRO
ENDM
Set_RPM_Out MACRO
ENDM
Clear_RPM_Out MACRO
ENDM
Set_LED_0 MACRO
ENDM
Clear_LED_0 MACRO
ENDM
Set_LED_1 MACRO
ENDM
Clear_LED_1 MACRO
ENDM
Set_LED_2 MACRO
ENDM
Clear_LED_2 MACRO
ENDM
Set_LED_3 MACRO
ENDM
Clear_LED_3 MACRO
ENDM
