local Matrix3X3 = require "matrix3x3"
local Vector3D = require "vector3d"   

--------------------------------------------

function isCharAiming(ped)
    return memory.getint8(getCharPointer(ped) + 0x528, false) == 19
end

--------------------------------------------
function getPlayerAimCoords()
    local vec_out = ffi.new("float[3]")
    local tmp_vec = ffi.new("float[3]")
    ffi.cast(
        "void (__thiscall*)(void*, float, float, float, float, float*, float*)",
        0x514970
    )(
        ffi.cast("void*", 0xB6F028),
        15.0,
        tmp_vec[0], tmp_vec[1], tmp_vec[2],
        tmp_vec,
        vec_out
    )
    return vec_out[0], vec_out[1], vec_out[2]
end
function getCharAimCoords(ped)
    if isCharAiming(ped) then 
        local sx, sy = getCursorPos()
        local sw, sh = getScreenResolution()
        if sx >= 0 and sy >= 0 and sx < sw and sy < sh then
            local posX, posY, posZ = convertScreenCoordsToWorld3D(sx + 100.0, sy - 120.0, 700.0)
            local camX, camY, camZ = getActiveCameraCoordinates()
            local result, colpoint = processLineOfSight(camX, camY, camZ, posX, posY, posZ, true, true, false, true, false, false, false)
            if result and colpoint.entity ~= 0 then
                local normal = colpoint.normal
                local pos = Vector3D(colpoint.pos[1], colpoint.pos[2], colpoint.pos[3]) - (Vector3D(normal[1], normal[2], normal[3]) * 0.1)
                local zOffset = 300
                if normal[3] >= 0.5 then zOffset = 1  end
                local result, colpoint2 = processLineOfSight(pos.x, pos.y, pos.z + zOffset, pos.x, pos.y, pos.z - 0.3, true, true, false, true, false, false, false)
                if result then
                    return pos.x, pos.y, pos.z
                end
            end
        end
    end
    return 0.0, 0.0, 0.0
end

--------------------------------------------

function setCurrentWeekday(day)
    memory.setint8(0xB7014E, day, true)
end

--------------------------------------------

local CPed_SetModelIndex = ffi.cast('void(__thiscall *)(void*, unsigned int)', 0x5E4880)
function setCharModel(ped, model)
    assert(doesCharExist(ped), 'ped not found')
    if not hasModelLoaded(model) then
        requestModel(model)
        loadAllModelsNow()
    end
    CPed_SetModelIndex(ffi.cast('void*', getCharPointer(ped)), ffi.cast('unsigned int', model))
end

--------------------------------------------

