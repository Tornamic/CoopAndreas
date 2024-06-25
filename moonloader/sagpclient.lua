-----------CONFIG-----------
local ip = '127.0.0.1'
local port = 7777
local name = '{ff0000}Tornamic'
----------------------------


script_name('sagpclient')

package.path = package.path .. ';../?.lua;../?.luac;../../?.lua;../../?.luac;../?/?.lua'
package.cpath = package.cpath .. ';../?.dll;../../?.dll'

require 'SLNet'

local netHandle = SLNetInit() 
netHandle:connect(ip, port) 
netHandle:setPrefix('SAGP') 
local chatOpened = false
local chatTable = {}
-- local player_enum = {
-- 	ACTOR   = 1,
-- 	PX   = 2,
-- 	PY  = 3,
-- 	PZ = 4,
-- 	ANGLE = 5,
-- 	VX = 6,
-- 	VY = 7,
-- 	VZ = 8
-- }
local players = {}
function onReceiveData(packetID, bitStream)
    
    local packet = bitStream:read(UINT16) / 256
    if packet == 1 then -- chat
        chatTable[table.getn(chatTable) + 1] = bitStream:read(STRING, bitStream:read(UINT8)) 
    elseif packet == 2 then -- sync
        local iname = bitStream:read(STRING, bitStream:read(UINT8)) 
        
        if iname == name then return end
        
        local pxb = bitStream:read(BOOL)
        local px = bitStream:read(FLOAT)
        if pxb then px = math.abs(px) * -1 end
        
        local pyb = bitStream:read(BOOL)
        local py = bitStream:read(FLOAT)
        if pyb then py = math.abs(py) * -1 end
        
        local pzb = bitStream:read(BOOL)
        local pz = bitStream:read(FLOAT)
        if pzb then pz = math.abs(pz) * -1 end
        
        local a = bitStream:read(FLOAT)
        
        local vxb = bitStream:read(BOOL)
        local vx = bitStream:read(FLOAT)
        if vxb then vx = math.abs(vx) * -1 end
        
        local vyb = bitStream:read(BOOL)
        local vy = bitStream:read(FLOAT)
        if vyb then vy = math.abs(vy) * -1 end
        
        local vzb = bitStream:read(BOOL)
        local vz = bitStream:read(FLOAT)
        if vzb then vz = math.abs(vz) * -1 end
        
        
        if not table_contains(players, iname) then
            players[iname] = {}
            if not isModelAvailable(0) then requestModel(0) end
            players[iname][1] = createChar(4, 0, px, py, pz)
            --players[iname][2] = addBlipForChar(px, py, pz)
            markModelAsNoLongerNeeded(0)
        end
        --chatTable[table.getn(chatTable) + 1] = string.format("footsync %s pos %f %f %f angle %f vel %f %f %f", iname, px, py, pz, a, vx, vy, vz)
        
        setCharCoordinates(players[iname][1], px, py, pz - 1.6)
        setCharHeading(players[iname][1], a)
        setCharVelocity(players[iname][1], vx, vy, vz)
    end
end
netHandle:setHook(onReceiveData)

require "lib.moonloader"
memory = require("memory")
local font_flag = require('moonloader').font_flag
local my_font = renderCreateFont('Arial', 8, font_flag.BORDER)
function main()
    while not isOpcodesAvailable() do wait(1) end
    Patch()
    -- connecting
    local BSNew = BitStream:new()
    BSNew:write(UINT8, #name):write(STRING, name)
    SLNetSend(netHandle, 2, BSNew, 1)

    lua_thread.create(OnFootSync)
    while true 
    do
        wait(0)
        netHandle:loop()
        ProcessIncomingChat()
        ProcessChatOnOff()
        DrawPlayer3DTexts()
    end
end
function onExitScript(quitGame)
    local BSNew = BitStream:new()
    BSNew:write(UINT8, #name):write(STRING, name)
    SLNetSend(netHandle, 3, BSNew, 1)
end
local imgui = require 'imgui'
local encoding = require 'encoding' 
encoding.default = 'CP1251'
u8 = encoding.UTF8
local shwd = false
local input = imgui.ImBuffer(144)
function imgui.OnDrawFrame()
    imgui.SetNextWindowSize(imgui.ImVec2(400, 75), imgui.Cond.FirstUseEver)    
    imgui.Begin(u8'¬вод текста') 
    imgui.InputText('<--', input)
    if not shwd then 
        imgui.SetKeyboardFocusHere() 
        shwd = true
    end
    if imgui.Button('Send') then
        local BSNew = BitStream:new()
        BSNew:write(UINT8, #u8:decode(input.v)):write(STRING, u8:decode(input.v))
        SLNetSend(netHandle, 1, BSNew, 1)
        input.v = ''
        chatOpened = not chatOpened 
        imgui.Process = chatOpened 
        shwd = false
    end
    imgui.End()
end

function OnFootSync()
    while true do
        wait(1000)
        local pax, pay, paz = getCharCoordinates (PLAYER_PED)   
        local aa =          getCharHeading     (PLAYER_PED) 
        local vax, vay, vza = getCharVelocity    (PLAYER_PED)

        local BSNew = BitStream:new()
        BSNew:write(FLOAT, pax):write(FLOAT, pay):write(FLOAT, paz):write(FLOAT, aa):write(FLOAT, vax):write(FLOAT, vay):write(FLOAT, vaz)
        SLNetSend(netHandle, 4, BSNew, 1)
    end
end
function DrawPlayer3DTexts()
    for i, value in pairs(players) do
        local result = doesCharExist(players[i][1])
        if result and players[i][1] ~= PLAYER_PED and  isCharOnScreen(players[i][1])
        then
            local cX, cY, cZ = getCharCoordinates(players[i][1])
            local vX, vY, vZ = getCharVelocity(players[i][1])
            local pzX, pzY, pzZ = getCharCoordinates(PLAYER_PED)
            cZ = cZ + 1.0
            if(math.sqrt((cX - pzX)^2 + (cY - pzY)^2 + (cZ - pzZ)^2) < 30.0)
            then
                local wposX, wposY = convert3DCoordsToScreen(cX, cY, cZ)
                local text = i
                renderFontDrawText(my_font, text, wposX - renderGetFontDrawTextLength(my_font, text) / 2, wposY, 0xFFFFFFFF)
            end
        end
    end 
end
function ProcessChatOnOff()
    -- if isKeyJustPressed(VK_RETURN) and chatOpened and input.v ~= '' then
    --     local BSNew = BitStream:new()
    --     BSNew:write(UINT8, #u8:decode(input.v)):write(STRING, u8:decode(input.v))
    --     SLNetSend(netHandle, 1, BSNew, 1)
    --     input.v = ''
    --     chatOpened = not chatOpened 
    --     imgui.Process = chatOpened     
    --     shwd = false
    -- end
    if isKeyJustPressed(VK_F6) then
        input.v = ''
        chatOpened = not chatOpened 
        imgui.Process = chatOpened  
        shwd = false 
    end
end

function ProcessIncomingChat()
    if(table.getn(chatTable) > 0) then
        local i = table.getn(chatTable)
        local j = 15
        while i > 0 and j > 0 do
            renderFontDrawText(my_font, chatTable[i], 10, 15 * j, 0xFFFFFFFF)
            i=i-1
            j=j-1
        end
    end
end

function Patch()
    memory.fill(0x748063, 0x90, 5, true) -- Disable Pause Game
    memory.fill(0x609A4E, 0x90, 6, true) -- Lock Player Animation
    memory.fill(0x440833, 0x90, 8, true) -- Disable Interior Peds
    memory.fill(0x53C159, 0x90, 5, true) -- Disable Plants Objects
    --memory.fill(0x5B47B0, 0xC3, 1, true) -- Disable Weapon Pickups
    memory.write(0x588BE0, 0xC3, 1, true) -- Disable Printing Messages
    --memory.fill(0x58FBE9, 0x90, 5, true) -- Disable Vehicles Names
    memory.fill(0x56E5AD, 0x90, 5, true) -- Disable Wasted Message
    setPedDensityMultiplier(0.0) -- Disable Peds on the street
    setCarDensityMultiplier(0.0) -- Disable Cars on the street
    setMaxWantedLevel(0) -- Disable Wanted Level
    setOnlyCreateGangMembers(true) -- Disable Gang Wars
    setCreateRandomGangMembers(false) -- Disable Gang Members
    setFreeResprays(true) -- Disable Paid Resprays
    switchRandomTrains(false) -- Disable Trains
    setCreateRandomCops(false) -- Disable Police
    switchEmergencyServices(false) -- Disable EMS
    switchAmbientPlanes(false) -- Disable Planes
    memory.write(0x9690A0, 0, 4, true) -- Disable Car Generator #1
    -- Disable Car Generator #2
    memory.fill(0x53C1C1, 0x90, 5, true)
    memory.fill(0x434272, 0x90, 5, true)
    -- Disable spawn with cigars and bottles
    memory.fill(0x4217F4, 0x90, 21, true)
    memory.fill(0x4218D8, 0x90, 17, true)
    memory.fill(0x5F80C0, 0x90, 10, true)
    memory.fill(0x5FBA47, 0x90, 10, true)
end
function table_contains(tbl, x)
    found = false
    for v, _ in pairs(tbl) do
        if v == x then 
            found = true 
        end
    end
    return found
end
