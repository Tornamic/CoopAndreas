local peds = {}
local pedsVehicle = {}
local pedsHP = {}
local pedsLastPacket = {}

special_skins={
    [3]='ANDRE',
    [4]='BBTHIN',
    [5]='BB',
    [298]='CAT',
    [292]='CESAR',
    [190]='COPGRL3',
    [299]='CLAUDE',
    [194]='CROGRL3',
    [268]='DWAYNE',
    [6]='EMMET',
    [272]='FORELLI',
    [195]='GANGRL3',
    [191]='GUNGRL3',
    [267]='HERN',
    [8]='JANITOR',
    [42]='JETHRO',
    [296]='JIZZY',
    [65]='KENDL',
    [2]='MACCER',
    [297]='MADDOGG',
    [192]='MECGRL3',
    [193]='NURGRL2',
    [293]='OGLOC',
    [291]='PAUL',
    [266]='PULASKI',
    [290]='ROSE',
    [271]='RYDER',
    [86]='RYDER3',
    [119]='SINDACO',
    [269]='SMOKE',
    [149]='SMOKEV',
    [208]='SUZIE',
    [270]='SWEET',
    [273]='TBONE',
    [265]='TENPEN',
    [295]='TORINO',
    [1]='TRUTH',
    [294]='WUZIMU',
    [289]='ZERO',
}

for plp=1, 250 do
    pedsLastPacket[plp] = -1
end
function NPCOnReceive(packetID, bs)
    local pedCount =            bs:read(UINT8)
    local isAiming = false
    for i=1,pedCount do
        local id =              bs:read(UINT8)
        pedsLastPacket[id] = os.clock()
        local inVeh =           bs:read(BOOL)
        local pedtype =         bs:read(UINT8)
        local skin =            bs:read(UINT16)
        if not inVeh then
            local px =          bs:read(FLOAT)
            local py =          bs:read(FLOAT)
            local pz =          bs:read(FLOAT)
            local a =           bs:read(FLOAT)
            local vx =          bs:read(FLOAT)
            local vy =          bs:read(FLOAT)
            local vz =          bs:read(FLOAT)
            local starttime =   bs:read(FLOAT)
            local aname =       bs:read(STRING, bs:read(UINT8))
            local alib =        bs:read(STRING, bs:read(UINT8))
            local isAiming =    bs:read(BOOL)
            NPCOnFootSync(id, pedtype, skin, px, py, pz, a, vx, vy, vz, starttime, aname, alib, isAiming)
        else
            local model =       bs:read(UINT16)
            local color1 =      bs:read(UINT8)
            local color2 =      bs:read(UINT8)
            local px =          bs:read(FLOAT)
            local py =          bs:read(FLOAT)
            local pz =          bs:read(FLOAT)
            local qx =          bs:read(FLOAT)
            local qy =          bs:read(FLOAT)
            local qz =          bs:read(FLOAT)
            local qw =          bs:read(FLOAT)
            local vx =          bs:read(FLOAT)
            local vy =          bs:read(FLOAT)
            local vz =          bs:read(FLOAT)
            local ax =          bs:read(FLOAT)
            local ay =          bs:read(FLOAT)
            local az =          bs:read(FLOAT)
            NPCInCarSync(id, pedtype, skin, model, color1, color2, px, py, pz, qx, qy, qz, qw, vx, vy, vz, ax, ay, az)
        end
        local weapid =          bs:read(UINT8)
        local ammo =            bs:read(UINT16)
        local aimx =            bs:read(FLOAT)
        local aimy =            bs:read(FLOAT)
        local aimz =            bs:read(FLOAT)
        local hp =              bs:read(UINT8)
        NPCWeaponSync(id, weapid, ammo)
        if aiming then
            taskAimGunAtCoord(peds[id], aimx, aimy, aimz, 1000)
        end
    end
end

--------------------------------------------

function NPCOnFootSync(id, pedtype, skin, x, y, z, a, vx, vy, vz, startTime, aname, alib, isAiming, blockvehremove)
    if blockvehremove == nil then blockvehremove = false end
    local is_special = false
    if special_skins[skin] then
        is_special = true
        --return
    -- else
    --     return
    end
    print(string.format('skinid %d', skin))
    if not doesCharExist(peds[id]) then

        if is_special then 
            loadSpecialCharacter(special_skins[skin], 1) 
            while not hasSpecialCharacterLoaded(1) do
                wait(0)
            end
        else requestModel(skin); loadAllModelsNow() end

        peds[id] = createChar(21, is_special and 290 or skin, x, y, z - 1.1)
        setCharProofs(peds[id], true, true, true, true, true)
        setCharStayInSamePlace(peds[id], true)
        setCharCantBeDraggedOut(peds[id], true)
        setCharDrownsInWater(peds[id], false)
        setCharHealth(peds[id], 100)
        dontRemoveChar(peds[id])
        setCharAccuracy(peds[id], 100)

        if is_special then unloadSpecialCharacter(1)
        else markModelAsNoLongerNeeded(skin) end
    end
    if getCharModel(peds[id]) ~= skin and not is_special then
        setCharModel(peds[id], skin)    
    end
    if isCharInAnyCar(peds[id]) then
        local car = storeCarCharIsInNoSave(peds[id])
        warpCharFromCarToCoord(peds[id], x, y, z)
        deleteCar(car)
    else
        if doesVehicleExist(pedsVehicle[id]) and not blockvehremove then -- при проблемах закоментировать
            deleteCar(pedsVehicle[id])    
        end
    end
    setCharCoordinates(peds[id], x, y, z - 1.1)
    setCharHeading(peds[id], a)
    setCharVelocity(peds[id], vx, vy, vz)
    if not isAiming and not isPauseMenuActive() then
        -- if not hasAnimationLoaded(alib) then
        --     requestAnimation(alib)
        -- end
        taskPlayAnimNonInterruptable(peds[id], aname, alib, 4.1, false, true, true, false, 333)
        setCharAnimCurrentTime(peds[id], aname, startTime) 
    end
end

--------------------------------------------

function NPCInCarSync(id, pedtype, skin, model, color1, color2, px, py, pz, qx, qy, qz, qw, vx, vy, vz, ax, ay, az)
    if not doesCharExist(peds[id]) then
        NPCOnFootSync(id, pedtype, skin, px, py, pz, 0, 0, 0, 0, 0, 'null', 'PED', false, true)
        return    
    end
    if not isCharInAnyCar(peds[id]) and not doesVehicleExist(pedsVehicle[id]) then
        customPlateForNextCar(model, 'CoopAndreas')
        if not hasModelLoaded(model) then
            requestModel(model)
            loadAllModelsNow()
        end
        pedsVehicle[id] = createCar(model, px, py, pz)
        setCarProofs(pedsVehicle[id], true, true, true, true, true)
        markModelAsNoLongerNeeded(model)
        warpCharIntoCar(peds[id], pedsVehicle[id])
    end
    if doesVehicleExist(pedsVehicle[id]) then
        if model ~= getCarModel(pedsVehicle[id]) then
            NPCOnFootSync(id, pedtype, skin, px, py, pz, 0, 0, 0, 0, 0, 'null', 'PED', false, true)
            NPCInCarSync(id, pedtype, skin, model, color1, color2, px, py, pz, qx, qy, qz, qw, vx, vy, vz, ax, ay, az)
        end
    end
    local is_special = false
    if special_skins[skin] then
        is_special = true
    end
    if getCharModel(peds[id]) ~= skin and not is_special then
        setCharModel(peds[id], skin)    
    end
    if not doesVehicleExist(pedsVehicle[id]) then return end
    local c1, c2 = getCarColours(pedsVehicle[id])
    if c1 ~= color1 or c2 ~= color2 then
        changeCarColour(pedsVehicle[id], color1, color2)
    end
    setCarCoordinates(pedsVehicle[id], px, py, pz)
    setVehicleQuaternion(pedsVehicle[id], qx, -qy, qz, qw)
    setCarVelocity(pedsVehicle[id], vx, vy, vz)
    setCarRotationVelocity(pedsVehicle[id], ax, ay, az)
end

--------------------------------------------

function NPCWeaponSync(id, weapid, ammo)
    if not doesCharExist(peds[id]) then return end
    local curAmmo = getAmmoInCharWeapon(peds[id], weapid)
    local model = getWeapontypeModel(weapid)
    if not hasModelLoaded(model) then
        requestModel(model)
        loadAllModelsNow()    
    end
    giveWeaponToChar(peds[id], weapid, ammo - curAmmo)
    setCurrentCharWeapon(peds[id], weapid)
    markModelAsNoLongerNeeded(model)
end

--------------------------------------------

function NPCDeleteIfUnavaibleAll()
    for i=1, 250 do
        if peds[i] ~= nil and doesCharExist(peds[i]) and peds[i] ~= PLAYER_PED and peds[i] ~= friend then
            if os.clock() - pedsLastPacket[i] >= 1 then
                pedsLastPacket[i] = -1
                deleteChar(peds[i])         
            end
        end
    end
    for _, v in pairs(pedsVehicle) do
        if not doesVehicleOccupied(v) then
            deleteCar(v)
        end
    end
end
