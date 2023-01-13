#!/usr/bin/env python3.9
# -*- coding: utf-8 -*-
# ############################################################################
#
# voiceb.py
# 11/17/2022 (c) Juan M. Casillas <juanm.casillas@gmail.com>
#
# read a config file, talk when a button is pressed, or released.
#
# ############################################################################


from voicebuttons import *
import argparse
import sys
import platform

import warnings
warnings.filterwarnings("ignore")
# python39\lib\site-packages\pyglet\libs\win32\__init__.py:326: UserWarning: Could not set COM MTA mode. Unexpected behavior may occur.

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbose", help="Show data about file and processing (Debug)", action="count", default=0)
    parser.add_argument("-c", "--config", help="reads the configuration from <config>", default="config/config.json")
    parser.add_argument("-l", "--list", help="show ships/templates available (planes/helos)", action="store_true")
    parser.add_argument("-o", "--logfile", help="Outputs to the defined file. Default = 'stdout'", default=sys.stdout)

    args = parser.parse_args()

    App.config(args.config)
    App.config_set('platform',platform.system()) 
    App.config_set('verbose',args.verbose)



    if args.logfile != sys.stdout:
        output_fd = open(args.logfile,"w+")
        OutputManager.config(output_fd) 

    if CONFIG().verbose >= 1:
        for device in CONFIG().devices:
            OutputManager.log("{:-<80}".format('- Configured Commands for device: %s ' % device["name"]))
            OutputManager.log("{:<20} {:<30} {:<20}".format("button", "msg", "event (press/release/toggle)"))
            OutputManager.log("{:-<80}".format('-'))
            for i in device["commands"]:
                OutputManager.log("{:<20} {:<30} {:<20}".format(i['name'], i['msg'], i['on']))

    manager = VoiceButtonsClass(CONFIG().voice)    
    
    if args.list:
        manager.list_devices()
        sys.exit(0)

    for device in CONFIG().devices:
        manager.add_device(device["name"], device["commands"])

    manager.run()
