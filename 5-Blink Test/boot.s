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
;                                      Blinking An LED                                              ;
; ------------------------------------------------------------------------------------------------- ;
; Designed for: Raspberry Pi 4 B                                                                    ;
; Processor: ARM Cortex-A72                                                                         ;
; ARM Instructions [Doc1]: https://developer.arm.com/documentation/dui0801/l?lang=en                ;
; Technical Ref Manual [Doc2]: https://developer.arm.com/documentation/100095/latest/               ;
; Peripherals Manual [Doc3]: https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf     ;
; ================================================================================================= ;

; ================================================================================================= ;
;                                   Set up the Assembler
; ------------------------------------------------------------------------------------------------- ;
use64                   ; 64-Bit Code                                                               ;
processor cpu64_v8      ; Raspberry PI 4 Processor Type                                             ;
; ================================================================================================= ;

org 0x80000             ; Where our code is loaded into RAM. (0x8000 for 32-bit)

; ================================================================================================= ;
;                                   Shutdown All But Main Core                                      ;
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
;                                   Set Addresses and Offsets                                       ;
; ------------------------------------------------------------------------------------------------- ;
GPIO_BASE = 0xFE200000  ; We are using 0xFE000000 because of [Doc3] Page 6                          ;
                        ; Offset 0x200000 comes from [Doc3] Page 66                                 ;
                                                                                                    ;
GPFSEL2 = 0x8                                                                                       ;
;                 Pin Diagram                                                                       ;
;      29  28  27  26  25  24  23  22  21  20                                                       ;
; 0b00 000 000 000 000 000 000 000 000 001 000                                                      ;
                                                                                                    ;
GPSET0 = 0x1C                                                                                       ;
GPCLR0 = 0x28                                                                                       ;
; ================================================================================================= ;

; ================================================================================================= ;
;                                               Main Code                                           ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: flags, x0, x1, x2                                                                           ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; Description: Blink an LED on and off to show we have working code.                                ;
; ------------------------------------------------------------------------------------------------- ;
main:                                                                                               ;
    mov x0, GPIO_BASE           ; Set the Base Address for accessing the GPIO                       ;
    mov w1, 1                   ; Put a 1 at bit 0. ie 0b000 001                                    ;
    lsl w1, w1, 3               ; Shift the 1, 3 spots to the left. 0b003 000                       ;
    str w1, [x0, GPFSEL2]       ; Store value into GPIO_BASE + GPFSEL2                              ;
                                                                                                    ;
    blink:                      ; Essentially a while(true) loop.                                   ;
        mov w1, 0x200000        ; Put a 1 in the 21st spot.                                         ;
        str w1, [x0, GPSET0]    ; Store w1 to GPIO_BASE + GPSET0 (Turn LED on)                      ;
                                                                                                    ;
        bl startDelay           ; Run our delay                                                     ;
                                                                                                    ;
        mov w1, 0x200000        ; Technically not needed since w1 still persists from line 80       ;
        str w1, [x0, GPCLR0]    ; Store w1 to GPIO_BASE + GPCLR0 (Turn LED off)                     ;
                                                                                                    ;
        bl startDelay           ; Run our delay                                                     ;
    b blink                     ; Run loop again                                                    ; 
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
