#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ############################################################################
#
# buttons.py
# 09/10/2020 (c) Juan M. Casillas <juanm.casillas@gmail.com>
#
# Create a list of defined buttons in hex for the lib
#
# ############################################################################

import math
BUTTONS=128
for i in range(0,BUTTONS):
    #print("#DEFINE BUTTON_{:d} {:>28}".format(i+1, "0x{:x}".format(int(math.pow(2,i)))))
    #print("#define BUTTON_{:d} {:>18}".format(i+1, "0x{:x}".format(i+1)))

    print("{:>10} {:>50} {:>150}".format(
        "{:d}".format(i+1),
        "0x{:x}".format(int(math.pow(2,i))),
        "{:b}".format(1<<(i+1-1))
    ))