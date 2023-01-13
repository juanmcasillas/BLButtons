##!
## E:\Software\python39\Scripts\pip.exe install pywin32 
## E:\Software\python39\Scripts\pip.exe install pyglet 

import win32com.client
import pyglet
from time import sleep

# see here https://blog.thea.codes/talking-to-gamepads-without-pygame/
import re
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
        self.speaker.Speak(text)


def map_control(device, control, onevent, msg, button, talk):
    if isinstance(control, pyglet.input.base.Button):

        if onevent == "press":
            @control.event
            def on_press():
                talk.speak(msg)
                if CONFIG().verbose > 2:
                    #OutputManager.log('%s: %r.on_press()' % (device.name, control))
                    OutputManager.log('%s: %s.on_press() [%s]' % (device.name, button, msg))


        if onevent == "release":
            @control.event
            def on_release():
                talk.speak(msg)
                if CONFIG().verbose > 2:
                    #OutputManager.log('%s: %r.on_release()' % (device.name, control))
                    OutputManager.log('%s: %s.on_press() [%s]' % (device.name, button, msg))


class VoiceButtonsClass:
    def __init__(self, voice):
        self.device = None
        self.talk = TalkClass()
        self.talk.set_voice(voice)

    def add_device(self, device_name, commands):

        self.device = None
        devices = pyglet.input.get_devices()
        for dev in devices:
            if dev.name == device_name:
                self.device = dev
                break

        if not self.device:
            raise Exception("can't find device %s" % device_name)

        self.device.open()
        controls = self.device.get_controls()

        if CONFIG().verbose > 1:
            OutputManager.log("{:-<80}".format('-'))

        for control in controls:

            for command in commands:

                regex = re.compile("^Button (\d+)$")

                onevent = command["on"].lower()
                msg = command["msg"]

                regdata = regex.match(control.raw_name)

                if regdata:
                    button = regdata.groups(1)[0]
                    full_name = "Button %d" % (int(button)+1)
                       
                    if full_name == command["name"]:
                        text = msg.format(button=button, on=onevent)
                        map_control(self.device, control, onevent, text, full_name, self.talk)
                        # only work on buttons, for now.
                        # if isinstance(control, pyglet.input.base.Button) and onevent in [ "press"]:
                        #     @control.event
                        #     def on_press():
                        #         if CONFIG().verbose > 2:
                        #             OutputManager.log('%s: %s.on_press()' % (self.device, control.raw_name))
                        #             self.talk.speak(msg.format(button=button, on=onevent))

                        # if isinstance(control, pyglet.input.base.Button) and onevent in [ "release"]:
                        #     @control.event
                        #     def on_release():
                        #         if CONFIG().verbose > 2:
                        #             OutputManager.log('%s: %s.on_release()' % (self.device, control.raw_name))
                        #             self.talk.speak(msg.format(button=button, on=onevent))
                        if CONFIG().verbose > 1:
                            OutputManager.log("Mapped '%s' on '%s' (event: %s) msg: %s" % (command["name"], control.raw_name, onevent, msg))
             

                    
    def list_devices(self):

        #print(f"0x{device['vendor_id']:04x}:0x{device['product_id']:04x} {}")
        # 0x2341:0x8036 Arduino Leonardo
        # 0x8087:0x0a1e 
        # 0x2341:0x8036 Arduino Leonardo
        # 0x3344:0x82c4 L-VPC Rotor TCS
        # 0x1e7d:0x2e22 ROCCAT Kone XTD
        # 0x3344:0x82c4 L-VPC Rotor TCS
        # 0xac05:0x0a82 BL-Combo (KMG)
        # 0x1c4f:0x0034 Usb Mouse
        # 0x044f:0xb68f T-Pendular-Rudder
        # 0xac05:0x0a82 BL-Combo (KMG)
        # 0xac05:0x0a82 BL-Combo (KMG)
        # 0xbd05:0x0b82 BL-Radio

        names = []
        OutputManager.log("{:-<80}".format('- Avaliable devices '))
        devices = pyglet.input.get_devices()
        for device in devices:
            name = device.name
            if name not in names and name != "":
                names.append(name)
        
        for n in names:
            OutputManager.log(n)
            


    def run(self):
        pyglet.app.run()


            
