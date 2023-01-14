-- removed to test performance
-- local vaicomlfs = require('lfs'); dofile(vaicomlfs.writedir()..[[Scripts\VAICOMPRO\VAICOMPRO.export.lua]])
local SimShakerlfs=require('lfs'); dofile(SimShakerlfs.writedir()..'Scripts/SimShaker.lua')
dofile(lfs.writedir()..[[Scripts\SimShaker-export-core\ExportCore.lua]])

--pcall(function() local dcsSr=require('lfs');dofile(dcsSr.writedir()..[[Mods\Services\DCS-SRS\Scripts\DCS-SimpleRadioStandalone.lua]]); end,nil);



-- my code to export my current plane data on the socket
local voiceblfs=require('lfs'); dofile(voiceblfs.writedir()..[[Scripts\voiceb\voicebExport.lua]])

