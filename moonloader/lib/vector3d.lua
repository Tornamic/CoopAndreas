-- This file is part of SA MoonLoader package.
-- Licensed under the MIT License.
-- Copyright (c) 2016, BlastHack Team <blast.hk>

-- Vector3D class
-- authors: MTA Team (original MTA's CVector), FYP (lua implementation)


local EPSILON = 0.0001
local function isNearZero(f)
	return math.abs(f) < EPSILON
end

Vector3D = function(x, y, z)
	local mt = {}
	local obj = {
		x = x,
		y = y,
		z = z
	}

	function obj:get()
		return self.x, self.y, self.z
	end

	function obj:length()
		return math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
	end

	function obj:normalize()
		local len = self:length()
		if len < EPSILON then return 0 end
		self.x = self.x / len
		self.y = self.y / len
		self.z = self.z / len
		return len
	end

	function obj:zeroNearZero()
		if isNearZero(self.x) then self.x = 0 end
		if isNearZero(self.y) then self.y = 0 end
		if isNearZero(self.z) then self.z = 0 end
	end

	function obj:dotProduct(v)
		return self.x * v.x + self.y * v.y + self.z * v.z
	end

	function obj:crossProduct(v)
		local x, y, z = self.x, self.y, self.z
		self.x = y * v.z - v.y * z
		self.y = z * v.x - v.z * x
		self.z = x * v.y - v.x * y
	end

	obj.zero_near_zero = obj.zeroNearZero
	obj.dot_product = obj.dotProduct
	obj.cross_product = obj.crossProduct

	-- meta
	function mt:__add(v)
		return Vector3D(self.x + v.x, self.y + v.y, self.z + v.z)
	end

	function mt:__mul(v)
		if type(v) == "number" then
			return Vector3D(self.x * v, self.y * v, self.z * v)
		end
		return Vector3D(self.x * v.x, self.y * v.y, self.z * v.z)
	end

	function mt:__sub(v)
		return Vector3D(self.x - v.x, self.y - v.y, self.z - v.z)
	end

	setmetatable(obj, mt)
	return obj
end

return Vector3D
