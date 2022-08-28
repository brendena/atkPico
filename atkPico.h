/* Copyright (C) 1883 Thomas Edison - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the GPLv2 license, which unfortunately won't be
 * written for another century.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


//The include the parity bit
enum PS2_COMMANDS
{
    //F0 might be get scan code???
    PS2_COMMANDS_RED_ID = 0x0F2 ,
    PS2_COMMANDS_SET_LEDS = 0x1ED
};


/*
Very common keys 
*/

#define PS2_SHIFT_L      0x12
#define PS2_SHIFT_R      0x59
#define PS2_CONTROL_L    0x14
#define PS2_EXT_CONTROL_R    0x14 //0xe0
#define PS2_ALT_L        0x11
#define PS2_EXT_ALT_R        0x11 //0xe0
#define PS2_EXT_GUI_L        0x1f //0xe0
#define PS2_EXT_GUI_R        0x27 //oxe0


//extension units
#define PS2_EXT_SYSTEM_POWER 0x37
#define PS2_EXT_SYSTEM_SLEEP 0x3F
#define PS2_EXT_SYSTEM_WAKE  0x5E
#define PS2_EXT_PRINT_SCREEN 0x7C
#define PS2_EXT_INSERT       0x70
#define PS2_EXT_HOME         0x6C
#define PS2_EXT_PAGE_UP      0x7D
#define PS2_EXT_DELETE       0x71
#define PS2_EXT_END          0x69
#define PS2_EXT_PAGE_DOWN    0x7A
#define PS2_EXT_RIGHT_ARROW  0x74
#define PS2_EXT_LEFT_ARROW   0x6B
#define PS2_EXT_DOWN_ARROW   0x72
#define PS2_EXT_UP_ARROW     0x75
#define PS2_EXT_KEYPAD       0x4A
#define PS2_EXT_KEYPAD_ENTER 0x5A
#define PS2_EXT_APP          0x2F
#define PS2_EXT_KEYBOARD_POWER 0x37 
#define PS2_EXT_SCAN_NEXT_TRACK 0x4D
#define PS2_EXT_SCAN_PREVIOUS_TRACK 0x15
#define PS2_EXT_STOP         0x3B
#define PS2_EXT_PLAY_PAUSE   0x34
#define PS2_EXT_MUTE         0x23
#define PS2_EXT_VOLUME_UP    0x32
#define PS2_EXT_VOLUME_DOWN  0x21
#define PS2_EXT_MEDIA_SELECT 0x50
#define PS2_EXT_MAIL         0x48
#define PS2_EXT_CALCULATOR   0x2B
#define PS2_EXT_MY_COMPUTER  0x40
#define PS2_EXT_WWW_SEARCH   0x10
#define PS2_EXT_WWW_HOME     0x3A
#define PS2_EXT_WWW_BACK     0x38
#define PS2_EXT_WWW_FORWARD  0x30
#define PS2_EXT_WWW_STOP     0x28
#define PS2_EXT_WWW_REFRESH  0x20
#define PS2_EXT_WWW_FAVORITES 0x18


/*
break/ctrl-pause
*/

//LOCKING_BUTTONS/LED VALUES
#define PS2_CAPS_LOCK    0x58
#define PS2_SCROLL_LOCK  0x7E
#define PS2_NUM_LOCK     0x77


#define PS2_EXTENSION_FLAG 0xe0
#define PS2_RELASE_FLAG    0xf0

#define PS2_ACK_RESPONSE 0xFA // 250


typedef struct Ps2LockingKeys
{
    unsigned char scrollLock : 1;
    unsigned char numLock : 1;
    unsigned char capsLock : 1;
}Ps2LockingKeys;

typedef union Ps2LockingKeysUnion
{
    Ps2LockingKeys keys;
    unsigned char value;
} Ps2LockingKeysUnion;


// Initialize PS/2 keyboard support
// Parameters
//     pio  - keyboard suport pio number. 0 or 1
//     gpio - GPIO number of data pin, ctl pin must be on next
//            adjacent GPIO
// Returns  - none
void atk_init(uint pio, uint gpio);

void atk_send_command(unsigned short data );

#ifdef __cplusplus
}
#endif
