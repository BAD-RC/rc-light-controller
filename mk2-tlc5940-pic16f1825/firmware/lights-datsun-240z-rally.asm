;******************************************************************************
;
;   datsun-240z-rally.asm
;
;   This file contains the business logic to drive the LEDs for the Tamiya
;   Datsun 240Z Rally body.
;
;   The hardware is based on PIC16F1825 and TLC5940. No DC/DC converter is used.
;
;   The TLC5940 IREF is programmed with a 2000 Ohms resistor, which means
;   the maximum LED current is 19.53 mA; each adjustment step is 0.317 mA.
;
;   The following lights are available:
;
;       OUT0    Main beam left     
;       OUT1    Main beam right
;       OUT2    High beam left (light pod)
;       OUT3    High beam right (light pod)
;       OUT4    
;       OUT5    
;       OUT6    
;       OUT7    
;       OUT8    
;       OUT9    
;       OUT10   Reverse left
;       OUT11   Reverse right
;       OUT12   Tail / Brake rear left
;       OUT13   Tail / Brake rear right 
;       OUT14   Indicator rear left
;       OUT15   Indicator rear right
;
;******************************************************************************
;
;   Author:         Werner Lane
;   E-mail:         laneboysrc@gmail.com
;
;******************************************************************************
    TITLE       Light control logic for the Tamiya Datsun 240Z Rally
    RADIX       dec

    #include    hw.tmp

#define LED_MAIN_BEAM_L 0
#define LED_MAIN_BEAM_R 1
#define LED_HIGH_BEAM_L 2
#define LED_HIGH_BEAM_R 3
#define LED_REVERSING_L 10   
#define LED_REVERSING_R 11    
#define LED_TAIL_BRAKE_L 12    
#define LED_TAIL_BRAKE_R 13    
#define LED_INDICATOR_R_L 14   
#define LED_INDICATOR_R_R 15   
    
    GLOBAL Init_lights
    GLOBAL Output_lights

    
    ; Functions and variables imported from utils.asm
    EXTERN Init_TLC5940    
    EXTERN TLC5940_send
    
    EXTERN xl
    EXTERN xh
    EXTERN temp
    EXTERN light_data

    
    ; Functions and variables imported from master.asm
    EXTERN blink_mode
    EXTERN light_mode
    EXTERN drive_mode
    EXTERN setup_mode
    EXTERN startup_mode
    EXTERN servo


; Bitfields in variable blink_mode
#define BLINK_MODE_BLINKFLAG 0          ; Toggles with 1.5 Hz
#define BLINK_MODE_HAZARD 1             ; Hazard lights active
#define BLINK_MODE_INDICATOR_LEFT 2     ; Left indicator active
#define BLINK_MODE_INDICATOR_RIGHT 3    ; Right indicator active

; Bitfields in variable light_mode
#define LIGHT_MODE_MAIN_BEAM 0      ; Main beam
#define LIGHT_MODE_HIGH_BEAM 1      ; High beam

; Bitfields in variable drive_mode
#define DRIVE_MODE_FORWARD 0 
#define DRIVE_MODE_BRAKE 1 
#define DRIVE_MODE_REVERSE 2

; Bitfields in variable setup_mode
#define SETUP_MODE_INIT 0
#define SETUP_MODE_CENTRE 1
#define SETUP_MODE_LEFT 2
#define SETUP_MODE_RIGHT 3
#define SETUP_MODE_STEERING_REVERSE 4
#define SETUP_MODE_THROTTLE_REVERSE 5
#define SETUP_MODE_NEXT 6
#define SETUP_MODE_CANCEL 7

; Bitfields in variable startup_mode
; Note: the higher 4 bits are used so we can simply "or" it with ch3
; and send it to the slave
#define STARTUP_MODE_NEUTRAL 4      ; Waiting before reading ST/TH neutral


; Since gpasm is not able to use 0.317 we need to calculate with micro-Amps
#define uA_PER_STEP 317

#define VAL_MAIN_BEAM (20 * 1000 / uA_PER_STEP)
#define VAL_HIGH_BEAM (20 * 1000 / uA_PER_STEP)
#define VAL_TAIL (4 * 1000 / uA_PER_STEP)
#define VAL_BRAKE (20 * 1000 / uA_PER_STEP)
#define VAL_REVERSE (20 * 1000 / uA_PER_STEP)
#define VAL_INDICATOR_REAR (20 * 1000 / uA_PER_STEP)


  
;******************************************************************************
; Relocatable variables section
;******************************************************************************
.data_lights UDATA



;============================================================================
;============================================================================
;============================================================================
.lights CODE


;******************************************************************************
; Init_lights
;******************************************************************************
Init_lights
    call    Init_TLC5940
    call    Clear_light_data

    ; Light up both high beam light pods until we receive the first command 
    ; from the UART
    BANKSEL light_data
    movlw   VAL_HIGH_BEAM
    movwf   light_data + LED_HIGH_BEAM_L
    movwf   light_data + LED_HIGH_BEAM_R
    call    TLC5940_send
    return


;******************************************************************************
; Output_lights
;******************************************************************************
Output_lights
    call    Clear_light_data

    BANKSEL startup_mode
    movf    startup_mode, f
    bnz     output_lights_startup

    movf    setup_mode, f
    bnz     output_lights_setup

    ; Normal mode here
    BANKSEL light_mode
    movfw   light_mode
    movwf   temp
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_tail
    btfsc   temp, LIGHT_MODE_MAIN_BEAM
    call    output_lights_main_beam
    btfsc   temp, LIGHT_MODE_HIGH_BEAM
    call    output_lights_high_beam

    BANKSEL drive_mode
    movfw   drive_mode
    movwf   temp
    btfsc   temp, DRIVE_MODE_BRAKE
    call    output_lights_brake

    BANKSEL drive_mode
    movfw   drive_mode
    movwf   temp
    btfsc   temp, DRIVE_MODE_REVERSE
    call    output_lights_reverse
    
    BANKSEL blink_mode
    btfss   blink_mode, BLINK_MODE_BLINKFLAG
    goto    output_lights_end
    
    movfw   blink_mode
    movwf   temp
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_HAZARD
    call    output_lights_indicator_right
    btfsc   temp, BLINK_MODE_INDICATOR_LEFT
    call    output_lights_indicator_left
    btfsc   temp, BLINK_MODE_INDICATOR_RIGHT
    call    output_lights_indicator_right
    
output_lights_end
    goto    output_lights_execute    


output_lights_startup
    btfss   startup_mode, STARTUP_MODE_NEUTRAL
    return
    
    movlw   VAL_MAIN_BEAM
    movwf   light_data + LED_MAIN_BEAM_L
    movwf   light_data + LED_MAIN_BEAM_R
    goto    output_lights_execute    


output_lights_setup
    btfsc   setup_mode, SETUP_MODE_CENTRE
    goto    output_lights_setup_centre
    btfsc   setup_mode, SETUP_MODE_LEFT
    goto    output_lights_setup_right
    btfsc   setup_mode, SETUP_MODE_RIGHT
    goto    output_lights_setup_right
    btfsc   setup_mode, SETUP_MODE_STEERING_REVERSE 
    call    output_lights_indicator_left
    btfsc   setup_mode, SETUP_MODE_THROTTLE_REVERSE 
    call    output_lights_main_beam

output_lights_execute    
    call    TLC5940_send
    return

output_lights_setup_centre
output_lights_setup_left
output_lights_setup_right
    return


output_lights_main_beam
    BANKSEL light_data
    movlw   VAL_MAIN_BEAM
    movwf   light_data + LED_MAIN_BEAM_L
    movwf   light_data + LED_MAIN_BEAM_R
    return
    
output_lights_high_beam
    BANKSEL light_data
    movlw   VAL_HIGH_BEAM
    movwf   light_data + LED_HIGH_BEAM_L
    movwf   light_data + LED_HIGH_BEAM_R
    return
    
output_lights_tail
    BANKSEL light_data
    movlw   VAL_TAIL
    movwf   light_data + LED_TAIL_BRAKE_L
    movwf   light_data + LED_TAIL_BRAKE_R
    return
    
output_lights_brake
    BANKSEL light_data
    movlw   VAL_BRAKE
    movwf   light_data + LED_TAIL_BRAKE_L
    movwf   light_data + LED_TAIL_BRAKE_R
    return
    
output_lights_reverse
    BANKSEL light_data
    movlw   VAL_REVERSE
    movwf   light_data + LED_REVERSING_L
    movwf   light_data + LED_REVERSING_R
    return

output_lights_indicator_left
    BANKSEL light_data
    movlw   VAL_INDICATOR_REAR
    movwf   light_data + LED_INDICATOR_R_L
    return
    
output_lights_indicator_right
    BANKSEL light_data
    movlw   VAL_INDICATOR_REAR
    movwf   light_data + LED_INDICATOR_R_R
    return


;******************************************************************************
; Clear_light_data
;
; Clear all light_data variables, i.e. by default all lights are off.
;******************************************************************************
Clear_light_data
    movlw   HIGH light_data
    movwf   FSR0H
    movlw   LOW light_data
    movwf   FSR0L
    movlw   16          ; There are 16 bytes in light_data
    movwf   temp
    clrw   
clear_light_data_loop
    movwi   FSR0++    
    decfsz  temp, f
    goto    clear_light_data_loop
    return
    
    END
