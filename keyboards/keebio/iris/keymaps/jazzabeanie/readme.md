notes on the matrix for swap hands
===================

see `/keyboards/keebio/iris/rev5/rev5.h` for how the matrix is set up. Basically, left hand starts with row 0, down to row 4 is the special keys and wraps around the side (also first two keys are nothing). Then top row of right hand is row 5 continuing from there.

on the right hand, the column index starts counting from the right, id, backspace column is 0, 'h' column is 5

returns col, row

q = 1, 1
a = 1, 2

4, 6 overflows to 5, 0
