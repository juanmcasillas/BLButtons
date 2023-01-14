##!
## E:\Software\python39\Scripts\pip.exe install pywin32 
## E:\Software\python39\Scripts\pip.exe install pyglet 

import win32com.client
import pyglet
from time import sleep

# see here https://blog.thea.codes/talking-to-gamepads-without-pygame/
import re
import socket
from time import sleep
import json
from types import SimpleNamespace
from datetime import datetime

from .output import OutputManager
from .config import CONFIG

class TalkClass:
    DAVID = 'DAVID'
    ZIRA = 'ZIRA'
    
    voices = { 
        'DAVID': 0, 
        'ZIRA':  1
    }

    def __init__(self):
        self.speaker = win32com.client.Dispatch("SAPI.SpVoice")
        pass

    def set_voice(self, voice):
        vcs = self.speaker.GetVoices()
        self.speaker.Voice      # this line IS REQUIRED in order to work (maybe to init the interface)
        self.speaker.SetVoice(vcs.Item(TalkClass.voices[voice]))

    def speak(self, text):
        flags = 1 
        # https://learn.microsoft.com/en-us/previous-versions/windows/desktop/ms720892(v=vs.85)
        # 'SpVoice Flags
        # SVSFDefault = 0
        # SVSFlagsAsync = 1
        # SVSFPurgeBeforeSpeak = 2
        # SVSFIsFilename = 4
        # SVSFIsXML = 8
        # SVSFIsNotXML = 16
        # SVSFPersistXML = 32

        # 'Normalizer Flags
        # SVSFNLPSpeakPunc = 64

        # 'TTS Format
        # SVSFParseSapi = 
        # SVSFParseSsml = 
        # SVSFParseAutoDetect = 

        # 'Masks
        # SVSFNLPMask = 64
        # SVSFParseMask = 
        # SVSFVoiceMask = 127
        # SVSFUnusedFlags = -128
        flags = 1 | 2   # SVSFlagsAsync | SVSFPurgeBeforeSpeak 
        self.speaker.Speak(text, flags)






def map_control(device_name, control, onevent, msg, button, values, talk, parent):
    if isinstance(control, pyglet.input.base.Button):

        if onevent == "press":
            @control.event
            def on_press():
                msg_l = msg
                if msg_l.find("$time") >=0:
                    repl = datetime.now().strftime("%H %M")
                    msg_l = msg_l.replace("$time", repl)
                talk.speak(msg_l)
                if CONFIG().verbose > 2:
                    OutputManager.log('%s: %s.on_press() [%s]' % (device_name, button, msg))


        if onevent == "release":
            @control.event
            def on_release():
                talk.speak(msg)
                if CONFIG().verbose > 2:
                    OutputManager.log('%s: %s.on_press() [%s]' % (device_name, button, msg))

        
        if onevent == "toggle":

            @control.event
            def on_press():
                state = parent.devices[device_name]['state'][button]
                parent.devices[device_name]['state'][button] = not state

                state = values[0] if not state else values[1]
                msg_l = msg.format(status=state)
       
                talk.speak(msg_l)
                if CONFIG().verbose > 2:
                    OutputManager.log('%s: %s.on_press() [%s] <toggle>' % (device_name, button, msg_l))


class VoiceButtonsClass:
    def __init__(self, voice, mode):
        self.devices = {}
        self.talk = TalkClass()
        self.talk.set_voice(voice)
        self.mode = mode
        self.current_mod = None
        self.mods = {}
        self.mod_default = None

    def add_device(self, device_name, commands):

        device = None
        devices = pyglet.input.get_devices()
        for dev in devices:
            if dev.name == device_name:
                device = dev
                break

        if not device:
            raise Warning("can't find device %s" % device_name)

        self.devices[device_name] = {'device': device, 'state': {}}
        self.devices[device_name]['device'].open()
        controls = self.devices[device_name]['device'].get_controls()

        if CONFIG().verbose > 1:
            OutputManager.log("{:-<80}".format('-'))

        for control in controls:

            for command in commands:

                regex = re.compile("^Button (\d+)$")

                onevent = command["on"].lower()
                msg = command["msg"]
                values = command["values"] if "values" in command else []

                regdata = regex.match(control.raw_name)

                if regdata:
                    button = regdata.groups(1)[0]
                    full_name = "Button %d" % (int(button)+1)
                       
                    if full_name == command["name"]:
                            
                        self.devices[device_name]['state'][full_name] = False
                        if onevent in ["press", "release"]:
                            text = msg.format(button=button, on=onevent)
                        else:
                            text = msg
                        map_control(device_name, control, onevent, text, full_name, values, self.talk, self)
                        if CONFIG().verbose > 1:
                            OutputManager.log("%s Mapped %s on '%s' (event: %s) msg: %s" % (device_name,command["name"], control.raw_name, onevent, msg))
             

                    
    def list_devices(self):

        names = []
        OutputManager.log("{:-<80}".format('- Avaliable devices '))
        devices = pyglet.input.get_devices()
        for device in devices:
            name = device.name
            if name not in names and name != "":
                names.append(name)
        
        for n in names:
            OutputManager.log(n)

    def start_server_dcs(self):
        if CONFIG().verbose >= 1:
            OutputManager.log("{:=<80}".format('= Configured Modules for DCS: %s:%d ' % (CONFIG().addr, CONFIG().port)))

            for mod in CONFIG().mods:
                OutputManager.log("{:<80}".format('* Configured Commands for MOD: %s' % mod["name"]))
                for device in  mod["devices"]:
                    OutputManager.log("{:-<80}".format('- Configured Commands for device: %s ' % device["name"]))
                    for i in device["commands"]:
                        OutputManager.log("    {:<20} {:<30} {:<20}".format(i['name'], i['msg'], i['on']))

        # create the dict.
        self.mods = {}
        self.mod_default = None
        for mod in CONFIG().mods:
            self.mods[mod["name"]] = mod
            if mod["name"] == "default":
                self.mod_default = mod

        # start the listening part from DCS.

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        self.sock.bind((CONFIG().addr, CONFIG().port))

        event_loop = pyglet.app.EventLoop()

        pyglet.clock.schedule_interval(self.run_server, CONFIG().delay)
        self.current_mod = None
        event_loop.run()

    def run_server(self, value):

            data, addr = self.sock.recvfrom(1024)
            try:
                msg = json.loads(data, object_hook=lambda d: SimpleNamespace(**d))
            except Exception as e:
                print("error parsing data: %s (%s)" % (e,data))
                return

            # 
            # process here the cmd
            # 
            if msg.cmd == "mod":
                if self.current_mod != msg.value:
                    self.current_mod = msg.value
                    if CONFIG().verbose > 1:
                        OutputManager.log("loading mod %s" % msg.value)
                    self.register_mod()

            if msg.cmd == "exit" and msg.value:
                if CONFIG().verbose > 1:
                    OutputManager.log("simulation exit")
                    self.current_mod = None
                




    def register_mod(self):
        mod = self.mods[self.current_mod] if self.current_mod in self.mods else self.mod_default
        if not mod:
            raise Exception("no mod found (%s) and not default mod. Exiting" % self.current_mod)

        # close and clear current config.
        for device in self.devices.keys():
            self.devices[device]['device'].close()
        
        self.devices = {}
        for device in mod["devices"]:
            self.add_device(device["name"], device["commands"])



    def start_server_default(self):
        if CONFIG().verbose >= 1:
            for device in CONFIG().devices:
                OutputManager.log("{:-<80}".format('- Configured Commands for device: %s ' % device["name"]))
                OutputManager.log("{:<20} {:<30} {:<20}".format("button", "msg", "event (press/release/toggle)"))
                OutputManager.log("{:-<80}".format('-'))
                for i in device["commands"]:
                    OutputManager.log("{:<20} {:<30} {:<20}".format(i['name'], i['msg'], i['on']))

        for device in CONFIG().devices:
            self.add_device(device["name"], device["commands"])

        self.run()


    def start(self):
        if self.mode == "DCS":
            self.start_server_dcs()
        else:
            self.start_server_default()


    def run(self):
        event_loop = pyglet.app.EventLoop()
        event_loop.run()

        

            
