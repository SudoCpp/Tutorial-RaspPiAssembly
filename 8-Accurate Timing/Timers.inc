; ================================================================================================= ;
;                                      Blinking An LED                                              ;
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
