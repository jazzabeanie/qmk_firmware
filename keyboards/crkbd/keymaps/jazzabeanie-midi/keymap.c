#include QMK_KEYBOARD_H

extern MidiDevice midi_device;

// MIDI CC codes for generic on/off switches (80, 81, 82, 83)
// Off: 0-63
// On:  64-127

#define MIDI_CC_OFF 0
#define MIDI_CC_ON  127

extern uint8_t is_master;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define _QWERTY 0
#define _RAISE 2

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  RAISE,
  MIDI_CC60,
  MIDI_CC61,
  MIDI_CC62,
  MIDI_CC63,
  MIDI_CC64,
  MIDI_CC65,
  MIDI_CC66,
  MIDI_CC67,
  MIDI_CC68,
  MIDI_CC69,
  MIDI_CC70,
  MIDI_CC71,
  MIDI_CC72,
  MIDI_CC73,
  MIDI_CC74,
  MIDI_CC75,
  MIDI_CC76,
  MIDI_CC77,
  MIDI_CC78,
  MIDI_CC79,
  // MIDI_CC80,
  // MIDI_CC81,
  // MIDI_CC82,
  // MIDI_CC83,
  // MIDI_CC84,
  // MIDI_CC85,
  // MIDI_CC86,
  // MIDI_CC87,
  // MIDI_CC88,
  // MIDI_CC89,
  // MIDI_CC90,
  // MIDI_CC91,
  // MIDI_CC92,
  // MIDI_CC93,
  // MIDI_CC94,
  // MIDI_CC95,
  // MIDI_CC96,
  // MIDI_CC97,
  // MIDI_CC98,
  // MIDI_CC99,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT( \
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    MIDI_CC60, MIDI_CC61, MIDI_CC62, MIDI_CC63, MIDI_CC64, MIDI_CC65,\
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+----------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    MIDI_CC66, MIDI_CC67, MIDI_CC68, MIDI_CC69, MIDI_CC70, MIDI_CC71,\
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+----------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                    MIDI_CC72, MIDI_CC73, MIDI_CC74, MIDI_CC75, MIDI_CC76, MIDI_CC77,\
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+----------+--------+--------|
                                          XXXXXXX, XXXXXXX, XXXXXXX,  MIDI_CC78,   TO(2), MIDI_CC78 \
                                      //`--------------------------'  `--------------------------'

  ),

  [_RAISE] = LAYOUT( \
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      MI_OCTU,  MI_A_4, MI_Bb_4,  MI_B_4,  MI_C_5, MI_CHU,\
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      MI_OCTD,  MI_E_3,  MI_F_3, MI_Gb_3,  MI_G_3, MI_CHD,\
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX,  MI_C_3, MI_Db_3,  MI_D_3, MI_Eb_3, XXXXXXX,\
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,   TO(0), XXXXXXX \
                                      //`--------------------------'  `--------------------------'
  )
};

void process_midi_cc(uint8_t cc_number, keyrecord_t *record) {
    if (record->event.pressed) {
        midi_send_cc(&midi_device, midi_config.channel, cc_number, MIDI_CC_ON);
    } else {
        midi_send_cc(&midi_device, midi_config.channel, cc_number, MIDI_CC_OFF);
    }
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MIDI_CC60:
            process_midi_cc(60, record);
            return true;
        case MIDI_CC61:
            process_midi_cc(61, record);
            return true;
        case MIDI_CC62:
            process_midi_cc(62, record);
            return true;
        case MIDI_CC63:
            process_midi_cc(63, record);
            return true;
        case MIDI_CC64:
            process_midi_cc(64, record);
            return true;
        case MIDI_CC65:
            process_midi_cc(65, record);
            return true;
        case MIDI_CC66:
            process_midi_cc(66, record);
            return true;
        case MIDI_CC67:
            process_midi_cc(67, record);
            return true;
        case MIDI_CC68:
            process_midi_cc(68, record);
            return true;
        case MIDI_CC69:
            process_midi_cc(69, record);
            return true;
        case MIDI_CC70:
            process_midi_cc(70, record);
            return true;
        case MIDI_CC71:
            process_midi_cc(71, record);
            return true;
        case MIDI_CC72:
            process_midi_cc(72, record);
            return true;
        case MIDI_CC73:
            process_midi_cc(73, record);
            return true;
        case MIDI_CC74:
            process_midi_cc(74, record);
            return true;
        case MIDI_CC75:
            process_midi_cc(75, record);
            return true;
        case MIDI_CC76:
            process_midi_cc(76, record);
            return true;
        case MIDI_CC77:
            process_midi_cc(77, record);
            return true;
        case MIDI_CC78:
            process_midi_cc(78, record);
            return true;
        case MIDI_CC79:
            process_midi_cc(79, record);
            return true;
        // case MIDI_CC80:
        //     process_midi_cc(80, record);
        //     return true;
        // case MIDI_CC81:
        //     process_midi_cc(81, record);
        //     return true;
        // case MIDI_CC82:
        //     process_midi_cc(82, record);
        //     return true;
        // case MIDI_CC83:
        //     process_midi_cc(83, record);
        //     return true;
        // case MIDI_CC84:
        //     process_midi_cc(84, record);
        //     return true;
        // case MIDI_CC85:
        //     process_midi_cc(85, record);
        //     return true;
        // case MIDI_CC86:
        //     process_midi_cc(86, record);
        //     return true;
        // case MIDI_CC87:
        //     process_midi_cc(87, record);
        //     return true;
        // case MIDI_CC88:
        //     process_midi_cc(88, record);
        //     return true;
        // case MIDI_CC89:
        //     process_midi_cc(89, record);
        //     return true;
        // case MIDI_CC90:
        //     process_midi_cc(90, record);
        //     return true;
        // case MIDI_CC91:
        //     process_midi_cc(91, record);
        //     return true;
        // case MIDI_CC92:
        //     process_midi_cc(92, record);
        //     return true;
        // case MIDI_CC93:
        //     process_midi_cc(93, record);
        //     return true;
        // case MIDI_CC94:
        //     process_midi_cc(94, record);
        //     return true;
        // case MIDI_CC95:
        //     process_midi_cc(95, record);
        //     return true;
        // case MIDI_CC96:
        //     process_midi_cc(96, record);
        //     return true;
        // case MIDI_CC97:
        //     process_midi_cc(97, record);
        //     return true;
        // case MIDI_CC98:
        //     process_midi_cc(98, record);
        //     return true;
        // case MIDI_CC99:
        //     process_midi_cc(99, record);
        //     return true;

        default:
            return true;
    }
};


// void persistent_default_layer_set(uint16_t default_layer) {
//   eeconfig_update_default_layer(default_layer);
//   default_layer_set(default_layer);
// }
