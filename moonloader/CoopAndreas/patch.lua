local ffi = require 'ffi'
ffi.cdef[[
    void *malloc(size_t size);
    void free(void *ptr);
]]

function PatchClient()
    -- Disable Wanted Level by Game
    memory.write(0x58DB5F, 0xBD, 1, true)
    memory.fill(0x58DB60, 0x00, 4, true)
    memory.fill(0x58DB64, 0x90, 4, true)

    memory.write(0x72C1B7, 0xEB, 1, true) -- Disable Haze Effect
    
    memory.fill(0x748063, 0x90, 5, true) -- Disable Pause Game
    memory.write(0x47C477, 0xEB, 1, true) -- Disable Camera Movement
    memory.fill(0x704E8A, 0x90, 5, true) -- Disable Motion Blur
    memory.fill(0x609A4E, 0x90, 6, true) -- Lock Player Animation
    memory.write(0x86D1EC, 0x0, 1, true) -- Disable Idle Animation
    memory.fill(0x53C090, 0x90, 5, true) -- Disable Replays
    memory.fill(0x440833, 0x90, 8, true) -- Disable Interior Peds
    memory.write(0x588BE0, 0xC3, 1, true) -- Disable Printing Messages
    memory.fill(0x58FBE9, 0x90, 5, true) -- Disable Vehicles Names
    setPedDensityMultiplier(0.0) -- Disable Peds on the street
    setCarDensityMultiplier(0.0) -- Disable Cars on the street
    setMaxWantedLevel(0) -- Disable Wanted Level
    setOnlyCreateGangMembers(true) -- Disable Gang Wars
    setCreateRandomGangMembers(false) -- Disable Gang Members
    enableBurglaryHouses(false) -- Disable Houses
    switchRandomTrains(false) -- Disable Trains
    setCreateRandomCops(false) -- Disable Police
    switchEmergencyServices(false) -- Disable EMS
    switchAmbientPlanes(false) -- Disable Planes
    memory.write(0x9690A0, 0, 4, true) -- Disable Car Generator #1
    -- Disable Car Generator #2
    memory.fill(0x53C1C1, 0x90, 5, true)
    memory.fill(0x434272, 0x90, 5, true)
    --memory.fill(0x561AF0, 0x90, 7, true) -- antipause
    
    -- Disable spawn 537 Train
    memory.setuint8(0x8D477C, 0x00, true)
    memory.setuint8(0x8D477D, 0x00, true)
  
    -- Disable spawn with cigars and bottles
    memory.fill(0x4217F4, 0x90, 21, true)
    memory.fill(0x4218D8, 0x90, 17, true)
    memory.fill(0x5F80C0, 0x90, 10, true)
    memory.fill(0x5FBA47, 0x90, 10, true)
  
    -- patch resolution
    memory.fill(0x745BC9, 0x90, 2, true)

    memory.setuint8(7634870, 1)
    memory.setuint8(7635034, 1)
    memory.fill(7623723, 144, 8)
    memory.fill(5499528, 144, 6)
end

function PatchClient2()
    local dir_full_path = "moonloader\\CoopAndreas\0"
    local dir = ffi.C.malloc(#dir_full_path)
    ffi.copy(dir, dir_full_path)
    local name = ffi.C.malloc(10)
    ffi.copy(name, "patch.scm\0")
    memory.setuint32(0x468EB5 + 1, tonumber(ffi.cast('uintptr_t', dir)), true)
    memory.setuint32(0x468EC4 + 1, tonumber(ffi.cast('uintptr_t', name)), true)
end