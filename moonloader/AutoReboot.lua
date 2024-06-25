script_name('ML-AutoReboot')
script_version_number(8)
script_version('1.1.1')
script_author('FYP')
script_description('reloads edited scripts automatically')
script_moonloader(021)
script_properties('work-in-pause', 'forced-reloading-only')

local ffi = require 'ffi'
ffi.cdef[[
	typedef void* HANDLE;
	typedef void* LPSECURITY_ATTRIBUTES;
	typedef unsigned long DWORD;
	typedef int BOOL;
	typedef const char *LPCSTR;
	typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
	} FILETIME, *PFILETIME, *LPFILETIME;

	BOOL __stdcall GetFileTime(HANDLE hFile, LPFILETIME lpCreationTime, LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);
	HANDLE __stdcall CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
	BOOL __stdcall CloseHandle(HANDLE hObject);
]]

--- Config
autoreloadDelay    = 500 -- ms
---

function main()
  -- for the reload case
  onSystemInitialized()
  while true do
    wait(autoreloadDelay)
    if files ~= nil then
      for fpath, saved_time in pairs(files) do
        local file_time = get_file_modify_time(fpath)
        if file_time ~= nil and (file_time[1] ~= saved_time[1] or file_time[2] ~= saved_time[2]) then
          local scr = find_script_by_path(fpath)
          if scr ~= nil then
            print('Reloading "' .. scr.name .. '"...')
            scr:reload()
          else
            print('Loading "' .. fpath .. '"...')
            script.load(fpath)
          end
          files[fpath] = file_time -- update time
        end
      end
    end
  end
end

function onSystemInitialized()
  if not initialized then
    init()
  end
end

function init()
  initialized = true
  files = {}
  -- store all loaded scripts
  for _, s in ipairs(script.list()) do
    local time = get_file_modify_time(s.path)
    if time ~= nil then
      files[s.path] = time
    end
  end
end

function find_script_by_path(path)
  for _, s in ipairs(script.list()) do
    if s.path == path then
      return s
    end
  end
  return nil
end

function get_file_modify_time(path)
	local handle = ffi.C.CreateFileA(path,
		0x80000000, -- GENERIC_READ
		0x00000001 + 0x00000002, -- FILE_SHARE_READ | FILE_SHARE_WRITE
		nil,
		3, -- OPEN_EXISTING
		0x00000080, -- FILE_ATTRIBUTE_NORMAL
		nil)
	local filetime = ffi.new('FILETIME[3]')
	if handle ~= -1 then
		local result = ffi.C.GetFileTime(handle, filetime, filetime + 1, filetime + 2)
		ffi.C.CloseHandle(handle)
		if result ~= 0 then
			return {tonumber(filetime[2].dwLowDateTime), tonumber(filetime[2].dwHighDateTime)}
		end
	end
	return nil
end
