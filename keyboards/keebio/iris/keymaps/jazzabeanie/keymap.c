// Copyright 2026 Jared Johnston (@jazzabeanie)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "qmk_midi.h"    // for midi_device and midi_send_cc()
#include "transactions.h" // for the split RPC that syncs LED state between halves


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
  // MIDI Control Change keys, used for the right hand of _ABLETON so those keys
  // trigger actions in Ableton rather than playing notes. Each is named after
  // the CC number it sends, so the keymap tells you what to look for when MIDI
  // mapping. The numbering jumps from 31 to 102 because 20-31 and 102-119 are the
  // two ranges the MIDI spec leaves undefined - anything else risks colliding with
  // a standard controller such as mod wheel (1) or sustain (64). Keep CC_20 first
  // and CC_116 last: process_record_user() relies on the two blocks being
  // contiguous and in ascending order.
  CC_20, CC_21, CC_22, CC_23, CC_24, CC_25, CC_26, CC_27, CC_28, CC_29, CC_30, CC_31,
  CC_102, CC_103, CC_104, CC_105, CC_106, CC_107, CC_108, CC_109,
  CC_110, CC_111, CC_112, CC_113, CC_114, CC_115, CC_116,
};

// The 27 CC_* keycodes are contiguous, so a keycode doubles as an index:
// 0-11 are CC 20-31 and 12-26 are CC 102-116. The gap in the CC numbers is
// deliberate (see above); the indices stay dense so they can be used as an
// array subscript.
#define CC_COUNT (CC_116 - CC_20 + 1)

static uint8_t cc_number_for(uint16_t keycode) {
  uint8_t i = keycode - CC_20;
  return (i < 12) ? 20 + i : 102 + (i - 12);
}

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

  // The right hand is numbered straight through in reading order - across each
  // row, top to bottom, then the thumbs - so the CC number tells you where the
  // key is. 20-31 covers the top two rows and 102-116 the rest. The middle right
  // thumb is left transparent (base layer RAISE) rather than sending a CC.
  [_ABLETON] = LAYOUT(
  //┌────────┬────────┬────────┬────────┬────────┬────────┐                          ┌────────┬────────┬────────┬────────┬────────┬────────┐
       TO(0),    KC_K,    KC_O,    KC_L, _______, _______,                              CC_20,   CC_21,   CC_22,   CC_23,   CC_24,   CC_25,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______,    KC_Y,    KC_H,    KC_U,    KC_J, _______,                              CC_26,   CC_27,   CC_28,   CC_29,   CC_30,   CC_31,
  //├────────┼────────┼────────┼────────┼────────┼────────┤                          ├────────┼────────┼────────┼────────┼────────┼────────┤
     _______,    KC_D,    KC_F,    KC_T,    KC_G,    KC_X,                             CC_102,  CC_103,  CC_104,  CC_105,  CC_106,  CC_107,
  //├────────┼────────┼────────┼────────┼────────┼────────┼────────┐        ┌────────┼────────┼────────┼────────┼────────┼────────┼────────┤
     _______,    KC_A,    KC_W,    KC_S,    KC_E,    KC_Z, _______,           CC_108,  CC_109,  CC_110,  CC_111,  CC_112,  CC_113,  CC_114,
  //└────────┴────────┴────────┴───┬────┴───┬────┴───┬────┴───┬────┘        └───┬────┴───┬────┴───┬────┴───┬────┴────────┴────────┴────────┘
                                    _______, _______, _______,                    CC_115, _______,  CC_116
                                // └────────┴────────┴────────┘                 └────────┴────────┴────────┘
  )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (keycode >= CC_20 && keycode <= CC_116) {
    // 127 on press and 0 on release, the same shape as MI_SUS, so Ableton can
    // MIDI map the key as either a momentary or a toggle control.
    midi_send_cc(&midi_device, midi_config.channel, cc_number_for(keycode), record->event.pressed ? 127 : 0);
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

#ifdef RGB_MATRIX_ENABLE
/* -------------------------------------------------------------------------
 * DAW -> keyboard LED feedback.
 *
 * Whatever the DAW sends back on the same numbers the CC keys send out lights
 * the key that sends it, turning the right hand into a status display.
 *
 * The raw value is stored rather than a single on/off bit. A plain MIDI clip
 * or Max for Live device will only ever send 0 and 127, but a Control Surface
 * script conventionally encodes state (clip stopped / playing / queued /
 * recording) in the value, Launchpad style. Keeping the byte means switching
 * to that later is a change to this file's colour logic only, not its shape.
 *
 * Notes are accepted alongside CCs for the same reason: grid-controller
 * Remote Scripts usually send Note On with velocity-as-colour rather than CC.
 * ------------------------------------------------------------------------- */

// Latest value the DAW sent for each CC key. 0 means off. Written by the MIDI
// callbacks on the master and by the split handler on the slave.
static volatile uint8_t cc_state[CC_COUNT];

// Set when cc_state changes so housekeeping only pushes it across the split
// link when there is something to say. Sending every cycle would add latency
// to the matrix scan for no benefit.
static volatile bool cc_dirty = false;

// Which LED sits under each CC key, resolved once from the keymap at startup
// so moving a CC key moves its light with it. NO_LED means unmapped.
static uint8_t cc_led[CC_COUNT];

static int8_t cc_index_for(uint8_t num) {
  if (num >= 20 && num <= 31) return num - 20;
  if (num >= 102 && num <= 116) return 12 + (num - 102);
  return -1;
}

static void cc_feedback_set(uint8_t num, uint8_t val) {
  int8_t i = cc_index_for(num);
  if (i >= 0 && cc_state[i] != val) {
    cc_state[i] = val;
    cc_dirty = true;
  }
}

// Wipe every stored value, so the whole right hand goes dark.
static void cc_feedback_clear_all(void) {
  memset((void *)cc_state, 0, CC_COUNT);
  // Marked dirty unconditionally rather than only on a real change: a dropped
  // transaction would leave the other half lit with state this one no longer
  // has, so this doubles as a resync.
  cc_dirty = true;
}

// Clear on the way out of the Ableton layer. This took over from the MI_OFF /
// MI_ON keys, which were inert - the code behind them lives in process_music.c,
// compiled out unless MIDI_BASIC or audio is enabled - and have since been
// replaced by CC keys. Doing it on exit means nothing the DAW lit is left
// glowing while you type normally, and the layer always starts dark, so
// anything alight really did come from MIDI. It also recovers from a script
// that quit without sending its zeroes.
layer_state_t layer_state_set_user(layer_state_t state) {
  static bool in_ableton = false;
  bool now = layer_state_cmp(state, _ABLETON);
  if (in_ableton && !now) {
    cc_feedback_clear_all();
  }
  in_ableton = now;
  return state;
}

// Channel is deliberately ignored. If a Remote Script ever uses one channel
// per track, this is the only place that needs to learn about it.
static void midi_cc_in(MidiDevice *device, uint8_t chan, uint8_t num, uint8_t val) {
  cc_feedback_set(num, val);
}

// A Note On with velocity 0 means note-off, which lands as "off" for free.
static void midi_noteon_in(MidiDevice *device, uint8_t chan, uint8_t num, uint8_t vel) {
  cc_feedback_set(num, vel);
}

static void midi_noteoff_in(MidiDevice *device, uint8_t chan, uint8_t num, uint8_t vel) {
  cc_feedback_set(num, 0);
}

// Runs on the half without the USB cable. The master owns the state; this side
// just takes what it is given.
static void cc_sync_slave_handler(uint8_t in_len, const void *in_data, uint8_t out_len, void *out_data) {
  if (in_len == CC_COUNT) {
    memcpy((void *)cc_state, in_data, CC_COUNT);
  }
}

// Push the state to the other half when it changes. Only the master initiates.
void housekeeping_task_user(void) {
  if (!is_keyboard_master() || !cc_dirty) return;

  // Take a snapshot first: a MIDI callback can land between the copy and the
  // send, and clearing the flag before sending means such an update is not
  // lost - it just sets the flag again and goes out on the next tick.
  uint8_t snapshot[CC_COUNT];
  cc_dirty = false;
  memcpy(snapshot, (const void *)cc_state, CC_COUNT);

  if (!transaction_rpc_send(CC_FEEDBACK_SYNC, CC_COUNT, snapshot)) {
    cc_dirty = true; // link was busy, try again next tick
  }
}

void keyboard_post_init_user(void) {
  for (uint8_t i = 0; i < CC_COUNT; i++) {
    cc_led[i] = NO_LED;
  }
  for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
      uint16_t keycode = keymap_key_to_keycode(_ABLETON, (keypos_t){.row = row, .col = col});
      if (keycode >= CC_20 && keycode <= CC_116) {
        cc_led[keycode - CC_20] = g_led_config.matrix_co[row][col];
      }
    }
  }

  // Both halves register: either one can end up being the slave depending on
  // which side the cable is in.
  transaction_register_rpc(CC_FEEDBACK_SYNC, cc_sync_slave_handler);

  // Safe to register here: protocol_pre_init() has already run setup_midi(),
  // so midi_device_init() will not clear these back out.
  midi_register_cc_callback(&midi_device, midi_cc_in);
  midi_register_noteon_callback(&midi_device, midi_noteon_in);
  midi_register_noteoff_callback(&midi_device, midi_noteoff_in);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
  // On the Ableton layer, blank whatever effect is running so the only lit key
  // is one the DAW asked for - if something glows, it came over MIDI. Done by
  // painting black here rather than switching the matrix off, because the
  // indicator pass only runs while rgb_matrix is enabled, and this leaves the
  // chosen mode and brightness untouched for every other layer.
  if (layer_state_is(_ABLETON)) {
    for (uint8_t i = led_min; i < led_max; i++) {
      rgb_matrix_set_color(i, 0, 0, 0);
    }
  }

  for (uint8_t i = 0; i < CC_COUNT; i++) {
    uint8_t val = cc_state[i];
    if (val == 0) continue;

    uint8_t led = cc_led[i];
    if (led == NO_LED || led < led_min || led >= led_max) continue;

    // Floor the brightness so any non-zero value is unmistakably visible. A
    // script sending 0/127 gets off/full green; one sending small palette
    // indices (1=green, 2=blink, ...) still shows clearly instead of sitting
    // at brightness 2 and looking like the feedback is broken.
    uint8_t level = (val >= 127) ? 255 : (val < 16 ? 128 : val * 2);
    rgb_matrix_set_color(led, 0, level, 0);
  }
  return false;
}
#endif // RGB_MATRIX_ENABLE
