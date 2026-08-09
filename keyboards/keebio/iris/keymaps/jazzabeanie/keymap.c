// Copyright 2026 Jared Johnston (@jazzabeanie)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "qmk_midi.h" // for midi_device and midi_send_cc()


// This keymap lives in the shared iris/keymaps directory, so it builds for any
// revision with the 56-key LAYOUT. rev8 (RP2040) drives its LEDs with rgb_matrix
// while rev5 (atmega32u4) uses rgblight, and each has its own keycodes - the
// wrong family compiles fine but the keys silently do nothing.
#ifdef RGB_MATRIX_ENABLE
#    define JZ_RGBT RM_TOGG
#    define JZ_RGBN RM_NEXT
#else
#    define JZ_RGBT UG_TOGG
#    define JZ_RGBN UG_NEXT
#endif

#define _QWERTY 0
#define _LOWER 1
#define _RAISE 2
#define _ADJUST 3
#define _ABLETON 4
#define _LEFT 5
#define _LEFT_LOWER 6

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  ADJUST,
  ABLETON,
  // MIDI Control Change keys, used for the whole right hand of _ABLETON so those
  // keys trigger actions in Ableton rather than playing notes. Each is named after
  // the CC number it sends, so the keymap tells you what to look for when MIDI
  // mapping. The numbering jumps from 31 to 102 because 20-31 and 102-119 are the
  // two ranges the MIDI spec leaves undefined - anything else risks colliding with
  // a standard controller such as mod wheel (1) or sustain (64). Keep CC_20 first
  // and CC_115 last: process_record_user() relies on the two blocks being
  // contiguous and in ascending order.
  CC_20, CC_21, CC_22, CC_23, CC_24, CC_25, CC_26, CC_27, CC_28, CC_29, CC_30, CC_31,
  CC_102, CC_103, CC_104, CC_105, CC_106, CC_107, CC_108, CC_109,
  CC_110, CC_111, CC_112, CC_113, CC_114, CC_115,
};

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
  // When the swap hands key is pressed, the next key will be put into this function by it's position. See ../../rev5/rev5.h for the index of each key. Left hand is rows 0 to 4 and right is 5-9. For columns zero is on the outside of each side. This function then returns the index (but reversed, so col, row) of the key that should be returned when swap-hands is enabled. see docs for more information: https://github.com/qmk/qmk_firmware/blob/master/docs/feature_swap_hands.md
  {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}},
  {{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}},
  {{0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}},
  {{0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}},
  {{1, 1}, {1, 1}, {2, 9}, {3, 9}, {4, 9}, {4, 9}}, // the first two keys don't exist on my keyboard. the last two keys are 4, 9 here. The first one doesn't matter because it is the swap key. the last one will swap LE6 for RE5 (so I can use the space key)
  {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}},
  {{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}},
  {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}},
  {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}},
  {{1, 1}, {1, 1}, {5, 4}, {3, 4}, {4, 9}, {4, 4}}, // First two keys don't exist. The 3rd is mapped to LE6 which should be KC_LGUI so I can have both alt and GUI on both hands. The 2nd last (RE5) maps to itself so the hand swap key does nothing for this key. Last one doesnt matter because it is the hand swap key itself.
  // see ../../rev5/rev5.h for the matrix configuration.
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT( // note: read docs on QK_GESC if Esc / Grave is behavine weirdly
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     QK_GESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                               KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                               KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,     KC_LGUI,          SH_MON,  KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    KC_LALT, LOWER,     SH_MON,                    KC_SPC,  RAISE,   KC_RALT
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_LOWER] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
      KC_GRV, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                               KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______, _______, KC_LEFT,   KC_UP,  KC_DOWN, KC_RGHT,                            KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_PLUS, KC_PIPE,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     _______,  KC_F1,    KC_F2,   KC_F3,   KC_F4,   KC_F5, _______,          KC_F6,   KC_F7,  KC_P8,   KC_F9,    KC_F10,  KC_F11,   KC_F12,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, _______,                   _______,  _______, _______
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_RAISE] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     KC_F12,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                              KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,                            KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______, _______, _______, KC_DEL,  KC_PGUP, KC_UNDS,                            KC_MINS, KC_EQL,  KC_LCBR, KC_RCBR, KC_PIPE,  KC_GRV,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     _______, _______, _______, _______, KC_PGDN, KC_MINS, KC_LPRN,          _______, KC_UNDS, KC_PLUS, KC_LBRC, KC_RBRC, KC_BSLS, KC_TILD,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, _______,                   KC_LEFT, _______, KC_RGHT
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_ADJUST] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
     _______, _______, _______, _______, _______, _______,                            _______, _______, _______, _______, _______, _______,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______, _______, _______, _______, JZ_RGBN, JZ_RGBT,                            _______, _______, _______, _______, KC_PSCR, _______,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______,   TO(4), KC_MPRV, KC_VOLU, KC_VOLD, KC_MNXT,                            KC_HOME, KC_PGDN, KC_PGUP,  KC_END, _______, _______,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     _______, _______, _______, _______, KC_MPLY, KC_MUTE, QK_BOOT,          QK_BOOT, _______, AG_TOGG, _______, _______, _______, KC_ENT ,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, _______,                   _______, _______, _______
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  ),

  [_ABLETON] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       TO(0),    KC_K,    KC_O,    KC_L, _______, _______,                              MI_ON,   CC_20,   CC_21,   CC_22,   CC_23,   CC_24,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______,    KC_Y,    KC_H,    KC_U,    KC_J, _______,                             MI_OFF,   CC_25,   CC_26,   CC_27,   CC_28,   CC_29,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______,    KC_D,    KC_F,    KC_T,    KC_G,    KC_X,                              CC_30,   CC_31,  CC_102,  CC_103,  CC_104,  CC_105,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     _______,    KC_A,    KC_W,    KC_S,    KC_E,    KC_Z, _______,           CC_106,  CC_107,  CC_108,  CC_109,  CC_110,  CC_111,  CC_112,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, _______,                    CC_113,  CC_114,  CC_115
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (keycode >= CC_20 && keycode <= CC_115) {
    // 127 on press and 0 on release, the same shape as MI_SUS, so Ableton can
    // MIDI map the key as either a momentary or a toggle control.
    uint8_t cc = (keycode <= CC_31) ? 20 + (keycode - CC_20) : 102 + (keycode - CC_102);
    midi_send_cc(&midi_device, midi_config.channel, cc, record->event.pressed ? 127 : 0);
    return false;
  }

  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case ADJUST:
      if (record->event.pressed) {
        layer_on(_ADJUST);
      } else {
        layer_off(_ADJUST);
      }
      return false;
      break;
  }
  return true;
}
