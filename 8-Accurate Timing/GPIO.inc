; ==============================================================================================
;                                      Blinking An LED
; ----------------------------------------------------------------------------------------------
; Designed for: Raspberry Pi 4 B
; Processor: ARM Cortex-A72
; ARM Instructions [Doc1]: https://developer.arm.com/documentation/dui0801/l?lang=en
; Technical Ref Manual [Doc2]: https://developer.arm.com/documentation/100095/latest/
; Peripherals Manual [Doc3]: https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
; ----------------------------------------------------------------------------------------------

GPIO_BASE = 0xFE200000  ; We are using 0xFE000000 because of [Doc3] Page 6
                        ; Offset 0x200000 comes from [Doc3] Page 66

GPIO_ACT_LED = 42       ;https://raspberrypi.stackexchange.com/questions/135867/bare-metal-led-access-on-rpi-4

; ================================================================================================= ;
;                                   GPIO Function Select
; ------------------------------------------------------------------------------------------------- ;
; Defines what mode a GPIO pin is set to.                                                           ;
;                                                                                                   ;
; GPFSEL0                       9   8   7   6   5   4   3   2   1   0                               ;
;                         0b00 001 000 000 000 000 000 000 000 001 000                              ;
; GPFSEL1                      19  18  17  16  15  14  13  12  11  10                               ;
;                         0b00 001 000 000 000 000 000 000 000 001 000                              ;
; GPFSEL2                      29  28  27  26  25  24  23  22  21  20                               ;
;                         0b00 001 000 000 000 000 000 000 001 000 000                              ;
; GPFSEL3                      39  38  37  36  35  34  33  32  31  30                               ;
;                         0b00 001 000 000 000 000 000 000 000 001 000                              ;
; GPFSEL4                      49  48  47  46  45  44  43  42  41  40                               ;
;                         0b00 001 000 000 000 000 000 000 000 001 000                              ;
; GPFSEL5                              57  56  55  54  53  52  51  50                               ;
;                         0b00 000 000 000 000 000 000 000 000 001 000                              ;
; ------------------------------------------------------------------------------------------------- ;
GPFSEL0 = 0x0                                                                                       ;
GPFSEL1 = 0x4                                                                                       ;
GPFSEL2 = 0x8                                                                                       ;
GPFSEL3 = 0xC                                                                                       ;
GPFSEL4 = 0x10                                                                                      ;
GPFSEL5 = 0x14                                                                                      ;
; ------------------------------------------------------------------------------------------------- ;
;                                   GPIO PIN MODES (Enum)                                           ;
; ------------------------------------------------------------------------------------------------- ;
; [Doc3] Page 67                                                                                    ;
; ------------------------------------------------------------------------------------------------- ;
GPIO_PINMODE_INPUT = 0      ; 0b000                                                                 ;
GPIO_PINMODE_OUTPUT = 1     ; 0b001                                                                 ;
GPIO_PINMODE_ALTFUNC0 = 4   ; 0b100                                                                 ;
GPIO_PINMODE_ALTFUNC1 = 5   ; 0b101                                                                 ;
GPIO_PINMODE_ALTFUNC2 = 6   ; 0b110                                                                 ;
GPIO_PINMODE_ALTFUNC3 = 7   ; 0b111                                                                 ;
GPIO_PINMODE_ALTFUNC4 = 3   ; 0b011                                                                 ;
GPIO_PINMODE_ALTFUNC5 = 2   ; 0b010                                                                 ;
; ------------------------------------------------------------------------------------------------- ;
;                                         GPIO Set Mode                                             ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: x0, w1, w2                                                                                  ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; ------------------------------------------------------------------------------------------------- ;
macro GPIO_PinModeSetter GPIONum,Mode                                                               ;
{                                                                                                   ;
    modesPerRegister = 32 / 3                   ; Determine number of groupings fit in a word.      ;
    regNum = GPIONum / modesPerRegister         ; Determine which register to use (integer division);
    regOffset = regNum * 4                      ; Calculate offset                                  ;
                                                                                                    ;
    shiftPosition = GPIONum mod modesPerRegister; If you treat 32 bits as an array, what is the index
    shiftAmount = shiftPosition * 3             ; Convert from "index" to actual bit location       ;
    Mask = (1 shl 3) - 1                        ; Creates 0b111 (0b1000 - 1)                        ;
                                                                                                    ;
    mov x0, GPIO_BASE                           ; Save base address to x0                           ;
    ldr w1, [x0, regOffset]                     ; Load current values so previously set modes remain;
    mov w2, Mask                                ; Put 0b111 into w2                                 ;
    mvn w2, w2, lsl shiftAmount                 ; Shift 0b111 to the left and NOT the whole word.   ;
    and w1, w1, w2                              ; Perform masking                                   ;
    mov w1, Mode                                ; Put mode in w1                                    ;
    lsl w1, w1, shiftAmount                     ; Shift mode to correct spot.                       ;
    str w1, [x0, regOffset]                     ; Store number to set mode                          ;
}                                                                                                   ;
; ================================================================================================= ;

; ================================================================================================= ;
;                                   GPIO Output Pin ON                                              ;
; ------------------------------------------------------------------------------------------------- ;
; Doc3 page 70                                                                                      ;
; GPSET0: Pins 0-31                                                                                 ;
; 0b0000000000000000000000000000100 ; Example is for pin 2                                          ;
; GPSET1: Pins 32-57                                                                                ;
; 0b0000000000000000000000000000100 ; Example is for pin 34                                         ;
; ------------------------------------------------------------------------------------------------- ;
GPSET0 = 0x1C                                                                                       ;
GPSET1 = 0x20                                                                                       ;
; ------------------------------------------------------------------------------------------------- ;
;                                       GPIO Turn ON                                                ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: x0, w1                                                                                      ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; ------------------------------------------------------------------------------------------------- ;
macro GPIO_SetOutputOn GPIONum                                                                      ;
{                                                                                                   ;
    regNum = GPIONum / 32               ; Determine which register to use                           ;
    regOffset = (regNum * 4) + GPSET0   ; Calculate offset                                          ;
                                                                                                    ;
    shiftAmount = GPIONum mod 32        ; Calculate how far to shift in register                    ;
                                                                                                    ;
    mov x0, GPIO_BASE                   ; Set GPIO Base address in x0                               ;
    mov w1, 1                                                                                       ;
    lsl w1, w1, shiftAmount             ; Shift 1 to the correct location                           ;
    str w1, [x0, regOffset]             ; Store value to turn output on                             ;
}                                                                                                   ;
; ================================================================================================= ;

; ================================================================================================= ;
;                                   GPIO Output Pin OFF                                             ;
; ------------------------------------------------------------------------------------------------- ;
; Doc3 Page 70                                                                                      ;
; GPCLR0: Pins 0-31                                                                                 ;
; 0b0000000000000000000000000000100 ; Example is for pin 2                                          ;
; GPCLR1: Pins 32-57                                                                                ;
; 0b0000000000000000000000000000100 ; Example is for pin 34                                         ;
; ------------------------------------------------------------------------------------------------- ;
GPCLR0 = 0x28                                                                                       ;
GPCLR1 = 0x2C                                                                                       ;
; ------------------------------------------------------------------------------------------------- ;
;                                       GPIO Turn OFF                                               ;
; ------------------------------------------------------------------------------------------------- ;
; Uses: x0, w1                                                                                      ;
; Returns: void                                                                                     ;
; Params: void                                                                                      ;
; ------------------------------------------------------------------------------------------------- ;
macro GPIO_SetOutputOff GPIONum                                                                     ;
{                                                                                                   ;
    regNum = GPIONum / 32               ; Determine which register to use                           ;
    regOffset = (regNum * 4) + GPCLR0   ; Calculate offset                                          ;
                                                                                                    ;
    shiftAmount = GPIONum mod 32        ; Determine how far to shift in register                    ;
                                                                                                    ;
    mov x0, GPIO_BASE                   ; Put GPIO Base into x0                                     ;
    mov w1, 1                                                                                       ;
    lsl w1, w1, shiftAmount             ; Shift 1 to the proper spot                                ;
    str w1, [x0, regOffset]             ; Store value to turn output off                            ;
}                                                                                                   ;
; ================================================================================================= ;
