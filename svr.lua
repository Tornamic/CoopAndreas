package.path = package.path .. ';../?.lua;../?.luac;../../?.lua;../../?.luac'
package.cpath = package.cpath .. './?.dll'

require 'SLNet'

netHandle = SLNetInit() 

netHandle:bind('*', 7777) 
netHandle:setPrefix('SAGP') 

local playersIp = {}
local playersPort = {}
local playersName = {}
function SendPlayerChat(text, addr, port)
    local BSNew = BitStream:new() 
    
    BSNew:write(UINT16, 1):write(UINT8, #text):write(STRING, text)
    SLNetSend(netHandle, 1, BSNew, addr, port, 1) 
end

function onReceiveData(packetID, bitStream, addr, port)
    effil.thread(function()

        print('Message from ' .. addr .. ':' .. port .. ' packetid ' .. packetID / 256)
        local packet = packetID / 256
        
        if packet == 1 then -- chat
            local text = bitStream:read(STRING, bitStream:read(UINT8))
            local msg = playersName[addr]..':{ffffff} '..text
            for key, value in pairs(playersIp) do
                SendPlayerChat(msg, value, playersPort[key])
            end
        elseif packet == 2 then -- connect
            local name = bitStream:read(STRING, bitStream:read(UINT8))
            print(string.format("connect %s", name))
            playersIp[name] = addr
            playersPort[name] = port
            playersName[addr] = name
        elseif packet == 3 then -- disconnect
            local name = bitStream:read(STRING, bitStream:read(UINT8))
            print(string.format("disconnect %s", name))
            table.remove(playersIp, name)
            table.remove(playersPort, name)
            table.remove(playersName, addr)
        elseif packet == 4 then -- sync on foot
            local name = playersName[addr]
            local px = bitStream:read(FLOAT)
            local py = bitStream:read(FLOAT)
            local pz = bitStream:read(FLOAT)
            local a = bitStream:read(FLOAT)
            local vx = bitStream:read(FLOAT)
            local vy = bitStream:read(FLOAT)
            local vz = bitStream:read(FLOAT)
            local startTime = bitStream:read(FLOAT)
            local aname =     bitStream:read(STRING, bitStream:read(UINT16))
            local alib =      bitStream:read(STRING, bitStream:read(UINT16))
            local loop =      bitStream:read(BOOL)
            local lockx =     bitStream:read(BOOL)
            local locky =     bitStream:read(BOOL)
    
            if name == nil then return end
            print(string.format("footsync %s pos %f %f %f angle %f vel %f %f %f", name, px, py, pz, a, vx, vy, vz))
            for key1, value1 in pairs(playersIp) do -- игрок которому отправляем
                    
                local BSNew = BitStream:new() 
    
                BSNew:write(UINT16, 2):write(UINT8, #name):write(STRING, name):write()
                BSNew:write(BOOL, px < 0):write(FLOAT, px):write(BOOL, py < 0):write(FLOAT, py):write(BOOL, pz < 0):write(FLOAT, pz)
                BSNew:write(FLOAT, a)
                BSNew:write(BOOL, vx < 0):write(FLOAT, vx):write(BOOL, vy < 0):write(FLOAT, vy):write(BOOL, vz < 0):write(FLOAT, vz)
                BSNew:write(FLOAT, starttime):write(UINT16, #aname):write(STRING, aname):write(UINT16, #alib):write(STRING, alib)
                BSNew:write(BOOL, loop)
                SLNetSend(netHandle, 2, BSNew, value1, playersPort[key1], 1) 
            end
        end
    end)
end

netHandle:setHook(onReceiveData) 
print('Hello')
while true do
    netHandle:loop()  
end