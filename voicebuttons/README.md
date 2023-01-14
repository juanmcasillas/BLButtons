# VoiceButtons

Create a talk-able interface to say what you want when pushes Joystick Buttons. Started to map
BL-Radio mode buttons, but ended to map DCS planes radio configurations, using LUA.

Broadcast UDP socket
Address: 127.0.0.1
Port:    64998

## Messages

```
{ "cmd": "mod", "value": "UH-1H"}
{ "cmd": "exit", "value": true}
```


## Install on DCS.

`Scripts\Export.lua`

Add 
```
-- my code to export my current plane data on the socket
local voiceblfs=require('lfs'); dofile(voiceblfs.writedir()..[[Scripts\voiceb\voicebExport.lua]])
```

