#!/usr/bin/env python3.9
# -*- coding: utf-8 -*-
# ############################################################################
#
# output.py
# 12/07/2022 (c) Juan M. Casillas <juanm.casillas@gmail.com>
#
# manage the output of the program to be able to write a text control.
#
# ############################################################################

import sys

class OutputManager:
    __file = sys.stdout
    __invoke = None
    
    @staticmethod
    def config(file=None, invoke=None):
        if not file:
            return OutputManager.__file
        else:
            OutputManager.__file = file

        if invoke:
            OutputManager.__invoke = invoke

    @staticmethod
    def log(*args, **kwargs):
        sep = kwargs.pop("sep", ' ')
        end = kwargs.pop("end", '\n')
        file = kwargs.pop("file", OutputManager.__file)
        flush = kwargs.pop("end", False)

        print(*args, sep=sep, end=end, file=file, flush=flush)
        if OutputManager.__invoke:
            OutputManager.__invoke()


if __name__ == "__main__":
    OutputManager.log("hello world")
    OutputManager.log("one","two","three", sep="\n", flush=True)

    fd = open("output.log","w+")
    OutputManager.config(fd)
    OutputManager.log("this line, to the file", sep="\n", flush=True)
