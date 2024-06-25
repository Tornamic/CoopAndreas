-- This file is part of SA MoonLoader package.
-- Licensed under the MIT License.
-- Copyright (c) 2016, BlastHack Team <blast.hk>

-- Matrix3X3 class
-- authors: MTA Team (original MTA's CMatrix), FYP (lua implementation)


local Vector3D = require "vector3d"

Matrix3X3 = function(rightX, rightY, rightZ, frontX, frontY, frontZ, upX, upY, upZ)
	local mt = {}
	local obj = {}
	if rightX ~= nil then
		obj.right = Vector3D(rightX, rightY, rightZ)
		obj.front = Vector3D(frontX, frontY, frontZ)
		obj.up = Vector3D(upX, upY, upZ)
	else
		obj.right = Vector3D(1, 0, 0)
		obj.front = Vector3D(0, 1, 0)
		obj.up = Vector3D(0, 0, 1)
	end

	function obj:get()
		return obj.right.x, obj.right.y, obj.right.z,
			obj.front.x, obj.front.y, obj.front.z,
			obj.up.x, obj.up.y, obj.up.z
	end

	function obj:vectorMul(v)
		return Vector3D(
			self.right.x * v.x + self.front.x * v.y + self.up.x * v.z,
			self.right.y * v.x + self.front.y * v.y + self.up.y * v.z,
			self.right.z * v.x + self.front.z * v.y + self.up.z * v.z)
	end

	function obj:rotate(v, theta)
		local tsin, tcos = math.sin(theta), math.cos(theta)
		local rightX, rightY, rightZ, frontX, frontY, frontZ, upX, upY, upZ
		-- rotate Y
		rightX = tcos + (1.0 - tcos) * v.x * v.x
		rightY = (1.0 - tcos) * v.x * v.y - tsin * v.z
		rightZ = (1.0 - tcos) * v.x * v.z + tsin * v.y
		-- rotate Y
		frontX = (1.0 - tcos) * v.y * v.x + tsin * v.z
		frontY = tcos + (1.0 - tcos) * v.y * v.y
		frontZ = (1.0 - tcos) * v.y * v.z - tsin * v.x
		-- rotate Z
		upX = (1.0 - tcos) * v.z * v.x - tsin * v.y
		upY = (1.0 - tcos) * v.z * v.y + tsin * v.x
		upZ = tcos + (1.0 - tcos) * v.z * v.z
		return Matrix3X3(rightX, rightY, rightZ, frontX, frontY, frontZ, upX, upY, upZ) * self
	end

	obj.vector_mul = obj.vectorMul

	-- meta
	function mt:__mul(m)
		local mat = Matrix3X3()
		mat.right = self:vectorMul(m.right)
		mat.front = self:vectorMul(m.front)
		mat.up = self:vectorMul(m.up)
		return mat
	end

	setmetatable(obj, mt)
	return obj
end

return Matrix3X3
