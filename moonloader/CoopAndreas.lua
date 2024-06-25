script_name('coopandreas')
script_author('Tornamic')

local inicfg = require 'inicfg'
local cfg = inicfg.load(nil, "moonloader\\CoopAndreas.cfg")
function main()
    while not isOpcodesAvailable() do wait(1) end 
    if cfg.Settings.Type == 'host' then 
        script.load("CoopAndreas\\host.lua")
    else
        script.load("CoopAndreas\\client.lua")
    end
end