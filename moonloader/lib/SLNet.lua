local SLNet = {}

SLNet._COPYRIGHT = "Copyright (C) 2020 - 2020 SL:MP Team"
SLNet._DESCRIPTION = "SL:NET is Network Interface based on UDP Socket"
SLNet._VERSION = "SL:NET 0.0.2b"

local socket = require 'socket'

UINT8 = 0; UINT16 = 1; UINT32 = 2;
INT8 = 3; INT16 = 4; INT32 = 5;
FLOAT = 6; BOOL = 7; STRING = 8;

BitCoder = require 'SLNet.BitCoder'
BitStream = require 'SLNet.BitStream'
local SLPeer = require 'SLNet.SLPeer.init'

SLNetInit = SLPeer.SLNetInit
SLNetSetHook = SLPeer.SLNetSetHook
SLNetBind = SLPeer.SLNetBind
SLNetConnect = SLPeer.SLNetConnect
SLNetLoop = SLPeer.SLNetLoop
SLNetSetPrefix = SLPeer.SLNetSetPrefix

return SLNet