local SLPeer = {}

local function queueSending(handle, data, addr, port)
  table.insert(handle.PacketsQueue.QueuedPackets, { Content = data, Addr = addr, Port = port })
end

function SLPeer.SLNetSend(handle, packetID, bitStream, addr, port, priority)
  priority = type(priority) == 'number' and priority or 0
  if type(handle) ~= 'table' or not handle.SockHandle then
    error('SLPeer::SLNetSend: Argument #1: handle argument shold be filled by SLNetHandle!')
    return false
  end
  if not bitStream then
    bitStream = BitStream:new()
  end
  if tostring(bitStream) ~= 'BitStream' then
    error('SLPeer::SLNetSend: Argument #2: bitstream argument should be filled by SL:BitStream!')
    return false
  end
  if type(addr) ~= 'string' then
    error('SLPeer::SLNetSend: Argument #4: address argument should be a string!')
    return false
  end
  if type(port) ~= 'number' then
    error('SLPeer::SLNetSend: Argument #5: port argument should be an integer!')
    return false
  end

  local preSend = BitStream:new()
  preSend:write(UINT32, handle.AcksData.SendID):write(UINT16, packetID):write(UINT8, priority)
  bitStream:setWritePointer(1):import(preSend:export())

  if priority > 0 then
    table.insert(handle.PacketsQueue.ConfReceiving, {
      Content = handle.VerificationPrefix .. bitStream:export() .. '\0',
      Times = priority, LastSend = os.time(), Addr = addr, Port = port,
      ID = handle.AcksData.SendID
    })
  end

  handle.AcksData.SendID = handle.AcksData.SendID + 1
  if handle.AcksData.SendID >= 4294967200 then
    handle.AcksData.SendID = 0
  end

  queueSending(handle, handle.VerificationPrefix .. bitStream:export() .. '\0', addr, port)
  return true
end

function SLPeer.SLNetReviewQueue(handle)
  while true do
    if handle and handle.SockHandle then
      local theSameSecond = false
      if os.time() == handle.PacketsQueue.LastSecondSend then
        theSameSecond = true
      else handle.PacketsQueue.PacketsPerSecond = 0 end
      if not theSameSecond or (handle.PacketsQueue.PacketsPerSecond < 50 and theSameSecond) then
        if handle.PacketsQueue.QueuedPackets[1] ~= nil then
          handle.SockHandle:sendto(handle.PacketsQueue.QueuedPackets[1].Content,
          handle.PacketsQueue.QueuedPackets[1].Addr, handle.PacketsQueue.QueuedPackets[1].Port)
          local maxSlot = #handle.PacketsQueue.QueuedPackets
          for i = 1, #handle.PacketsQueue.QueuedPackets - 1 do
            handle.PacketsQueue.QueuedPackets[i] = handle.PacketsQueue.QueuedPackets[i + 1]
          end
          handle.PacketsQueue.QueuedPackets[maxSlot] = nil
          handle.PacketsQueue.LastSecondSend = os.time()
          handle.PacketsQueue.PacketsPerSecond = handle.PacketsQueue.PacketsPerSecond + 1
        elseif handle.PacketsQueue.ConfReceiving[1] ~= nil then
          for i = #handle.PacketsQueue.ConfReceiving, 1, -1 do
            if os.time() - handle.PacketsQueue.ConfReceiving[i].LastSend >= 1 then
              queueSending(handle, handle.PacketsQueue.ConfReceiving[i].Content,
              handle.PacketsQueue.ConfReceiving[i].Addr, handle.PacketsQueue.ConfReceiving[i].Port)
              handle.PacketsQueue.ConfReceiving[i].LastSend = os.time()
              handle.PacketsQueue.ConfReceiving[i].Times = handle.PacketsQueue.ConfReceiving[i].Times - 1
              if handle.PacketsQueue.ConfReceiving[i].Times <= 0 then
                table.remove(handle.PacketsQueue.ConfReceiving, i)
              end
            end
          end
        end
      end
    end
    coroutine.yield()
  end
end

function SLPeer.SLNetReviewDatagrams(handle)
  while true do
    if handle and handle.SockHandle then
      for i = #handle.AcksData.UsedIDs, 1, -1 do
        local diff = math.abs(handle.AcksData.LastID - handle.AcksData.UsedIDs[i])
        if diff >= 300 then table.remove(handle.AcksData.UsedIDs, i) end
      end
      local data, addr, port = handle.SockHandle:receivefrom()
      if data and addr and port then
        if data:sub(1, #handle.VerificationPrefix) == handle.VerificationPrefix then
          data = BitStream:new(data:sub(#handle.VerificationPrefix + 1, #data)):setReadPointer(1):setWritePointer(1)
          local uniqeID, pID, priority = data:read(UINT32), data:read(UINT16), data:read(UINT8)
          data = BitStream:new(data:export():sub(8, #data:export())):setReadPointer(1):setWritePointer(1)

          local skipPacket = false
          for k, v in ipairs(handle.AcksData.UsedIDs) do
            if uniqeID == v then
              skipPacket = true
              break
            end
          end

          if not skipPacket then
            if priority > 0 then
              local ConfBS = BitStream:new():write(UINT32, uniqeID)
              SLPeer.SLNetSend(handle, 0, ConfBS, addr, port, 0)
            end
            if pID > 0 then handle.ReceivingHook(pID, data, addr, port)
            elseif pID == 0 then
              local confID = data:read(UINT32)
              for i = #handle.PacketsQueue.ConfReceiving, 1, -1 do
                if confID == handle.PacketsQueue.ConfReceiving[i].ID then
                  table.remove(handle.PacketsQueue.ConfReceiving, i)
                  break
                end
              end
            end
          end

        end
      end
    end
    coroutine.yield()
  end
end

return SLPeer