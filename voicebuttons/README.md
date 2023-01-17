# VoiceButtons

Create a talk-able interface to say what you want when pushes Joystick Buttons. Started to map BL-Radio mode buttons, but ended to map DCS planes radio configurations, using LUA. The socket is a *broadcast UDP socket* with Address: `127.0.0.1` and port `64998`

## JSON Messages

The JSON messages exported from DCS are:

* `start`: at the begining of the simulation
* `mod`:  periodically the mod info
* `exit`: at the end of the simulation

```json
{ "cmd": "start", "value": true}
{ "cmd": "mod", "value": "UH-1H"}
{ "cmd": "exit", "value": true}
```

The `.lua` code is an adaptation of `winwing` export `lua` code for their devices. It daisy-chains other modules, so its safe include it at the end of the `Export.lua` file.



## Install on DCS.

You should run the script with `-d` option. `-vvv` recommended to debug the codes. This mode allows you
to have profiles for each plane, so the radios can be mapped to different planes.

`Scripts\Export.lua`

Add 

```json
-- my code to export my current plane data on the socket
local voiceblfs=require('lfs'); dofile(voiceblfs.writedir()..[[Scripts\voiceb\voicebExport.lua]])
```

## Config options

There are multiple commands that allows you to customize the beheavior of the app:

### Press

Define a command with `on=press` mean the text `msg` will be speech when the button is **pressed**. Most used
with temporary switches (buttons).

```json
{  
    "name"  : "Button 121",
    "msg"   : "Uniform radio",
    "on"    : "press"
},
```

### Release

Define a command with `on=release` mean the text `msg` will be speech when the button is **released**. Useful
to use with permanent switches (e.g. you define a `press` action for switch up, and a `release` action for 
switch `down`, e.g. landing gear lever).

```json
{  
    "name"  : "Button 121",
    "msg"   : "Uniform radio",
    "on"    : "release"
},
```


### Toggle

Define a command with `on=toggle` mean the text `msg` will be speech when the button changes their state,
between two posible values. These values can be customized using the `values` array, so you can say things
like "on", "engaged", etc.

```json
 {  
    "name"  : "Button 1",
    "msg"   : "default AutoPilot {status}",
    "on"    : "toggle",
    "values": ["ON","OFF"]
}
```

This example will say "default Autopilot ON", and "default Autopilot OFF". First state is always ON (from OFF to ON).
note the `{status}` template item yo can position in your phrase.

### Time

A small utility that says the current time, in 24-hour format. Useful to check the time without wear off the glasses.

```json
{  
    "name": "Button 13",
    "msg" : "{time}",
    "on"  : "press"
}
```

## Config file example

* All the text is speak in the default windows 10 language, in my case, english.
* `voice` can be `DAVID` or `ZIRA`, for now.
* `addr` and `port` locates the export data from DCS port. Uses a UDP broadcast port, with JSON format.
* `delay` periodically run the server at this time (float, seconds 0.1). Not need to touch it (checks the MOD change only)
* `mods` define an array of planes, with their specific radio configuration for the modes. You **must** define a `default` plane in order to catch all non defined mods.

Each mod (plane) is defined as a `name` that must match the MOD id in DCS, and `commands` array, that maps the buttons to voice messages.

The radio button box have 8 "special" buttons activated when the rotary selector is changed:

| **Mode** | **Button** |
|:--------:|:----------:|
|     1    |     121    |
|     2    |     122    |
|     3    |     123    |
|     4    |     124    |
|     5    |     125    |
|     6    |     126    |
|     7    |     127    |
|     8    |     128    |


So you can map any action to the selector. I recommend **NOT MAPPING** the mode buttons in DCS to any action, but if
you find it useful, you can.


```json
{
    "voice": "ZIRA",
    "addr": "127.0.0.1",
    "port": 64998,
    "delay": 0.1,
    "mods": [
        {
            "name": "UH-1H",
            "devices": [
                { 
                    "name": "BL-Radio",
                    "commands": [
                        {  
                            "name"  : "Button 121",
                            "msg"   : "Uniform radio",
                            "on"    : "press"
                        },
                        {  
                            "name"  : "Button 122",
                            "msg"   : "NAV radio",
                            "on"    : "press"
                        },
                        {  
                            "name"  : "Button 123",
                            "msg"   : "Alpha Mike radio",
                            "on"    : "press"
                        },
                        {  
                            "name"  : "Button 124",
                            "msg"   : "Fox Mike radio",
                            "on"    : "press"
                        },
                        {  
                            "name"  : "Button 125",
                            "msg"   : "ADF radio",
                            "on"    : "press"
                        }
                    ]
                },
                { 
                    "name": "BL-Combo (KMG)",
                    "commands": [
                        {  
                            "name": "Button 13",
                            "msg" : "{time}",
                            "on"  : "press"
                        }
                    ]
                }
            ]
        },
        {
            "name": "default",
            "devices": [
                { 
                    "name": "BL-Radio",
                    "commands": [
                        {  
                            "name"  : "Button 1",
                            "msg"   : "default AutoPilot {status}",
                            "on"    : "toggle",
                            "values": ["ON","OFF"]
                        }
                    ]
                },
                { 
                    "name": "BL-Combo (KMG)",
                    "commands": [
                        {  
                            "name": "Button 19",
                            "msg" : "Eject",
                            "on"  : "press"
                        }
                    ]
                }
            ]
        }
    ]   
}
```
