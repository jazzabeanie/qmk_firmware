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
// DAW lights up are all on the right hand. This transaction carries the LED
// state to the other half so the feedback works whichever half is plugged in.
// The payload is 26 bytes, comfortably inside the 32-byte RPC_M2S_BUFFER_SIZE,
// so it goes across in a single transaction.
// Guarded so rev5, which has no rgb_matrix and so no feedback code, does not
// pay ~126 bytes for RPC machinery nothing registers a handler for.
#ifdef RGB_MATRIX_ENABLE
#    define SPLIT_TRANSACTION_IDS_USER CC_FEEDBACK_SYNC
#endif
