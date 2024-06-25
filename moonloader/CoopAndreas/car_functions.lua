function getCarQuatVelocity(vehicle)
	local x = memory.getfloat(getCarPointer(vehicle)+80,true)
	local y = memory.getfloat(getCarPointer(vehicle)+84,true)
	local z = memory.getfloat(getCarPointer(vehicle)+88,true)
	return x,y,z
end

--------------------------------------------

function setCarQuatVelocity(vehicle, x, y, z)
	memory.setfloat(getCarPointer(vehicle)+80, x, true)
	memory.setfloat(getCarPointer(vehicle)+84, y, true)
	memory.setfloat(getCarPointer(vehicle)+88, z, true)
end

--------------------------------------------

function setCarVelocity(vehicle, x, y, z)
    memory.setfloat(getCarPointer(vehicle)+68, x, true)
	memory.setfloat(getCarPointer(vehicle)+72, y, true)
	memory.setfloat(getCarPointer(vehicle)+76, z, true)
end

--------------------------------------------

function getCarVelocity(vehicle, x, y, z)
    local x = memory.getfloat(getCarPointer(vehicle)+68,true)
	local y = memory.getfloat(getCarPointer(vehicle)+72,true)
	local z = memory.getfloat(getCarPointer(vehicle)+76,true)
	return x,y,z
end

--------------------------------------------

function setCarHornState(vehicle, state)
    local on = 0
    if state then on = 1 end
    memory.setuint32(getCarPointer(vehicle)+1300, on, true)
end

--------------------------------------------

function doesVehicleOccupied(vehicle)
	if doesVehicleExist(vehicle) then
		local ped = getDriverOfCar(vehicle)
		if ped == nil then return false end
		return doesCharExist(ped)
	end
	return false
end