# Flashing on mac

I think these commenters were on my mac:

The only way I could flash my device was to run `make crkbd:default:avrdude` while it was plugged in (as per github foostan docs for corne-classic). Do this while the pro micro is plugged in, then press reset twice (may need to hold it down). Then when nothing apears to work, open qmk toolbox and it will see the pro micro. Then flash it with the hex file (which you already generated before with `make crkbd:default`).

# Flashing on Linux

Put the keyboard into reset mode (see the keymap.c file of what button does this).

First do setup, then

- `qmk compile`
- `qmk flash`

# My Keyboards

My first keyboard was a crkbd. My mapping is at https://github.com/jazzabeanie/qmk_firmware/tree/master/keyboards/crkbd/keymaps/jazzabeanie.

My 2nd was the Keebio Iris. Make this ready for flashing by holding the reset button when plugging it in.

# Setup

Setup according to https://docs.qmk.fm/#/newbs_building_firmware

`qmk config user.keyboard=keebio/iris/rev5`
`qmk config user.keymap=jazzabeanie`
`qmk setup`
