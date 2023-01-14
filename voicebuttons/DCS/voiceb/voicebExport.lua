g_lfs=require('lfs')
package.path  = package.path..";"..g_lfs.writedir().."Scripts\\voiceb\\?.lua"
package.cpath = package.cpath..";"..g_lfs.writedir().."Scripts\\voiceb\\?.dll"
do
	if g_voiceBInit==nil then
		g_voiceBInit=true

		local _voiceb={}
		_voiceb.selfData={}
		_voiceb.clock=0
		_voiceb.delay=1.0
		_voiceb.interval=0.10
        _voiceb.json=loadfile("Scripts\\JSON.lua")()
		_voiceb.net=require("voicebNetwork")
        _voiceb.net.start()
        local _send={}
		_voiceb.net.send("ready")
		do

			_voiceb.LuaExportStart=function()
				log.write("voiceB",log.INFO,"Export start!")
			end
			
			_voiceb.LuaExportBeforeNextFrame=function()
			end

			_voiceb.getNet=function(t)
				local data=LoGetSelfData()
                if data~=nil and data.Name~=nil then
                    local _send={}
				    _send["cmd"]="mod"
				    _send["value"]=data.Name
                    local _msg =  _voiceb.json:encode(_send)
                    _voiceb.net.send(_msg)
                    --log.write("voiceB",log.INFO,_msg)
                end
			end
			
			_voiceb.LuaExportActivityNextEvent=function(t)
				_voiceb.clock=t+_voiceb.interval
				_voiceb.getNet(_voiceb.clock)
				return _voiceb.clock
			end
			
			_voiceb.LuaExportStop=function()
                local _send={}
                _send["cmd"]="exit"
                _send["value"]="true"
                local _msg =  _voiceb.json:encode(_send)
				_voiceb.net.send(_msg)
				log.write("voiceB",log.INFO,"Export stop!")
			end

			_voiceb.OtherLuaExportStart=LuaExportStart								
			_voiceb.OtherLuaExportBeforeNextFrame=LuaExportBeforeNextFrame			
			_voiceb.OtherLuaExportActivityNextEvent=LuaExportActivityNextEvent		
			_voiceb.OtherLuaExportStop=LuaExportStop								
			
			LuaExportStart=function()
				_voiceb.LuaExportStart()
				if _voiceb.OtherLuaExportStart then
					_voiceb.OtherLuaExportStart()
				end
			end
			
			LuaExportBeforeNextFrame=function()
				_voiceb.LuaExportBeforeNextFrame()
				if _voiceb.OtherLuaExportBeforeNextFrame then
					_voiceb.OtherLuaExportBeforeNextFrame()
				end
			end
			
			LuaExportActivityNextEvent=function(t)
				t=_voiceb.LuaExportActivityNextEvent(t)
				if _voiceb.OtherLuaExportActivityNextEvent then
					t=_voiceb.OtherLuaExportActivityNextEvent(t)
				end
				return t
			end
			
            LuaExportStop=function()
                _voiceb.LuaExportStop()
                if _voiceb.OtherLuaExportStop then
                    _voiceb.OtherLuaExportStop()
                end
            end
            log.write("voiceB",log.INFO,"voiceB export installed!")
        end
	end
end
