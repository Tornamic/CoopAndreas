function NPCSendOut()
    local peds = getAllChars()
    local bs = BitStream:new()
    bs:write(UINT8, table.getn(peds) - 2)
    local success = false
    for i=0, table.getn(peds) do
        if doesCharExist(peds[i]) and peds[i] ~= PLAYER_PED and peds[i] ~= friend then
            bs = NPCSyncOut(bs, peds[i], i)
            success = true
        end
    end
    if success then
        SLNetSend(netHandle, 9, bs, friendIp, friendPort, 1)
    end
end

--------------------------------------------

function NPCSyncOut(bs, ped, pedid)     --------- все одним пакетом нахуй       (кроме выстрелов)
    
    local veh = nil
    local inVeh = false

    if isCharInAnyCar(ped) then
        veh = storeCarCharIsInNoSave(ped)
        if getDriverOfCar(veh) == ped then 
            inVeh = true 
        end    
    end
    
    local pedtype =             getPedType               (ped)
    local skin =                getCharModel             (ped)     
    
    bs:write(UINT8, pedid) 
    bs:write(BOOL, inVeh)
    bs:write(UINT8, pedtype):write(UINT16, skin)

    if not inVeh then
        local px, py, pz =      getCharCoordinates       (ped)
        local a =               getCharHeading           (ped)
        local vx, vy, vz =      getCharVelocity          (ped)
        local anim =            getCharCurrentAnimation  (ped)
        local aname = anim[2]
        local alib = anim[1]
        local starttime =       getCharAnimCurrentTime   (ped, aname)
        local isAiming =        isCharAiming             (ped)
        bs:write(FLOAT, px):write(FLOAT, py):write(FLOAT, pz)
        bs:write(FLOAT, a)
        bs:write(FLOAT, vx):write(FLOAT, vy):write(FLOAT, vz)
        bs:write(FLOAT, starttime):write(UINT8, #aname):write(STRING, aname):write(UINT8, #alib):write(STRING, alib)
        bs:write(BOOL, isAiming)
    else
        local model =           getCarModel             (veh)
        local color1, color2 =  getCarColours           (veh)
        local px, py, pz =      getCarCoordinates       (veh)
        local qx, qy, qz, qw =  getVehicleQuaternion    (veh)
        local vx, vy, vz =      getCarVelocity          (veh)
        local ax, ay, az =      getCarQuatVelocity      (veh)
        bs:write(UINT16, model)
        bs:write(UINT8, color1):write(UINT8, color2)
        bs:write(FLOAT, px):write(FLOAT, py):write(FLOAT, pz)
        bs:write(FLOAT, qx):write(FLOAT, qy):write(FLOAT, qz):write(FLOAT, qw)
        bs:write(FLOAT, vx):write(FLOAT, vy):write(FLOAT, vz)
        bs:write(FLOAT, ax):write(FLOAT, ay):write(FLOAT, az)
    end
    local weap =                getCurrentCharWeapon    (ped)
    local ammo =                getAmmoInCharWeapon     (ped, weap)
    local aimx, aimy, aimz =    getCharAimCoords        (ped)
    local hp =                  getCharHealth           (ped)
    bs:write(UINT8, weap)
    bs:write(UINT16, ammo)
    bs:write(FLOAT, aimx):write(FLOAT, aimy):write(FLOAT, aimz)
    bs:write(UINT8, hp)

    return bs
end
