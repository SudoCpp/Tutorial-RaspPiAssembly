; Copyright 2025 SudoCPP

; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the “Software”), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
; of the Software, and to permit persons to whom the Software is furnished to do;
; so, subject to the following conditions:

; The above copyright notice and this permission notice shall be included in all
; copies or substantial portions of the Software.

; THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
; INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
; PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
; HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
; OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
; SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

; ================================================================================================= ;
;                            Blinking An LED with Accurate Timing                                   ;
; ------------------------------------------------------------------------------------------------- ;
; Designed for: Raspberry Pi 4 B                                                                    ;
; Processor: ARM Cortex-A72                                                                         ;
; ARM Instructions: https://developer.arm.com/documentation/dui0801/l?lang=en                       ;
; Technical Ref Manual: https://developer.arm.com/documentation/100095/latest/                      ;
; Peripherals Manual: https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf            ;
; ================================================================================================= ;

; ================================================================================================= ;
;                                   Set up the Assembler
; ------------------------------------------------------------------------------------------------- ;
use64                   ; 64-Bit Code                                                               ;
processor cpu64_v8      ; Raspberry PI 4 Processor Type                                             ;
; ================================================================================================= ;

org 0x80000             ; Where our code is loaded into RAM. (0x8000 for 32-bit)
b start                 ; Jump to start so our timer code is not run prematurely

include "GPIO.inc"
include "Timers.inc"

start:
; ================================================================================================= ;
;                                Shutdown All But Main Core                                         ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: flags, x0                                                                                   ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; Description: Suspends all cores except for Core 0                                                 ;
; ------------------------------------------------------------------------------------------------- ;
load_processor_core_id:                                                                             ;
    mrs x0, MPIDR_EL1   ; https://developer.arm.com/documentation/100095/0003/System-Control/AArch64-register-descriptions/Multiprocessor-Affinity-Register--EL1?lang=en
    and x0, x0, #0x3    ; x0 = x0 & 3 which gives us our code ID by itself                          ;
    cbz x0, main        ; goto main if core ID is 0                                                 ;
    hang_cpu:                                                                                       ;
        wfe             ; https://developer.arm.com/documentation/ka001283/latest/                  ;
    b hang_cpu                                                                                      ;
; ================================================================================================= ;


; ================================================================================================= ;
;                                        Main Code                                                  ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: flags, x0                                                                                   ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; Description: Blink our LED but with the abilityt to use millisecond and microsecond precision.    ;
; ------------------------------------------------------------------------------------------------- ;
main:                                                                                               ;
    GPIO_PinModeSetter 3,GPIO_PINMODE_OUTPUT    ; Set GPIO3 to an Output                            ;
                                                                                                    ;
    blink:                                      ; while (true) loop                                 ;
        GPIO_SetOutputOn 3                      ; Turn LED on                                       ;
                                                                                                    ;
        mov x0, 1000                            ; Delay for 1000 milliseconds                       ;
        bl Timers.MilliDelay                    ; Run delay                                         ;
                                                                                                    ;
        GPIO_SetOutputOff 3                     ; Turn LED off                                      ;
                                                                                                    ;
        mov x0, 1000                            ; Delay for 1000 milliseconds                       ;
        bl Timers.MilliDelay                    ; Run delay                                         ;
    b blink                                     ; loop                                              ;
; ================================================================================================= ;
