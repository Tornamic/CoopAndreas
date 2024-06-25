-- This file is part of SA MoonLoader package.
-- Licensed under the MIT License.
-- Copyright (c) 2016, BlastHack Team <blast.hk>


local bit = require "bit"
local int32max = 0xFFFFFFFF
local bitex = {}

function bitex.bextract(v, off, bits)
	return bit.band(bit.band(bit.rshift(v, off), bit.rshift(int32max, 32 - bits)), int32max)
end

function bitex.breplace(v, n, off, bits)
	n = bitex.bextract(n, 0, bits)
	return bit.bor(bit.band(v, bit.bnot(bit.lshift(bit.rshift(int32max, 32 - bits), off))), bit.lshift(n, off))
end

return bitex
