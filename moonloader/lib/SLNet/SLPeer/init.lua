local SLPeer = {}

function defineServer()
  local tmp = require 'SLNet.SLPeer.server'
  SLNetSend = tmp.SLNetSend
  SLNetReviewQueue = tmp.SLNetReviewQueue
  SLNetReviewDatagrams = tmp.SLNetReviewDatagrams
end
function defineClient()
  local tmp = require 'SLNet.SLPeer.client'
  SLNetSend = tmp.SLNetSend
  SLNetReviewQueue = tmp.SLNetReviewQueue
  SLNetReviewDatagrams = tmp.SLNetReviewDatagrams
end
function defCallback() return false end

function SLPeer:SLNetInit()
  local SLPeerHandle =
  {
    SockHandle = socket.udp(),
    ConnectionStatus = false,
    DefinedAsClient = false,
    AcksData =
    {
      LastID = 0,
      UsedIDs = {},
      SendID = 0
    },
    PacketsQueue =
    {
      PacketsPerSecond = 0,
      LastSecondSend = 0,
      QueuedPackets = {},
      ConfReceiving = {}
    },
    ReceivingHook = defCallback,
    Coroutines = {},
    VerificationPrefix = 'SLNET'
  }

  function SLPeerHandle:bind(addr, port)
    if type(addr) ~= 'string' then
      error('SLPeerHandle::BIND Argument #1: address argument should be a string!')
      return SLPeerHandle
    end
    if type(port) ~= 'number' then
      error('SLPeerHandle::BIND Argument #2: port argument should be an integer!')
      return SLPeerHandle
    end
    SLPeer:SLNetBind(SLPeerHandle, addr, port)
    return SLPeerHandle
  end
  function SLPeerHandle:connect(addr, port)
    if type(addr) ~= 'string' then
      error('SLPeerHandle::CONNECT Argument #1: address argument should be a string!')
      return SLPeerHandle
    end
    if type(port) ~= 'number' then
      error('SLPeerHandle::CONNECT Argument #2: port argument should be an integer!')
      return SLPeerHandle
    end
    SLPeer:SLNetConnect(SLPeerHandle, addr, port)
    return SLPeerHandle
  end
  function SLPeerHandle:setHook(func)
    if type(func) ~= 'function' then
      error('SLPeerHandle::SETHOOK Argument #1: function argument should be a callback function!')
      return SLPeerHandle
    end
    SLPeer:SLNetSetHook(SLPeerHandle, func)
    return SLPeerHandle
  end
  function SLPeerHandle:loop()
    SLPeer:SLNetLoop(SLPeerHandle)
    return SLPeerHandle
  end
  function SLPeerHandle:setPrefix(prefix)
    SLPeer:SLNetSetPrefix(SLPeerHandle, prefix)
    return SLPeerHandle
  end

  return SLPeerHandle
end

function SLPeer:SLNetSetHook(handle, func)
  if type(handle) ~= 'table' or not handle.SockHandle then
    error('SLPeer::SLNetSetHook: Argument #1: handle argument shold be filled by SLNetHandle!')
    return false
  end
  if type(func) ~= 'function' then
    error('SLPeer::SLNetSetHook: Argument #2: function argument should be a callback function!')
    return false
  end
  handle.ReceivingHook = func
  return true
end

function SLPeer:SLNetBind(handle, addr, port) -- for servers
  if type(handle) ~= 'table' or not handle.SockHandle then
    error('SLPeer::SLNetBind: Argument #1: handle argument shold be filled by SLNetHandle!')
    return false
  end
  if type(addr) ~= 'string' then
    error('SLPeer::SLNetBind: Argument #2: address argument should be a string!')
    return false
  end
  if type(port) ~= 'number' then
    error('SLPeer::SLNetBind: Argument #3: port argument should be an integer!')
    return false
  end
  if handle.ConnectionStatus and handle.DefinedAsClient then
    error('SLPeer::SLNetBind: This HANDLE is already using as client!')
    return false
  end
  handle.DefinedAsClient = false
  handle.ConnectionStatus = true
  handle.SockHandle:settimeout(0) -- non-blocking mode
  handle.SockHandle:setsockname(addr, port)
  defineServer()
  --copas.addthread(SLPeer.SLNetReviewQueue, handle)
  --copas.addthread(SLPeer.SLNetReviewDatagrams, handle)
  handle.Coroutines.SendQueue = coroutine.create(SLNetReviewQueue, handle)
  handle.Coroutines.ReceiveQueue = coroutine.create(SLNetReviewDatagrams, handle)
  return true
end

function SLPeer:SLNetConnect(handle, addr, port)
  if type(handle) ~= 'table' or not handle.SockHandle then
    error('SLPeer::SLNetConnect: Argument #1: handle argument shold be filled by SLNetHandle!')
    return false
  end
  if type(addr) ~= 'string' then
    error('SLPeer::SLNetConnect: Argument #2: address argument should be a string!')
    return false
  end
  if type(port) ~= 'number' then
    error('SLPeer::SLNetConnect: Argument #3: port argument should be an integer!')
    return false
  end
  if handle.ConnectionStatus and not handle.DefinedAsClient then
    error('SLPeer::SLNetConnect: This HANDLE is already using as server!')
    return false
  end
  handle.DefinedAsClient = true
  handle.ConnectionStatus = true
  handle.SockHandle:settimeout(0) -- non-blocking mode
  handle.SockHandle:setpeername(addr, port)
  defineClient()
  --copas.addthread(SLPeer.SLNetReviewQueue, handle)
  --copas.addthread(SLPeer.SLNetReviewDatagrams, handle)
  handle.Coroutines.SendQueue = coroutine.create(SLNetReviewQueue, handle)
  handle.Coroutines.ReceiveQueue = coroutine.create(SLNetReviewDatagrams, handle)
  return true
end

function SLPeer:SLNetLoop(handle)
  if type(handle) ~= 'table' or not handle.SockHandle then
    error('SLPeer::SLNetLoop: Argument #1: handle argument shold be filled by SLNetHandle!')
    return false
  end
  if not handle.ConnectionStatus then return false end
  local ok, err = coroutine.resume(handle.Coroutines.SendQueue, handle)
  if not ok then error(err) end
  local ok, err = coroutine.resume(handle.Coroutines.ReceiveQueue, handle)
  if not ok then error(err) end
  return true
end

function SLPeer:SLNetSetPrefix(handle, prefix)
  if type(handle) ~= 'table' or not handle.SockHandle then
    error('SLPeer::SLNetConnect: Argument #1: handle argument shold be filled by SLNetHandle!')
    return false
  end
  if type(prefix) ~= 'string' then
    error('SLPeer::SLNetSetPrefix: Argument #2: prefix argument should be a string!')
    return false
  end
  handle.VerificationPrefix = prefix
  return true
end

return SLPeer