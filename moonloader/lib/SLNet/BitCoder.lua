local ffi = require 'ffi'
local bit_coder = {}

local types = {
  [0] = 'unsigned char',
  [1] = 'unsigned short',
  [2] = 'unsigned long',
  [3] = 'signed char',
  [4] = 'signed short',
  [5] = 'signed long',
  [6] = 'float',
  [7] = 'unsigned char' -- boolean
}

local function get_type(x)
  return type(x) == 'number'
  and types[x] or tostring(x)
end

local function set_value(val, type)
  local size = ffi.sizeof(type)
  local r = ffi.new(type..'[1]', val)
  return ffi.string(ffi.cast('const char*', r), ffi.sizeof(type))
end

local function get_value(src, type)
  local size = ffi.sizeof(type)
  local r = ffi.new('char[?]', size, src:byte(1, size))
  return ffi.cast(type..'*', r)[0]
end

function bit_coder:encode(s_type, s_value)
  return set_value(type(s_value) == 'number'
  and s_value or 0, get_type(s_type) or 'unsigned char')
end

function bit_coder:decode(s_type, s_value)
  return get_value(type(s_value) == 'string'
  and s_value or '\0', get_type(s_type) or 'unsigned char')
end

return bit_coder
