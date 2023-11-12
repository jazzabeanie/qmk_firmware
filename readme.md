# Flashing on mac

I think these commenters were on my mac:

The only way I could flash my device was to run `make crkbd:default:avrdude` while it was plugged in (as per github foostan docs for corne-classic). Do this while the pro micro is plugged in, then press reset twice (may need to hold it down). Then when nothing apears to work, open qmk toolbox and it will see the pro micro. Then flash it with the hex file (which you already generated before with `make crkbd:default`).

I have since got it to it work by compiling the hex file with `qmk compile -kb crkbd -km jazzabeanie-midi`, then use qmk toolbox to load the hex file. Tick on Auto-flash, then connect the keyboard and press the reset button (short GND & RST pins).

This is a keyboard firmware based on the [tmk\_keyboard firmware](https://github.com/tmk/tmk_keyboard) with some useful features for Atmel AVR and ARM controllers, and more specifically, the [OLKB product line](https://olkb.com), the [ErgoDox EZ](https://ergodox-ez.com) keyboard, and the Clueboard product line.

# Flashing on Linux

Put the keyboard into reset mode (see the keymap.c file of what button does this).

First do setup, then

- `qmk compile`
- `qmk flash`

# My Keyboards

My first keyboard was a crkbd. My mapping is at https://github.com/jazzabeanie/qmk_firmware/tree/master/keyboards/crkbd/keymaps/jazzabeanie.

My 2nd was the Keebio Iris. Make this ready for flashing by holding the reset button when plugging it in. I think this was rev5

My 3rd was the Keebio Iris rev8. I have put the jazzabeanie keymap on it. Either side can enter bootloader mode by holding both lower and raise and pressing the home or end button (the innter most button).

# Setup

Setup according to https://docs.qmk.fm/#/newbs_building_firmware

`qmk config user.keyboard=keebio/iris/rev5`
`qmk config user.keymap=jazzabeanie`
`qmk setup`
