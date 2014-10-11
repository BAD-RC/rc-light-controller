#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>
#include <stdbool.h>

#define CONFIG_VERSION 1


// Suppress unused parameter or variable warning
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

#define __SYSTICK_IN_MS 20

// The ROM_MAGIC marker is used to identify the location of ROM constants when
// parsing a light controller binary by an external tool
#define ROM_MAGIC 0x6372424c                // LBrc (LANE Boys RC) in little endian

#define ST 0
#define TH 1
#define CH3 2

// Number of positions of our virtual light switch. Includes the "off"
// position 0.
// NOTE: if you change this value you need to adjust CAR_LIGHT_FUNCTION_T
// accordingly!
#define LIGHT_SWITCH_POSITIONS 9

#define MAX_LIGHT_PROGRAMS 25
#define MAX_LIGHT_PROGRAM_VARIABLES 100

// Convenience functions for min/max
#define MIN(x, y) ((x) < (y) ? x : (y))
#define MAX(x, y) ((x) > (y) ? x : (y))


// Opcodes for light programs
#define OPCODE_GOTO             0x01
#define OPCODE_SET              0x02
#define OPCODE_SET_VARIABLE     0x03
#define OPCODE_FADE             0x04
#define OPCODE_FADE_VARIABLE    0x05
#define OPCODE_WAIT             0x06
#define OPCODE_WAIT_VARIABLE    0x07

#define OPCODE_ASSIGN           0x08    // VAR =    type1=var|type2, id1, id2 
#define OPCODE_ADD              0x09    // VAR +=   type1=var|type2, id1, id2
#define OPCODE_SUBTRACT         0x0a    // VAR -=   type1=var|type2, id1, id2
#define OPCODE_MULTIPL          0x0b    // VAR *=   type1=var|type2, id1, id2
#define OPCODE_DIVIDE           0x0c    // VAR /=   type1=var|type2, id1, id2

#define OPCODE_SKIP_IF_EQ       0x0d    // ==       type1|type2, id1, id2
#define OPCODE_SKIP_IF_NE       0x0e    // !=       type1|type2, id1, id2
#define OPCODE_SKIP_IF_GE       0x0f    // >=       type1|type2, id1, id2
#define OPCODE_SKIP_IF_GT       0x10    // >        type1|type2, id1, id2
#define OPCODE_SKIP_IF_LE       0x11    // <=       type1|type2, id1, id2
#define OPCODE_SKIP_IF_LT       0x12    // <        type1|type2, id1, id2

#define OPCODE_IF_ANY           0x20    // + 29 bits run_state!
#define OPCODE_IF_ALL           0x40    // + 29 bits run_state!
#define OPCODE_IF_NONE          0x80    // + 29 bits run_state!

#define OPCODE_END_OF_PROGRAM   0xfe
#define OPCODE_END_OF_PROGRAMS  0xff

// Instructions comprise of opcodes and parameters
#define INSTRUCTION_END_OF_PROGRAM \
    (OPCODE_END_OF_PROGRAM << 24)

#define INSTRUCTION_END_OF_PROGRAMS \
    (OPCODE_END_OF_PROGRAMS << 24)

#define INSTRUCTION_SET(start, stop, value) \
    ((OPCODE_SET << 24) | (stop << 16) | (start << 8) | value)

#define INSTRUCTION_FADE(start, stop, value) \
    ((OPCODE_FADE << 24) | (stop << 16) | (start << 8) | value)

#define INSTRUCTION_WAIT(time_in_ms) \
    ((OPCODE_WAIT << 24) | (time_in_ms / __SYSTICK_IN_MS))

#define INSTRUCTION_GOTO(line_no) \
    ((OPCODE_GOTO << 24) | line_no)


// Offset of special position within every light program
#define PRIORITY_STATE_OFFSET 0
#define RUN_STATE_OFFSET 1
#define LEDS_USED_OFFSET 2
#define FIRST_OPCODE_OFFSET 3


#define LED_USED(x) (1 << x)
#define START_LED(x) (x << 16)
#define STOP_LED(x) (x << 8)


// ****************************************************************************
typedef enum {
    RUN_WHEN_LIGHT_SWITCH_POSITION   = (1 << 0),     // Bits 0..8
    RUN_WHEN_LIGHT_SWITCH_POSITION_1 = (1 << 1),
    RUN_WHEN_LIGHT_SWITCH_POSITION_2 = (1 << 2),
    RUN_WHEN_LIGHT_SWITCH_POSITION_3 = (1 << 3),
    RUN_WHEN_LIGHT_SWITCH_POSITION_4 = (1 << 4),
    RUN_WHEN_LIGHT_SWITCH_POSITION_5 = (1 << 5),
    RUN_WHEN_LIGHT_SWITCH_POSITION_6 = (1 << 6),
    RUN_WHEN_LIGHT_SWITCH_POSITION_7 = (1 << 7),
    RUN_WHEN_LIGHT_SWITCH_POSITION_8 = (1 << 8),

    RUN_WHEN_NEUTRAL                 = (1 << 9),
    RUN_WHEN_FORWARD                 = (1 << 10),
    RUN_WHEN_REVERSING               = (1 << 11),
    RUN_WHEN_BRAKING                 = (1 << 12),

    RUN_WHEN_INDICATOR_LEFT          = (1 << 13),
    RUN_WHEN_INDICATOR_RIGHT         = (1 << 14),
    RUN_WHEN_HAZARD                  = (1 << 15),
    RUN_WHEN_BLINK_FLAG              = (1 << 16),
    RUN_WHEN_BLINK_LEFT              = (1 << 17),
    RUN_WHEN_BLINK_RIGHT             = (1 << 18),

    RUN_WHEN_WINCH_DISABLERD         = (1 << 19),
    RUN_WHEN_WINCH_IDLE              = (1 << 20),
    RUN_WHEN_WINCH_IN                = (1 << 21),
    RUN_WHEN_WINCH_OUT               = (1 << 22),

    RUN_WHEN_GEAR_1                  = (1 << 23),
    RUN_WHEN_GEAR_2                  = (1 << 24),

    RUN_ALWAYS                       = (1 << 31)
} LIGHT_PROGRAM_RUN_STATE_T;


// ****************************************************************************
typedef enum {
    RUN_WHEN_NORMAL_OPERATION           = 0,
    RUN_WHEN_NO_SIGNAL                  = (1 << 0),
    RUN_WHEN_INITIALIZING               = (1 << 1),
    RUN_WHEN_SERVO_OUTPUT_SETUP_CENTRE  = (1 << 2),
    RUN_WHEN_SERVO_OUTPUT_SETUP_LEFT    = (1 << 3),
    RUN_WHEN_SERVO_OUTPUT_SETUP_RIGHT   = (1 << 4),
    RUN_WHEN_REVERSING_SETUP_STEERING   = (1 << 5),
    RUN_WHEN_REVERSING_SETUP_THROTTLE   = (1 << 6),
    RUN_WHEN_GEAR_CHANGED               = (1 << 7),
} LIGHT_PROGRAM_PRIORITY_STATE_T;


// ****************************************************************************
typedef enum {
    // By specifying this unused value we force the enmeration to fit in a
    // uint16_t
    EMPTY = 0xffff,

    CONFIG_SECTION = 0x01,
    GAMMA_TABLE = 0x02,
    LOCAL_LEDS = 0x10,
    SLAVE_LEDS = 0x20,
    LIGHT_PROGRAMS = 0x30
} ROM_SECTION_T;


// ****************************************************************************
typedef struct {
    uint32_t magic_value;
    ROM_SECTION_T type;
    uint16_t version;
} MAGIC_T;


// ****************************************************************************
typedef struct {
    MAGIC_T magic;
    uint8_t gamma_table[256];
} GAMMA_TABLE_T;


// ****************************************************************************
typedef struct {
    MAGIC_T magic;
    int number_of_programs;
    const uint32_t *start[MAX_LIGHT_PROGRAMS];
    uint32_t programs[50];
} LIGHT_PROGRAMS_T;

// ****************************************************************************
typedef struct {
    uint16_t left;
    uint16_t centre;
    uint16_t right;
} SERVO_ENDPOINTS_T;


// ****************************************************************************
typedef struct {
    uint32_t raw_data;
    SERVO_ENDPOINTS_T endpoint;
    int16_t normalized;
    uint16_t absolute;
    bool reversed;
} CHANNEL_T;


// ****************************************************************************
typedef enum {
    GEAR_1 = 0,
    GEAR_2 = 1
} GEAR_T;


// ****************************************************************************
typedef enum {
    SERVO_OUTPUT_SETUP_OFF = 0,
    SERVO_OUTPUT_SETUP_CENTRE = 0x01,
    SERVO_OUTPUT_SETUP_LEFT = 0x02,
    SERVO_OUTPUT_SETUP_RIGHT = 0x04
} SERVO_OUTPUT_T;


// ****************************************************************************
typedef enum {
    REVERSING_SETUP_OFF = 0,
    REVERSING_SETUP_STEERING = 0x01,
    REVERSING_SETUP_THROTTLE = 0x02,
} REVERSING_SETUP_T;


// ****************************************************************************
typedef enum {
    WINCH_DISABLED = 0,
    WINCH_IDLE = 0x01,
    WINCH_IN = 0x02,
    WINCH_OUT = 0x04
} WINCH_T;


// ****************************************************************************
typedef struct {
    unsigned int systick : 1;               // Set for one mainloop every 20 ms
    unsigned int new_channel_data : 1;      // Set for one mainloop every time servo pulses were received

    unsigned int no_signal : 1;
    unsigned int initializing : 1;
    unsigned int servo_output_setup : 3;
    unsigned int reversing_setup : 2;

    unsigned int blink_flag : 1;            // Toggles with 1.5 Hz
    unsigned int blink_hazard : 1;          // Hazard lights active
    unsigned int blink_indicator_left : 1;  // Left indicator active
    unsigned int blink_indicator_right : 1; // Right indicator active
    unsigned int forward : 1;               // Set when the car is driving forward
    unsigned int braking : 1;               // Set when the brakes are enganged
    unsigned int reversing : 1;             // Set when the car is reversing

    unsigned int gear_changed : 1;          // Set for one mainloop when a new gear was selected
    unsigned int gear : 1;

    unsigned int winch_mode : 3;
} GLOBAL_FLAGS_T;


// ****************************************************************************
typedef enum {
    MASTER_WITH_SERVO_READER,
    MASTER_WITH_UART_READER,
    SLAVE
} MASTER_MODE_T;


// ****************************************************************************
typedef struct {
    MAGIC_T magic;

    MASTER_MODE_T mode;

    struct {
        // If mode is MASTER_WITH_SERVO_READER  then all flags are mutually
        // exculsive.
        // If mode is MASTER_WITH_UART_READER then there can be one UART output
        // (slave, preprocessor or winch) and one servo output (steering wheel
        // or gearbox servo)
        unsigned int slave_output : 1;
        unsigned int preprocessor_output : 1;
        unsigned int winch_output : 1;
        unsigned int steering_wheel_servo_output : 1;
        unsigned int gearbox_servo_output : 1;

        unsigned int esc_forward_reverse : 1;
        unsigned int ch3_is_local_switch : 1;
        unsigned int ch3_is_momentary : 1;

        unsigned int auto_brake_lights_forward_enabled : 1;
        unsigned int auto_brake_lights_reverse_enabled : 1;

        // If ENABLE_BRAKE_DISARM_TIMEOUT is not set, the user has to go for
        // brake, then neutral, before reverse engages. Otherwise reverse
        // engages if the user stays in neutral for a few seconds.
        //
        // Tamiya ESC need this ENABLE_BRAKE_DISARM_TIMEOUT cleared.
        // The China ESC and HPI SC-15WP need ENABLE_BRAKE_DISARM_TIMEOUT set.
        unsigned int brake_disarm_timeout_enabled : 1;
    } flags;

    uint16_t auto_brake_counter_value_forward_min;
    uint16_t auto_brake_counter_value_forward_max;
    uint16_t auto_brake_counter_value_reverse_min;
    uint16_t auto_brake_counter_value_reverse_max;
    uint16_t auto_reverse_counter_value_min;
    uint16_t auto_reverse_counter_value_max;
    uint16_t brake_disarm_counter_value;

    uint16_t blink_counter_value;
    uint16_t indicator_idle_time_value;
    uint16_t indicator_off_timeout_value;

    // Centre threshold defines a range where we consider the servo being
    // centred. In order to prevent "flickering" especially for the brake and
    // reverse light the CENTRE_THRESHOLD_HIGH and CENTRE_THRESHOLD_LOW provide
    // a hysteresis that we apply to the throttle when processing drive_mode.
    uint16_t centre_threshold_low;
    uint16_t centre_threshold;
    uint16_t centre_threshold_high;
    uint16_t blink_threshold;

    uint16_t light_switch_positions;
    uint16_t initial_endpoint_delta;
    uint16_t ch3_multi_click_timeout;
    uint16_t winch_command_repeat_time;

    uint32_t baudrate;
    uint16_t no_signal_timeout;
} LIGHT_CONTROLLER_CONFIG_T;



// ****************************************************************************
// Definitions for the various light configuration structures

typedef uint8_t LED_T;

typedef struct {    // 4-bytes packed (2 bits free)
    // Simulation of incandescent lights
    uint8_t max_change_per_systick;

    // Simulation of a weak ground connection
    uint8_t reduction_percent;
    unsigned int light_switch_position_0 : 1;
    unsigned int light_switch_position_1 : 1;
    unsigned int light_switch_position_2 : 1;
    unsigned int light_switch_position_3 : 1;
    unsigned int light_switch_position_4 : 1;
    unsigned int light_switch_position_5 : 1;
    unsigned int light_switch_position_6 : 1;
    unsigned int light_switch_position_7 : 1;
    unsigned int light_switch_position_8 : 1;
    unsigned int tail_light : 1;
    unsigned int brake_light : 1;
    unsigned int reversing_light : 1;
    unsigned int indicator_left : 1;
    unsigned int indicator_right : 1;
} LIGHT_FEATURE_T;

// For standard car light functions we have an array of values, one per LED,
// where each entry corresponds to one light funciton. The user can assign
// multiple functions to a single LED (such as brake and tail light function)
// and the software will "mix" the final color value.

typedef struct {    // 20-bytes packed (1 byte free)
    LIGHT_FEATURE_T features;

    LED_T always_on;
    LED_T light_switch_position[LIGHT_SWITCH_POSITIONS];
    LED_T tail_light;
    LED_T brake_light;
    LED_T reversing_light;
    LED_T indicator_left;
    LED_T indicator_right;
} CAR_LIGHT_T;


typedef struct {
    MAGIC_T magic;
    uint8_t led_count;  // Number of actually used LEDs

    // Pointer to the first element in an array of lights.
    const CAR_LIGHT_T *car_lights;
} CAR_LIGHT_ARRAY_T;


// ****************************************************************************
// The entropy variable is incremented every mainloop. It can therefore serve
// as a random value in practical RC car application,
// Certainly not suitable for secure implementations...
extern uint32_t entropy;

extern const LIGHT_CONTROLLER_CONFIG_T config;
extern const CAR_LIGHT_ARRAY_T local_leds;
extern const CAR_LIGHT_ARRAY_T slave_leds;
extern const GAMMA_TABLE_T gamma_table;
extern const LIGHT_PROGRAMS_T light_programs;

extern GLOBAL_FLAGS_T global_flags;
extern CHANNEL_T channel[3];
extern SERVO_ENDPOINTS_T servo_output_endpoint;


// ****************************************************************************
// Globally accessible functions from various modules
void SysTick_handler(void);

void load_persistent_storage(void);
void write_persistent_storage(void);

void init_servo_reader(void);
void read_all_servo_channels(void);
void SCT_irq_handler(void);

void init_uart_reader(void);
void read_preprocessor(void);

void process_ch3_clicks(void);

void process_drive_mode(void);

void process_indicators(void);
void toggle_hazard_lights(void);

void init_servo_output(void);
void process_servo_output(void);
void servo_output_setup_action(uint8_t ch3_clicks);
void gearbox_action(uint8_t ch3_clicks);

void process_winch(void);
void winch_action(uint8_t ch3_clicks);
bool abort_winching(void);

void process_channel_reversing_setup(void);
void reversing_setup_action(uint8_t ch3_clicks);

void output_preprocessor(void);

void init_lights(void);
void process_lights(void);
void next_light_sequence(void);
void light_switch_up(void);
void light_switch_down(void);
void toggle_light_switch(void);

#endif // __GLOBALS_H
