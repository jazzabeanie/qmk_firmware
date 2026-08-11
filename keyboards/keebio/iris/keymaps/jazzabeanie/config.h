/*
Copyright 2017 Danny Nguyen <danny@keeb.io>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

// Handedness comes from SPLIT_HAND_PIN (D5) in the rev5 keyboard.json, which takes
// precedence over EE_HANDS, so this line has never had any effect. Kept as a note.
// #define EE_HANDS

// Enables the MI_* keycodes and midi_config. The firmware is size-tight, so this
// build also relies on LTO_ENABLE in rules.mk to fit. NO_ACTION_MACRO and
// NO_ACTION_FUNCTION used to be needed here too, but upstream removed both -
// they are no longer recognised and the space they saved is gone anyway.
#define MIDI_ADVANCED

// MIDI only ever reaches the half that has the USB cable, but the CC keys the
// DAW lights up are all on the left hand. This transaction carries the LED
// state to the other half so the feedback works whichever half is plugged in.
// The payload is 27 bytes, comfortably inside the 32-byte RPC_M2S_BUFFER_SIZE,
// so it goes across in a single transaction.
// Guarded so rev5, which has no rgb_matrix and so no feedback code, does not
// pay ~126 bytes for RPC machinery nothing registers a handler for.
#ifdef RGB_MATRIX_ENABLE
#    define SPLIT_TRANSACTION_IDS_USER CC_FEEDBACK_SYNC

// Send the layer state to the other half as well. QMK does not do this by
// default: the slave scans its matrix and hands the result to the master, which
// owns all the layer logic, so the slave's own layer_state never changes.
//
// rgb_matrix_indicators_advanced_user() runs on BOTH halves though, each over
// its own LEDs. Without this the slave reads layer_state as 0, decides it is
// not on _ABLETON, and keeps painting the running effect - so one half blanks
// and the other stays lit.
#    define SPLIT_LAYER_STATE_ENABLE
#endif

// rev8's keyboard.json turns on 43 animations, which makes RM_NEXT a 44-step
// slog. Cut it to nine. This file is added to CONFIG_H after the generated
// info_config.h (build_keyboard.mk:459), which is what makes #undef work here.
//
// Kept: SOLID_COLOR (always compiled in), ALPHAS_MODS, GRADIENT_UP_DOWN,
// RAINBOW_MOVING_CHEVRON, RAINDROPS, HUE_BREATHING, PIXEL_FRACTAL,
// TYPING_HEATMAP, DIGITAL_RAIN.

// Statics and bands
#undef ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#undef ENABLE_RGB_MATRIX_BREATHING
#undef ENABLE_RGB_MATRIX_BAND_SAT
#undef ENABLE_RGB_MATRIX_BAND_VAL
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#undef ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#undef ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL

// Hue cycling
#undef ENABLE_RGB_MATRIX_CYCLE_ALL
#undef ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#undef ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#undef ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#undef ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#undef ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#undef ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#undef ENABLE_RGB_MATRIX_DUAL_BEACON
#undef ENABLE_RGB_MATRIX_RAINBOW_BEACON
#undef ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS

// Random / textural
#undef ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#undef ENABLE_RGB_MATRIX_HUE_PENDULUM
#undef ENABLE_RGB_MATRIX_HUE_WAVE
#undef ENABLE_RGB_MATRIX_PIXEL_RAIN
#undef ENABLE_RGB_MATRIX_PIXEL_FLOW

// Every reactive effect. post_config.h switches RGB_MATRIX_KEYREACTIVE_ENABLED
// on if any single one of these survives, so dropping the whole group is what
// actually removes the keypress-tracking overhead.
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
#undef ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
#undef ENABLE_RGB_MATRIX_SPLASH
#undef ENABLE_RGB_MATRIX_MULTISPLASH
#undef ENABLE_RGB_MATRIX_SOLID_SPLASH
#undef ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
