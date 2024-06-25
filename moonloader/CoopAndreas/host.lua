package.path = package.path .. ';../?.lua;../?.luac;../../?.lua;../../?.luac;../?/?.lua'
package.cpath = package.cpath .. ';../?.dll;../../?.dll'

--------------------------------------------

script_name('coopandreas_host')
script_author('Tornamic')
script_properties("work-in-pause")
--------------------------------------------

ffi = require 'ffi'
local inicfg = require 'inicfg'
local cfg = inicfg.load(nil, "moonloader\\CoopAndreas.cfg")

--------------------------------------------

require 'SLNet'

--------------------------------------------

require 'CoopAndreas.anims'
require 'CoopAndreas.commands_handler'
require 'CoopAndreas.car_functions'
require 'CoopAndreas.char_functions'
require 'CoopAndreas.npc_sync_out'

--------------------------------------------

netHandle = SLNetInit() 

--------------------------------------------

netHandle:bind('*', tonumber(cfg.Settings.Port)) 
netHandle:setPrefix('CPAND') 

--------------------------------------------

local chatOpened = false
local chatTable = {}
friend = nil
friendIp = ''
friendPort = 0
local friendMarker = nil
local friendLastPacketTime = 0
local friendVehicle = nil
local friendWeapon = 0
local friendAmmo = 0
local friendHP = 0
local friendArmour = 0

--------------------------------------------

function onReceiveData(packetID, bitStream, addr, port)
    friendLastPacketTime = os.clock()
    friendIp = addr
    friendPort = port
    if packetID == 1 then
        local message =   '{ff0000}Client:{ffffff} '..bitStream:read(STRING, bitStream:read(UINT8))
        chatTable[table.getn(chatTable) + 1] = message
        local bs = BitStream:new()
        bs:write(UINT8, #message):write(STRING, message)
        SLNetSend(netHandle, 1, bs, friendIp, friendPort, 1)
    elseif packetID == 2 then -- footsync
        -- lua_thread.create(function()
        local x =           bitStream:read(FLOAT)
        local y =           bitStream:read(FLOAT)
        local z =           bitStream:read(FLOAT)
        local a =           bitStream:read(FLOAT)
        local vx =          bitStream:read(FLOAT)
        local vy =          bitStream:read(FLOAT)
        local vz =          bitStream:read(FLOAT)
        local startTime =   bitStream:read(FLOAT)
        local aname =       bitStream:read(STRING, bitStream:read(UINT8))
        local alib =        bitStream:read(STRING, bitStream:read(UINT8))
        local isAiming =    bitStream:read(BOOL)
        FriendOnFootSync(x, y, z, a, vx, vy, vz, startTime, aname, alib, isAiming)    
        -- end)
    elseif packetID == 3 then -- incarsync
        local model =       bitStream:read(UINT16)
        local color1 =      bitStream:read(UINT8)
        local color2 =      bitStream:read(UINT8)
        local px =          bitStream:read(FLOAT)
        local py =          bitStream:read(FLOAT)
        local pz =          bitStream:read(FLOAT)
        local qx =          bitStream:read(FLOAT)
        local qy =          bitStream:read(FLOAT)
        local qz =          bitStream:read(FLOAT)
        local qw =          bitStream:read(FLOAT)
        local vx =          bitStream:read(FLOAT)
        local vy =          bitStream:read(FLOAT)
        local vz =          bitStream:read(FLOAT)
        local ax =          bitStream:read(FLOAT)
        local ay =          bitStream:read(FLOAT)
        local az =          bitStream:read(FLOAT)
        local horn =        bitStream:read(BOOL)
        FriendInCarSync(model, color1, color2, px, py, pz, qx, qy, qz, qw, vx, vy, vz, ax, ay, az, horn)
    elseif packetID == 4 then -- weapon sync
        local weapid =      bitStream:read(UINT8)
        local ammo =        bitStream:read(UINT16)
        local curAmmo = getAmmoInCharWeapon(friend, weapid)
        local model = getWeapontypeModel(weapid)
        if not hasModelLoaded(model) then
            requestModel(model)
            loadAllModelsNow()    
        end
        giveWeaponToChar(friend, weapid, ammo - curAmmo)
        setCurrentCharWeapon(friend, weapid)
        markModelAsNoLongerNeeded(model)
    elseif packetID == 5 then -- aim sync
        local x =           bitStream:read(FLOAT)
        local y =           bitStream:read(FLOAT)
        local z =           bitStream:read(FLOAT)          
        taskAimGunAtCoord(friend, x, y, z, 100) 
    elseif packetID == 6 then -- shot sync
        local x =           bitStream:read(FLOAT)
        local y =           bitStream:read(FLOAT)
        local z =           bitStream:read(FLOAT)
        taskShootAtCoord(friend, x, y, z, 1000)
    elseif packetID == 7 then -- health armour sync
        friendHP =      bitStream:read(UINT8)
        friendArmour =  bitStream:read(UINT8)
    end
end
netHandle:setHook(onReceiveData)

--------------------------------------------

require "lib.moonloader"
memory = require("memory")
local font_flag = require('moonloader').font_flag
local my_font = renderCreateFont('Arial', 9, font_flag.BORDER + font_flag.BOLD)

--------------------------------------------

function main()
    while not isOpcodesAvailable() do wait(1) end

    chatTable[table.getn(chatTable) + 1] = '{ff0000}[CoopAndreas]{ffffff} Server successfuly running on port {aa0000}'..cfg.Settings.Port

    CreateFriend()
    setPedDensityMultiplier(1.0)
    setCarDensityMultiplier(1.0)
    
    lua_thread.create(function() 
        while true do
            wait(100)
            if wLastHP == 0 and wLastArmour == 0 then
                HPArmourSyncOut()    
            end
            ProcessWeaponUpdate()
            AimSyncOut()
            if isCharInAnyCar(PLAYER_PED) then
                local veh = storeCarCharIsInNoSave(PLAYER_PED)
                if getDriverOfCar(veh) == PLAYER_PED then
                    InCarSyncOut()
                else
                end
            else
                OnFootSyncOut()
            end
        end
    end)
    lua_thread.create(function() 
        while true do
            wait(350)
            NPCSendOut()
        end
    end)
    lua_thread.create(function() 
        while true do
            wait(10000)
            SyncCurrentTime()
        end
    end)
    while true 
    do
        wait(0)
        netHandle:loop()
        local str = ''
        local who = ''
        for k, v in pairs(getAllChars()) do
            if v == PLAYER_PED then who = '(me)' end
            if v == friend then who = '(ot)' end
            str = string.format('%s%d. %d%s\n', str, k, v, who)
            who = ''
        end
        renderFontDrawText(my_font, str, 500, 200, -1)
        ProccesSitAsPassenger()
        ShootSyncOut()
        HPArmourSyncOut()
        DrawFriendHPArmour()
        DrawFriendAFK()
        ProcessIncomingChat()
        ProcessChatOnOff()
    end
end

--------------------------------------------

addEventHandler("onScriptTerminate", function(script)
    if thisScript() == script then
        if doesCharExist(friend) then
            deleteChar(friend)
        end
    end
  end)

--------------------------------------------

local imgui = require 'imgui'

local encoding = require 'encoding' 
encoding.default = 'CP1251'
u8 = encoding.UTF8

local shwd = false
local input = imgui.ImBuffer(144)

--------------------------------------------

function imgui.OnDrawFrame()

    local x, y = getScreenResolution() 
    imgui.SetNextWindowSize(imgui.ImVec2(400, 75), imgui.Cond.FirstUseEver)   
    imgui.SetNextWindowPos(imgui.ImVec2(x/2 - 100, y/2 - 40), imgui.Cond.FirstUseEver)

    imgui.Begin(u8'¬вод текста') 
    imgui.InputText('<--', input)

    if not shwd then 
        imgui.SetKeyboardFocusHere() 
        shwd = true
    end

    if imgui.Button('Send') then
        if string.find(u8:decode(input.v), '^%s+$') == nil and u8:decode(input.v) ~= '' then
            if not CommandsHandler(u8:decode(input.v), true) then
                local message = "{ff0000}Host:{ffffff} "..u8:decode(input.v)
                chatTable[table.getn(chatTable) + 1] = message
                if friendIp ~= nil then
                    local bs = BitStream:new()
                    bs:write(UINT8, #message):write(STRING, message)
                    SLNetSend(netHandle, 1, bs, friendIp, friendPort, 1)
                end
            end
        end
        input.v = ''
        chatOpened = false
        imgui.Process = chatOpened  
        shwd = false 
    end
    imgui.End()
end

--------------------------------------------

function ProcessChatOnOff()
    if isKeyJustPressed(VK_F6) then
        input.v = ''
        chatOpened = not chatOpened 
        imgui.Process = chatOpened  
        shwd = false 
    end
end

--------------------------------------------

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
    renderFontDrawText(my_font, string.format("Vehicle count: %d\nPed count: %d", table.getn(getAllVehicles()), table.getn(getAllChars())), 10, 15 * 20, 0xFFFFFFFF)
end

--------------------------------------------

function CreateFriend()
    local x, y, z = getCharCoordinates(PLAYER_PED)
    y = y + 1.0
    requestModel(0)
    loadAllModelsNow()
    friend = createChar(21, 0, x, y, z - 1.1)
    setCharDecisionMaker(friend, 65545)
    setCharProofs(friend, true, true, true, true, true)
    setCharDropsWeaponsWhenDead(friend, false)
    setCharStayInSamePlace(friend, true)
    setCharCantBeDraggedOut(friend, true)
    setCharDrownsInWater(friend, false)
    setCharHealth(friend, 1000)
    friendMarker = addBlipForChar(friend)
    changeBlipColour(friendMarker, -1)
    dontRemoveChar(friend)
    setCharAccuracy(friend, 100)
    setCharCanBeKnockedOffBike(friend, false)
    markModelAsNoLongerNeeded(0)
end

--------------------------------------------

function FriendOnFootSync(x, y, z, a, vx, vy, vz, startTime, aname, alib, isAiming)
    if isCharInAnyCar(friend) then
        warpCharFromCarToCoord(friend, x, y, z)  
        local maxPassengers = getMaximumNumberOfPassengers(friendVehicle)
        if maxPassengers > 0 then
            for i=0, maxPassengers do
                if getCharInCarPassengerSeat(friendVehicle, i) == PLAYER_PED then
                    warpCharFromCarToCoord(PLAYER_PED, x, y, z)
                    break
                end
            end
        end  
        deleteCar(friendVehicle)    
    end
    setCharCoordinates(friend, x, y, z - 1.1)
    setCharHeading(friend, a)
    setCharVelocity(friend, vx, vy, vz)
    if not isAiming then
        if not hasAnimationLoaded(alib) then
            requestAnimation(alib)
        end
        taskPlayAnimNonInterruptable(friend, aname, alib, 4.1, false, true, true, false, 100)
        setCharAnimCurrentTime(friend, aname, startTime) 
    end
end

--------------------------------------------

function FriendInCarSync(model, color1, color2, px, py, pz, qx, qy, qz, qw, vx, vy, vz, ax, ay, az, horn)
    if not isCharInAnyCar(friend) then
        customPlateForNextCar(model, 'CoopAndreas')
        if not hasModelLoaded(model) then
            requestModel(model)
            loadAllModelsNow()
        end
        friendVehicle = createCar(model, px, py, pz)
        setCarProofs(friendVehicle, true, true, true, true, true)
        markModelAsNoLongerNeeded(model)
        warpCharIntoCar(friend, friendVehicle)
    end
    if not doesVehicleExist(friendVehicle) then return end
    local c1, c2 = getCarColours(friendVehicle)
    if c1 ~= color1 or c2 ~= color2 then
        changeCarColour(friendVehicle, color1, color2)
    end
    setCarCoordinatesNoOffset(friendVehicle, px, py, pz)
    setVehicleQuaternion(friendVehicle, qx, -qy, qz, qw)
    --setCarVelocity(friendVehicle, vx, vy, vz)
    setCarForwardSpeed(friendVehicle, vx)
    setCarRotationVelocity(friendVehicle, ax, ay, az)
    setCarHornState(friendVehicle, horn)
end

--------------------------------------------

function DrawFriendAFK()
    if(os.clock() - friendLastPacketTime > 2) and not isPauseMenuActive() then
        if doesCharExist(friend) then
            if isCharOnScreen(friend) then
                local hX, hY, hZ = getCharCoordinates(friend)
                local pX, pY, pZ = getCharCoordinates(PLAYER_PED)
                if(math.sqrt((hX - pX)^2 + (hY - pY)^2 + (hZ - pZ)^2) < 30.0) then 
                    local sx, sy = convert3DCoordsToScreen(hX, hY, hZ)
                    renderFontDrawText(my_font, string.format('{ff0000}AFK: {999999}%d sec', os.clock() - friendLastPacketTime), sx, sy + 1.0, -1)    
                end
            end
        end
    end
end

--------------------------------------------

function OnFootSyncOut() 
    if friendIp == nil then return end
    local px, py, pz = getCharCoordinates       (PLAYER_PED)
    local a =          getCharHeading           (PLAYER_PED)
    local vx, vy, vz = getCharVelocity          (PLAYER_PED)
    local anim =       getCharCurrentAnimation  (PLAYER_PED)
    local aname = anim[2]
    local alib = anim[1]
    local starttime =  getCharAnimCurrentTime   (PLAYER_PED, aname)
    local isAiming =   isCharAiming             (PLAYER_PED)
    local bs = BitStream:new() 
    bs:write(FLOAT, px):write(FLOAT, py):write(FLOAT, pz)
    bs:write(FLOAT, a)
    bs:write(FLOAT, vx):write(FLOAT, vy):write(FLOAT, vz)
    bs:write(FLOAT, starttime):write(UINT8, #aname):write(STRING, aname):write(UINT8, #alib):write(STRING, alib)
    bs:write(BOOL, isAiming)

    SLNetSend(netHandle, 2, bs, friendIp, friendPort, 1)
end

--------------------------------------------

function InCarSyncOut()
    if not isCharInAnyCar(PLAYER_PED) then return end
    local veh = storeCarCharIsInNoSave(PLAYER_PED)

    local model =           getCarModel             (veh)
    local color1, color2 =  getCarColours           (veh)
    local px, py, pz =      getCarCoordinates       (veh)
    local qx, qy, qz, qw =  getVehicleQuaternion    (veh)
    --local vx, vy, vz =      getCarVelocity          (veh)
    local vy, vz =      0, 0
    local vx = getCarSpeed(veh)
    local ax, ay, az =      getCarQuatVelocity      (veh)
    local horn =            isPlayerPressingHorn    (PLAYER_HANDLE)
    local bs = BitStream:new() 
    bs:write(UINT16, model)
    bs:write(UINT8, color1):write(UINT8, color2)
    bs:write(FLOAT, px):write(FLOAT, py):write(FLOAT, pz)
    bs:write(FLOAT, qx):write(FLOAT, qy):write(FLOAT, qz):write(FLOAT, qw)
    bs:write(FLOAT, vx):write(FLOAT, vy):write(FLOAT, vz)
    bs:write(FLOAT, ax):write(FLOAT, ay):write(FLOAT, az)
    bs:write(BOOL, horn)    
    SLNetSend(netHandle, 3, bs, friendIp, friendPort, 1)     
end

--------------------------------------------

local wLastWeapon = 0
local wLastAmmo = 0

function ProcessWeaponUpdate()
    local cLastWeapon = getCurrentCharWeapon    (PLAYER_PED)
    local cLastAmmo =   getAmmoInCharWeapon     (PLAYER_PED, cLastWeapon)
    if cLastAmmo ~= wLastAmmo or cLastWeapon ~= wLastWeapon then
        local bs = BitStream:new()
        bs:write(UINT8, cLastWeapon):write(UINT16, cLastAmmo)
        SLNetSend(netHandle, 4, bs, friendIp, friendPort, 1)
    end
    wLastWeapon = cLastWeapon
    wLastAmmo = cLastAmmo
end

--------------------------------------------

function AimSyncOut()
    if isCharAiming(PLAYER_PED) then
        local x, y, z = getPlayerAimCoords()
        local bs = BitStream:new() 
        bs:write(FLOAT, x):write(FLOAT, y):write(FLOAT, z)   
        SLNetSend(netHandle, 5, bs, friendIp, friendPort, 1)
    end
end

--------------------------------------------

function ShootSyncOut()
    if isCharShooting(PLAYER_PED) then
        local x, y, z = getPlayerAimCoords()
        if x == 0 and y == 0 and z == 0 then return end
        ProcessWeaponUpdate()
        local bs = BitStream:new() 
        bs:write(FLOAT, x):write(FLOAT, y):write(FLOAT, z)   
        SLNetSend(netHandle, 6, bs, friendIp, friendPort, 1)
    end
end

--------------------------------------------

function HPArmourSyncOut()
    local cLastHP = getCharHealth(PLAYER_PED)
    local cLastArmour = getCharArmour(PLAYER_PED)
    if cLastArmour ~= wLastArmour or cLastHP ~= wLastHP then
        local bs = BitStream:new()
        bs:write(UINT8, cLastHP):write(UINT8, cLastArmour)
        SLNetSend(netHandle, 7, bs, friendIp, friendPort, 1)
    end
    wLastHP = cLastHP
    wLastArmour = cLastArmour
end

--------------------------------------------

function DrawFriendHPArmour()
    if doesCharExist(friend) and not isPauseMenuActive() then
        if isCharOnScreen(friend) then
            local hX, hY, hZ = getCharCoordinates(friend)
            local pX, pY, pZ = getCharCoordinates(PLAYER_PED)
            if(math.sqrt((hX - pX)^2 + (hY - pY)^2 + (hZ - pZ)^2) < 10.0) then 
                local sx, sy = convert3DCoordsToScreen(hX, hY, hZ + 0.9)
                renderDrawBoxWithBorder (sx - 50, sy, 100.0, 12.0, 0xFF5a0c0e, 3.3, 0xFF000000)
                renderDrawBox           (sx - 50 + 3.3, sy + 3.3, friendHP - 6.6, 12.0 - 6.6, 0xFFb4191d, 3.3, 0xFF000000)
                if friendArmour > 0 then
                    renderDrawBoxWithBorder (sx - 50, sy - 20.0, 100.0, 12.0, 0xFF707070, 3.3, 0xFF000000)
                    renderDrawBox           (sx - 50 + 3.3, sy + 3.3  - 20.0, friendArmour - 6.6, 12.0 - 6.6, 0xFFe1e1e1, 3.3, 0xFF000000)
                end
            end
        end
    end
end

--------------------------------------------

function ProccesSitAsPassenger()
    if isKeyJustPressed(VK_G) then
        if doesVehicleExist(friendVehicle) then
            local x, y, z = getCharCoordinates(PLAYER_PED)
            local vx, vy, vz = getCarCoordinates(friendVehicle)
            if math.sqrt((x - vx)^2 + (y - vy)^2 + (z - vz)^2) < 10.0 then
                local maxPassengers = getMaximumNumberOfPassengers(friendVehicle)
                if maxPassengers > 0 then
                    for i=0, maxPassengers do
                        if isCarPassengerSeatFree(friendVehicle, i) then
                            taskEnterCarAsPassenger(PLAYER_PED, friendVehicle, 1000, i)
                            return
                        end
                    end
                end
            end
        end
    end
end

--------------------------------------------

function SyncCurrentTime()
    local hours, mins = getTimeOfDay()
    local day = getCurrentDayOfWeek()
    local bs = BitStream:new()
    bs:write(UINT8, hours):write(UINT8, mins)
    bs:write(UINT8, day)
    SLNetSend(netHandle, 8, bs, friendIp, friendPort, 1)
end
