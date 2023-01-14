package.path  = package.path..";.\\LuaSocket\\?.lua"
package.cpath = package.cpath..";.\\LuaSocket\\?.dll"
w_net={}
w_net.socket={}
w_net.addr={
	{ip="127.0.0.1",port=64998},
}

function w_net.start()
	w_net.socket=require("socket")
	w_net.net = w_net.socket.udp()
	w_net.net:setsockname("*", 64998)
	w_net.net:setoption('broadcast', true)
	w_net.net:settimeout(0)
end

function w_net.send(msg)
	for i,_addr in pairs(w_net.addr) do
		w_net.socket.try(w_net.net:sendto(msg,_addr.ip,_addr.port))
	end
end

function w_net.stop()
	w_net.net:close()
end

return w_net