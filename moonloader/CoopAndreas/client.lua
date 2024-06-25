script_name('coopandreas_client')
script_author('Tornamic')
script_properties("work-in-pause")
--------------------------------------------

package.path = package.path .. ';../?.lua;../?.luac;../../?.lua;../../?.luac;../?/?.lua'
package.cpath = package.cpath .. ';../?.dll;../../?.dll'

--------------------------------------------

require 'CoopAndreas.patch'
require 'CoopAndreas.npc_sync_in'

--------------------------------------------

ffi = require 'ffi'
local inicfg = require 'inicfg' -- cfg loading
local cfg = inicfg.load(nil, "moonloader\\CoopAndreas.cfg")

--------------------------------------------

require 'SLNet'

--------------------------------------------

require 'CoopAndreas.anims'
require 'CoopAndreas.commands_handler'
require 'CoopAndreas.car_functions'
require 'CoopAndreas.char_functions'

--------------------------------------------

netHandle = SLNetInit() 

--------------------------------------------

netHandle:connect(cfg.Settings.Ip, tonumber(cfg.Settings.Port)) -- connect to host 
netHandle:setPrefix('CPAND') 

--------------------------------------------

local chatOpened = false
local chatTable = {}
host = nil
hostIp = ''
hostPort = ''
local hostMarker = nil
local hostLastPacketTime = 0
local hostVehicle = nil
local hostHP = 0
local hostArmour = 0

--------------------------------------------

function onReceiveData(packetID, bitStream)
    hostLastPacketTime = os.clock()
    if packetID == 1 then
        local message =     bitStream:read(STRING, bitStream:read(UINT8))
        chatTable[table.getn(chatTable) + 1] = message

    elseif packetID == 2 then -- foot sync
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
        HostOnFootSync(x, y, z, a, vx, vy, vz, startTime, aname, alib, isAiming)    
    elseif packetID == 3 then -- incar sync
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
        HostInCarSync(model, color1, color2, px, py, pz, qx, qy, qz, qw, vx, vy, vz, ax, ay, az, horn)
    elseif packetID == 4 then -- weapon sync
        local weapid =      bitStream:read(UINT8)
        local ammo =        bitStream:read(UINT16)
        local curAmmo = getAmmoInCharWeapon(host, weapid)
        local model = getWeapontypeModel(weapid)
        if not hasModelLoaded(model) then
            requestModel(model)
            loadAllModelsNow()    
        end
        giveWeaponToChar(host, weapid, ammo - curAmmo)
        setCurrentCharWeapon(host, weapid)
        markModelAsNoLongerNeeded(model)
    elseif packetID == 5 then -- aim sync
        local x =           bitStream:read(FLOAT)
        local y =           bitStream:read(FLOAT)
        local z =           bitStream:read(FLOAT) 
        taskAimGunAtCoord(host, x, y, z, 1000) 
    elseif packetID == 6 then -- shot sync
        local x =           bitStream:read(FLOAT)
        local y =           bitStream:read(FLOAT)
        local z =           bitStream:read(FLOAT)
        taskShootAtCoord(host, x, y, z, 1000)
    elseif packetID == 7 then -- health armour sync
        hostHP =            bitStream:read(UINT8)
        hostArmour =        bitStream:read(UINT8)
    elseif packetID == 8 then -- time date sync
        local hours =       bitStream:read(UINT8)
        local mins =        bitStream:read(UINT8)
        local day =         bitStream:read(UINT8)
        setTimeOfDay(hours, mins)
        setCurrentWeekday(day)
    elseif packetID == 9 then
        NPCOnReceive(packetID, bitStream)
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

    PatchClient()
    
    chatTable[table.getn(chatTable) + 1] = '{ff0000}[CoopAndreas]{ffffff} Client successfully started!'
    
    CreateHost()
    lua_thread.create(function() 
        while true do
            wait(100)
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
            wait(600)
            NPCDeleteIfUnavaibleAll()    
        end
    end)
    while true 
    do
        wait(0)
        netHandle:loop()
        ProccesSitAsPassenger()
        ShootSyncOut()
        HPArmourSyncOut()
        DrawHostHPArmour()
        DrawHostAFK()
        ProcessIncomingChat()
        ProcessChatOnOff()
        if isKeyJustPressed(VK_F5) then
            setCharCoordinates(PLAYER_PED, getCharCoordinates(host))    
        end
        renderFontDrawText(my_font, string.format("Vehicle count: %d\nPed count: %d", table.getn(getAllVehicles()), table.getn(getAllChars())), 10, 15 * 20, 0xFFFFFFFF)
    end
end
addEventHandler("onScriptTerminate", function(script)
    if thisScript() == script then
        local vehs = getAllVehicles()
        for i=1, table.getn(vehs) do
            if doesVehicleExist(vehs[i]) then
                deleteCar(vehs[i])    
            end
        end
        local chars = getAllChars()
        for i=1, table.getn(chars) do
            if doesCharExist(chars[i]) and chars[i] ~= PLAYER_PED then
                deleteChar(chars[i])    
            end
        end
        if doesCharExist(host) then
            deleteChar(host)
        end
    end
  end)

--------------------------------------------

local imgui = require 'imgui'

--------------------------------------------
local encoding = require 'encoding' 
encoding.default = 'CP1251'
u8 = encoding.UTF8

--------------------------------------------

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
            if not CommandsHandler(u8:decode(input.v), false) then
                local bs = BitStream:new()
                bs:write(UINT8, #u8:decode(input.v)):write(STRING, u8:decode(input.v))
                SLNetSend(netHandle, 1, bs, 1)
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
end

--------------------------------------------

function CreateHost()
    local x, y, z = getCharCoordinates(PLAYER_PED)
    y = y + 1.0
    requestModel(0)
    loadAllModelsNow()
    host = createChar(21, 0, x, y, z - 1.1)
    setCharDecisionMaker(host, 65545)
    setCharProofs(host, true, true, true, true, true)
    setCharDropsWeaponsWhenDead(host, false)
    setCharStayInSamePlace(host, true)
    setCharCantBeDraggedOut(host, true)
    setCharDrownsInWater(host, false)
    setCharHealth(host, 1000)
    hostMarker = addBlipForChar(host)
    changeBlipColour(hostMarker, -1)
    dontRemoveChar(host)
    setCharAccuracy(host, 100)
    setCharCanBeKnockedOffBike(host, false)
    markModelAsNoLongerNeeded(0)
end

--------------------------------------------

function HostOnFootSync(x, y, z, a, vx, vy, vz, startTime, aname, alib, isAiming)
    if isCharInAnyCar(host) and doesVehicleExist(hostVehicle) then
        warpCharFromCarToCoord(host, x, y, z)    
        local maxPassengers = getMaximumNumberOfPassengers(hostVehicle)
        if maxPassengers > 0 then
            for i=1, maxPassengers do
                if getCharInCarPassengerSeat(hostVehicle, i) == PLAYER_PED then
                    warpCharFromCarToCoord(PLAYER_PED, x, y, z)
                    break
                end
            end
        end
        deleteCar(hostVehicle)    
    end
    setCharCoordinates(host, x, y, z - 1.1)
    setCharHeading(host, a)
    setCharVelocity(host, vx, vy, vz)
    if not isAiming and not isPauseMenuActive() then
        if not hasAnimationLoaded(alib) then
            requestAnimation(alib)
        end
        taskPlayAnimNonInterruptable(host, aname, alib, 4.1, false, true, true, false, 100)
        setCharAnimCurrentTime(host, aname, startTime) 
    end
end

--------------------------------------------

function HostInCarSync(model, color1, color2, px, py, pz, qx, qy, qz, qw, vx, vy, vz, ax, ay, az, horn)
    if not isCharInAnyCar(host) then
        customPlateForNextCar(model, 'CoopAndreas')
        if not hasModelLoaded(model) then
            requestModel(model)
            loadAllModelsNow()
        end
        hostVehicle = createCar(model, px, py, pz)
        setCarProofs(hostVehicle, true, true, true, true, true)
        markModelAsNoLongerNeeded(model)
        warpCharIntoCar(host, hostVehicle)
    end
    if not doesVehicleExist(hostVehicle) then return end
    local c1, c2 = getCarColours(hostVehicle)
    if c1 ~= color1 or c2 ~= color2 then
        changeCarColour(hostVehicle, color1, color2)
    end
    setCarCoordinatesNoOffset(hostVehicle, px, py, pz)
    setVehicleQuaternion(hostVehicle, qx, -qy, qz, qw)
    --setCarVelocity(hostVehicle, vx, vy, vz)
    setCarForwardSpeed(hostVehicle, vx)
    setCarRotationVelocity(hostVehicle, ax, ay, az)
    setCarHornState(hostVehicle, horn)
end

--------------------------------------------

function DrawHostAFK()
    if(os.clock() - hostLastPacketTime > 2) and not isPauseMenuActive() then
        if doesCharExist(host) then
            if isCharOnScreen(host) then
                local hX, hY, hZ = getCharCoordinates(host)
                local pX, pY, pZ = getCharCoordinates(PLAYER_PED)
                if(math.sqrt((hX - pX)^2 + (hY - pY)^2 + (hZ - pZ)^2) < 30.0) then 
                    local sx, sy = convert3DCoordsToScreen(hX, hY, hZ + 1)
                    renderFontDrawText(my_font, string.format('{ff0000}AFK: {999999}%d sec', os.clock() - hostLastPacketTime), sx, sy + 1.0, -1)    
                end
            end
        end
    end
end

--------------------------------------------

function OnFootSyncOut() 

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
    SLNetSend(netHandle, 2, bs, 1)  
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
    local vx            =   getCarSpeed             (veh)
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
    SLNetSend(netHandle, 3, bs, 1)     
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
        SLNetSend(netHandle, 4, bs, 1)
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
        SLNetSend(netHandle, 5, bs, 1)
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
        SLNetSend(netHandle, 6, bs, 1)
    end
end

--------------------------------------------

local wLastHP = 0
local wLastArmour = 0

function HPArmourSyncOut()
    local cLastHP = getCharHealth(PLAYER_PED)
    local cLastArmour = getCharArmour(PLAYER_PED)
    if cLastArmour ~= wLastArmour or cLastHP ~= wLastHP then
        local bs = BitStream:new()
        bs:write(UINT8, cLastHP):write(UINT8, cLastArmour)
        SLNetSend(netHandle, 7, bs, 1)
    end
    wLastHP = cLastHP
    wLastArmour = cLastArmour
end

--------------------------------------------

function DrawHostHPArmour()
    if doesCharExist(host) and not isPauseMenuActive() then
        if isCharOnScreen(host) then
            local hX, hY, hZ = getCharCoordinates(host)
            local pX, pY, pZ = getCharCoordinates(PLAYER_PED)
            if(math.sqrt((hX - pX)^2 + (hY - pY)^2 + (hZ - pZ)^2) < 10.0) then 
                local sx, sy = convert3DCoordsToScreen(hX, hY, hZ + 0.9)
                renderDrawBoxWithBorder (sx - 50, sy, 100.0, 12.0, 0xFF5a0c0e, 3.3, 0xFF000000)
                renderDrawBox           (sx - 50 + 3.3, sy + 3.3, hostHP - 6.6, 12.0 - 6.6, 0xFFb4191d, 3.3, 0xFF000000)
                if hostArmour > 0 then
                    renderDrawBoxWithBorder (sx - 50, sy - 20.0, 100.0, 12.0, 0xFF707070, 3.3, 0xFF000000)
                    renderDrawBox           (sx - 50 + 3.3, sy + 3.3  - 20.0, hostArmour - 6.6, 12.0 - 6.6, 0xFFe1e1e1, 3.3, 0xFF000000)
                end
            end
        end
    end
end

--------------------------------------------

function ProccesSitAsPassenger()
    if isKeyJustPressed(VK_G) then
        if doesVehicleExist(hostVehicle) then
            local x, y, z = getCharCoordinates(PLAYER_PED)
            local vx, vy, vz = getCarCoordinates(hostVehicle)
            if math.sqrt((x - vx)^2 + (y - vy)^2 + (z - vz)^2) < 10.0 then
                local maxPassengers = getMaximumNumberOfPassengers(hostVehicle)
                if maxPassengers > 0 then
                    for i=0, maxPassengers do
                        if isCarPassengerSeatFree(hostVehicle, i) then
                            taskEnterCarAsPassenger(PLAYER_PED, hostVehicle, 1000, i)
                            return
                        end
                    end
                end
            end
        end
    end
end

--------------------------------------------
