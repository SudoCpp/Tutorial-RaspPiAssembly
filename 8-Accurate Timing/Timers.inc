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
;                                  Timers for Accurate Timing                                       ;
; ------------------------------------------------------------------------------------------------- ;
; Designed for: Raspberry Pi 4 B                                                                    ;
; Processor: ARM Cortex-A72                                                                         ;
; ARM Instructions: https://developer.arm.com/documentation/dui0801/l?lang=en                       ;
; Technical Ref Manual: https://developer.arm.com/documentation/100095/latest/                      ;
; Peripherals Manual: https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf            ;
; ================================================================================================= ;
Timers:

; ================================================================================================= ;
;                                     Millisecond Delay                                             ;
; ------------------------------------------------------------------------------------------------- ;
    .MilliDelay:                    ; x0 - delay time in microseconds                               ;
        mrs x1, CNTPCT_EL0          ; https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTPCT-EL0--Counter-timer-Physical-Count-Register
        mrs x2, CNTFRQ_EL0          ; https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTFRQ-EL0--Counter-timer-Frequency-Register
        mov x3, 0xFFFFFFFF          ; Prepare bit mask (make sure only 32 bit are filled)           ;
        and x2, x2, x3              ; Mask                                                          ;
        mov x3, 1000                                                                                ;
        udiv x2, x2, x3             ; x2 = x2 / 1000 ; cycles per millisecond                       ;
        mul x2, x2, x0              ; total cycles that need to ellapse                             ;
        add x2, x1, x2                                                                              ;
        .MilliDelay.delayCheck:                                                                     ;
            mrs x1, CNTPCT_EL0                                                                      ; 
            cmp x2, x1                                                                              ;
        b.gt .MilliDelay.delayCheck                                                                 ;
    ret                                                                                             ;
; ================================================================================================= ;


; ================================================================================================= ;
;                                       Microsecond Delay                                           ;
; ------------------------------------------------------------------------------------------------- ;
    .MicroDelay:                    ; x0 - delay time in microseconds                               ;
        mrs x1, CNTPCT_EL0          ; https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTPCT-EL0--Counter-timer-Physical-Count-Register
        mrs x2, CNTFRQ_EL0          ; https://developer.arm.com/documentation/ddi0601/2024-12/AArch64-Registers/CNTFRQ-EL0--Counter-timer-Frequency-Register
        mov x3, 0xFFFFFFFF                                                                          ;
        and x2, x2, x3                                                                              ;
        mov x3, 1000                                                                                ;
        udiv x2, x2, x3             ; x2 = x2 / 1000 ; cycles per millisecond                       ;
        mul x2, x2, x0              ; We are multiplying here first so we don't lose too much precision
        udiv x2, x2, x3             ; x2 = x2 / 1000 ;  ellapse cycles in microseconds              ;
        add x2, x1, x2              ;total cycles                                                   ;
        .MicroDelay.delayCheck:                                                                     ;
            mrs x1, CNTPCT_EL0                                                                      ;
            cmp x2, x1                                                                              ;
        b.gt .MicroDelay.delayCheck                                                                 ;
    ret                                                                                             ;
; ================================================================================================= ;
