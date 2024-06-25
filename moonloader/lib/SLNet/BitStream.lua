local bit_stream = {}

local types_size = {
  [0] = 1, [1] = 2,
  [2] = 4, [3] = 1,
  [4] = 2, [5] = 4,
  [6] = 4, [7] = 1
}

local bs_class = {
  bytes = '',
  read_ptr = 1,
  write_ptr = 1
}

function bs_class:setReadPointer(x)
  x = type(x) == 'number' and x or 1
  self.read_ptr = x == 0 and 1 or x
  return self
end
function bs_class:setWritePointer(x)
  x = type(x) == 'number' and x or 1
  self.write_ptr = x == 0 and 1 or x
  return self
end
function bs_class:export()
  return self.bytes
end
function bs_class:import(x)
  x = type(x) == 'string' and x or '\0'

  local pointer = self.write_ptr
  local save_data = self.bytes:sub(pointer, #self.bytes)

  self.bytes = self.bytes:sub(1, pointer - 1)
  self.bytes = self.bytes .. tostring(x)
  self.bytes = self.bytes .. save_data

  return self
end
function bs_class:write(s_type, s_value)
  s_type = type(s_type) == 'number' and s_type or 0

  local pointer = self.write_ptr
  local save_data = self.bytes:sub(pointer, #self.bytes)
  self.bytes = self.bytes:sub(1, pointer - 1)

  if s_type == 8 then -- string
    self.bytes = self.bytes .. s_value
    self.write_ptr = self.write_ptr + #s_value
  elseif s_type == 7 then -- boolean
    s_value = type(s_value) == 'boolean' and s_value or false
    self.bytes = self.bytes .. BitCoder:encode(s_type, s_value and 1 or 0)
    self.write_ptr = self.write_ptr + types_size[s_type]
  else
    self.bytes = self.bytes .. BitCoder:encode(s_type, s_value)
    self.write_ptr = self.write_ptr + types_size[s_type]
  end

  self.bytes = self.bytes .. save_data
  return self
end
function bs_class:read(s_type, s_len)
  s_type = type(s_type) == 'number' and s_type or 0
  s_len = type(s_len) == 'number' and s_len or 0
  s_len = (s_type == 8 and s_len or types_size[s_type]) - 1

  local pointer = self.read_ptr
  local s_value = self.bytes:sub(pointer, pointer + s_len)

  self.read_ptr = self.read_ptr + s_len + 1

  if s_type == 8 then
    return s_value
  elseif s_type == 7 then
    s_value = BitCoder:decode(s_type, s_value)
    s_value = type(s_value) == 'number' and s_value or 0
    return s_value == 1 and true or false
  end

  s_value = BitCoder:decode(s_type, s_value)
  return s_value
end

bs_class.__tostring = function() return 'BitStream' end
bs_class.__index = bs_class

function bit_stream:new(value)
  local new_bitstream = {}
  setmetatable(new_bitstream, bs_class)
  if type(value) == 'string' then
    new_bitstream.bytes = value
  end
  return new_bitstream
end

return bit_stream