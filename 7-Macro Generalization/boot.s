; ================================================================================================= ;
;                                      Blinking Activity LED                                        ;
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

include "GPIO.inc"      ; Include our GPIO information

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
;                                           Main Code                                               ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: flags, x0, x1, x2                                                                           ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; Description: If debug is true, light turns on.                                                    ;
;    If debug is false, light will turn on and then turn off.                                       ;
;    If program has crashed, light will not turn on at all.                                         ;
; ------------------------------------------------------------------------------------------------- ;
main:                                                                                               ;
    GPIO_PinModeSetter 42,GPIO_PINMODE_OUTPUT   ; Set Activity LED as an Output                     ;
                                                                                                    ;
    blink:                                                                                          ;
        GPIO_SetOutputOn 42                     ; Turn LED on                                       ;
                                                                                                    ;
        bl startDelay                           ; Delay                                             ;
                                                                                                    ;
        GPIO_SetOutputOff 42                    ; Turn LED off                                      ;
                                                                                                    ;
        bl startDelay                           ; Delay                                             ;
    b blink                                     ; Loop                                              ;
; ================================================================================================= ;

; ================================================================================================= ;
;                                           Delay                                                   ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: x2                                                                                          ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; Description: Delays by looping through a very large number                                        ;
; ------------------------------------------------------------------------------------------------- ;
startDelay:                                                                                         ;
    mov x2, #0xFFFFFF           ; A very large number to delay on                                   ;
    delayLoop:                  ; Essentially a for loop. for (int x2 = 0xFFFFFF; x2 > 0; x2--)     ;
        sub x2, x2, 1           ; x2--                                                              ;
    cbnz x2, delayLoop          ; if x2 != 0 goto delayLoop                                         ;
ret                             ; Return to where called                                            ;
; ================================================================================================= ;