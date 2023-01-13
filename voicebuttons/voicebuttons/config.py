#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# ############################################################################
#
# config.py
# 11/17/2022 (c) Juan M. Casillas <juanm.casillas@gmail.com>
#
# Implements basic config using a global allocation (App) and JSON
# Load config: App.config(args.config)
# Use it OutputManager.log(App.config().template_dir)
#
# ############################################################################

import json
from types import SimpleNamespace
import os
import os.path


class ConfigClass:
    "must match exactly the json file"
    def __init__(self, device, voice="ZIRA", commands=[]):
        self.device = device
        self.voice = voice
        self.commands = commands

        # options set by config_set()
        self.verbose = 0
        self.config_dir = None
        self.config_file = None

    def validate(self):
        voices = ["DAVID", "ZIRA"]
        if self.voice.upper() not in voices:
            raise Exception("voice %s not valid: Options: %s" % (self.voice, voices.keys()))




class App:
    __conf = None

    @staticmethod
    def config(fname=None):
        if not fname:
            return App.__conf
        else:
            try:
                with open(fname) as json_data_file:
                    data = json.load(json_data_file)
                    #data = json.load(json_data_file, object_hook=lambda d: SimpleNamespace(**d))

                    App.__conf = ConfigClass(**data)

                App.__conf.validate()
                App.__conf.config_dir = os.path.dirname(fname)
                App.__conf.config_file = fname

                return App.__conf

            except Exception as e:
                raise Exception(e) from e
    
    @staticmethod
    def config_set(var, value):
        App.__conf.__dict__[var] = value
        return App.__conf
    
   
def CONFIG():
    "syntactic sugar for the other modules"
    return App.config()


